# -----------------------------------------------------------------------------
# IMPORTS
# -----------------------------------------------------------------------------
import time
import adapted_mqtt as mqtt
import hmacSha256 as hmacSha256


class Coordinator:

    def __init__(self):

        # -----------------------------------------------------------------------------
        # Read and save variables to start MQTT
        with open("test.txt") as file:
            for lines in file:
                data = lines.split(";")

        # Info needed to MQTT Broker
        self.strBrokerAddress = data[0]
        self.sPort = int(data[1])
        self.strUser = data[2]
        self.strPassword = data[3]

        # Variable
        self.sUserPetition = 0

        # Variables for message management
        self.strTempMsgHashed = ""
        self.strMessage = ""
        self.sTypeMsg = 0  # 0 Nothing - 1 Ping - 2 Status - 3 Closed - 5 Open


    # -----------------------------------------------------------------------------
    # Initialization calls to start MQTT
    def b_init_mqtt(self):

        mqtt.init_mqtt(self.strBrokerAddress, self.sPort, self.strUser, self.strPassword)
        while not mqtt.Connected:  # Wait for connection
            time.sleep(0.1)
        print(mqtt.subscribe("esp/responses"))
        return 1  # TODO - Timeouts to return true o false


    # -----------------------------------------------------------------------------
    # Function to catch user entry and validates it
    def s_user_petition(self):
        self.sUserPetition = 0
        while not self.sUserPetition:
            try:
                self.sUserPetition = int(input("1) Get Status \n"
                                               "2) Open the cap \n"
                                               "3) Close the cap \n"
                                               "4) Exit\n"))
                if not 1 <= self.sUserPetition <= 4:
                    raise ValueError
            except ValueError:
                self.sUserPetition = 0
                print("Invalid option")

        return self.sUserPetition


    # -----------------------------------------------------------------------------
    # Function that manages messages timeouts and saves the message received
    def message_timeout(self):
        while not mqtt.bMessageReceived:
            print(".")
            time.sleep(0.5)
        self.strTempMsgHashed = mqtt.strMessageReceived
        mqtt.bMessageReceived = False
        return 1  # TODO - Return true or false based on Timeouts


    # -----------------------------------------------------------------------------
    # Function that returns
    def get_message_hashed(self):
        return self.strTempMsgHashed


    # -----------------------------------------------------------------------------
    # Function that reads message and acts in function
    def message_reader(self, raw_message):
        # 1 - Check authentication
        if hmacSha256.check_authentication(raw_message, "secretKey"):
            self.strMessage = hmacSha256.get_msg(raw_message)
            if strMessage == "PING":
                self.sTypeMsg = 1
            elif strMessage == "STATUS":
                self.sTypeMsg = 2
                print("He recibido un mensaje de STATUS")
                # TODO - GET INFO
            elif strMessage == "CLOSED":
                self.sTypeMsg = 3
                print("He recibido un mensaje de CLOSED")
                # TODO - CLOSED MSG
            elif strMessage == "OPEN":
                self.sTypeMsg = 4
                print("He recibido un mensaje de OPEN")
                # TODO - OPEN MSG
            else:
                self.sTypeMsg = 0
                print("Mensaje desconocido")
            return 1
        else:
            # Message no authentic
            return 0


    # -----------------------------------------------------------------------------
    # Function that manages user petition
    def petition_manager(self):
        if self.sUserPetition != "4":
            # Case 1 - Get Status
            if self.sUserPetition == "1":
                strPingMsgToSend = hmacSha256.prepare_msg("PING", "secretKey")
                print(mqtt.publish("esp/order", strPingMsgToSend))
                print("Waiting for response")
                if self.message_timeout():
                    if self.message_reader(self.strTempMsgHashed):
