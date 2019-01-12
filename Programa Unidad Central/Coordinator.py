# -----------------------------------------------------------------------------
# IMPORTS
# -----------------------------------------------------------------------------
import time
import json
import MQTTHelper as MQTTHelperClass
import hmacSha256 as hmacSha256
import Defines as definesValues


class Coordinator:

    def __init__(self):
        # -----------------------------------------------------------------------------
        # Read and save variables to start MQTT
        data = ""
        with open("test.txt") as file:
            for lines in file:
                data = lines.split(";")

        # Info needed to MQTT Broker
        self.mqtt = MQTTHelperClass.MQTTHelper()
        self.strBrokerAddress = data[0]  # TODO - Que pasa si no lee bien el data (Hacer un if)
        self.sPort = int(data[1])
        self.strUser = data[2]
        self.strPassword = data[3]

        # Variable
        self.sUserPetition = 0

        # Variables for message management
        self.strTempMsgHashed = ""
        self.strMessage = ""
        self.sTypeMsg = 0  # 0 Nothing - 1 Ping - 2 Status - 3 Closed - 4 Open

        # Variables for Status Message
        self.strMessageStatusToShow = ""
        self.photodiodeValue = 0
        self.fdcOpenValue = 0
        self.fdcClosedValue = 0
        self.axisXValue = 0.0
        self.axisYValue = 0.0
        self.axisZValue = 0.0

    # -----------------------------------------------------------------------------
    # Initialization calls to start MQTT
    # -----------------------------------------------------------------------------
    def b_init_mqtt(self):

        self.mqtt.init_mqtt(self.strBrokerAddress, self.sPort, self.strUser, self.strPassword)
        while not self.mqtt.Connected:  # Wait for connection
            print("Waiting for connection")
            time.sleep(0.1)
        print(self.mqtt.subscribe("esp/responses"))
        return 1  # TODO - Timeouts to return true o false

    # -----------------------------------------------------------------------------
    # Function to catch user entry and validates it
    # -----------------------------------------------------------------------------
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
    # Function that checks the authentity reads message and clasifies it
    # -----------------------------------------------------------------------------
    def message_reader(self, raw_message):
        # 1 - Check authentication
        if hmacSha256.check_authentication(raw_message, "secretKey"):
            self.strMessage = hmacSha256.get_msg(raw_message)

            # 2 - Identify the message
            if self.strMessage == "PING":
                self.sTypeMsg = definesValues.MSG_TYPE_PING

            elif self.strMessage[2:8] == "STATUS": # JSON message at the beginning has {" so the info starts at 2nd char
                self.sTypeMsg = definesValues.MSG_TYPE_STATUS
                print("He recibido un mensaje de STATUS")
                
            elif self.strMessage == "CLOSED":
                self.sTypeMsg = definesValues.MSG_TYPES_CLOSED
                print("He recibido un mensaje de CLOSED")
                # TODO - CLOSED MSG

            elif self.strMessage == "OPEN":
                self.sTypeMsg = definesValues.MSG_TYPES_OPEN
                print("He recibido un mensaje de OPEN")
                # TODO - OPEN MSG

            else:
                self.sTypeMsg = 0 # TODO - QUE PASA SI RECIBE MSG DESCONOCIDO
                print("Mensaje desconocido")
            return True
        else:
            # Message no authentic
            return False

    # -----------------------------------------------------------------------------
    # Function that decode Status Message
    # -----------------------------------------------------------------------------
    def decode_Status_msg(self, raw_message):
        # Decoded message to dict type
        decoded = json.loads(raw_message)
        # Save values to check
        try:
            self.photodiodeValue = decoded["STATUS"]["PHOTODIODE"]
            self.fdcOpenValue = decoded["STATUS"]["FDC"]["FDC_O"]
            self.fdcClosedValue = decoded["STATUS"]["FDC"]["FDC_C"]
            self.axisXValue = decoded["STATUS"]["ANGLE"]["X_AXIS"]
            self.axisYValue = decoded["STATUS"]["ANGLE"]["Y_AXIS"]
            self.axisZValue = decoded["STATUS"]["ANGLE"]["Z_AXIS"]
            return 1
        # Interruption if cant read any parameter of JSON msg
        except KeyError:
            return definesValues.ERROR_READ_JSON

    # -----------------------------------------------------------------------------
    # Function that analyzes photodiode value
    # -----------------------------------------------------------------------------
    def analyze_Photodiode_Value(self, value):
        if value < definesValues.PHOTODIODE_MAX_VALUE:
            return False
        else:
            return True

    # -----------------------------------------------------------------------------
    # Function that analyzes the values of status msg and compose external msg
    # -----------------------------------------------------------------------------
    def compose_status(self):
        # Check if it can read JSON message
        if self.decode_Status_msg(self.strMessage) == 1:
            self.strMessageStatusToShow = "STATUS: \n"
            self.strMessageStatusToShow = self.strMessageStatusToShow + "Photodiode Value: " + str(self.photodiodeValue)
            if self.analyze_Photodiode_Value(self.photodiodeValue):
                self.strMessageStatusToShow = self.strMessageStatusToShow + " - Invalid to open the cap"
            else:
                self.strMessageStatusToShow = self.strMessageStatusToShow + " - Valid to open the cap"
            self.strMessageStatusToShow = self.strMessageStatusToShow + "\nOpen: " + str(self.fdcOpenValue) + \
                                          "\nClosed: " + str(self.fdcClosedValue) + "\nX Axis: " + \
                                          str(self.axisXValue) + "\nY Axis: " + str(self.axisYValue) + \
                                          "\nZ Axis: " + str(self.axisZValue)

            return self.strMessageStatusToShow 
        else:
            return "Cant read JSON msg"

    # -----------------------------------------------------------------------------
    # Function that manages user petition
    # Return value:
    #   1  - Everything Ok
    #   0  - Different message type than expected
    #   -1 - No authentic
    #   -2 - Timeout
    # -----------------------------------------------------------------------------
    def expected_message(self, msg_type):
        if self.mqtt.wait_message():
            # Check authenticity
            if self.message_reader(self.mqtt.get_raw_message()):
                # Check if the message is the expected
                if self.sTypeMsg == msg_type:
                    return 1
                else:
                    return definesValues.ERROR_NOT_MESSAGE_EXPECTED
            else:
                return definesValues.ERROR_NOT_MESSAGE_AUTHENTIC
        else:
            return definesValues.ERROR_TIMEOUT

    # -----------------------------------------------------------------------------
    # Function that manages user petition
    # -----------------------------------------------------------------------------
    def petition_manager(self):
        self.s_user_petition()
        if self.sUserPetition != 4:
            # Case 1 - Get Status
            if self.sUserPetition == 1:
                self.mqtt.send_message('PING')
                if self.expected_message(definesValues.MSG_TYPE_PING) == 1:
                    self.mqtt.send_message('GETSTATUS')
                    if self.expected_message(definesValues.MSG_TYPE_STATUS) == 1:
                        print(self.compose_status())

