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


    def b_init_mqtt(self):
        """ Initializes the mqtt object with the appropriate parameters
        Args:
            None

        Returns:
            The define value of the kind of message that has received
        """

        self.mqtt.init_mqtt(self.strBrokerAddress, self.sPort, self.strUser, self.strPassword)
        while not self.mqtt.Connected:  # Wait for connection
            print("Waiting for connection")
            time.sleep(0.1)
        print(self.mqtt.subscribe("esp/responses"))
        return 1  # TODO - Timeouts to return true o false


    def message_reader(self, raw_message):
        """ Reads the message given and classifies it
        Args:
            raw_message: The message to be classifies

        Returns:
            The define value of the kind of message that has received
        """

        if raw_message == "PING":
            return definesValues.MSG_TYPE_PING

        elif raw_message[2:8] == "STATUS": # JSON message at the beginning has {" so the info starts at 2nd char
            return definesValues.MSG_TYPE_STATUS

        elif raw_message == "CLOSED":
            return definesValues.MSG_TYPES_CLOSED
            # TODO - CLOSED MSG

        elif raw_message == "OPEN":
            return definesValues.MSG_TYPES_OPEN
            # TODO - OPEN MSG

        else:
            return 0 # TODO - QUE PASA SI RECIBE MSG DESCONOCIDO



    # -----------------------------------------------------------------------------
    # Function that decode Status Message
    # -----------------------------------------------------------------------------
    def decode_Status_msg(self, raw_message):
        # Decoded message to dict type
        decoded = json.loads(raw_message)
        # Save values to check
        try:
            self.photodiodeValue = decoded["STATUS"]["PHOTODIODE"]
            self.fdcOpenValue = decoded["STATUS"]["LimitSwitch"]["LimitSwitch_O"]
            self.fdcClosedValue = decoded["STATUS"]["LimitSwitch"]["LimitSwitch_C"]
            self.axisXValue = decoded["STATUS"]["ANGLES"]["X_AXIS"]
            self.axisYValue = decoded["STATUS"]["ANGLES"]["Y_AXIS"]
            self.axisZValue = decoded["STATUS"]["ANGLES"]["Z_AXIS"]
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
    # Function that analyzes angles values, Z value isnt necessary because of
    # rotation in Z isnt relevant for the cap
    # -----------------------------------------------------------------------------
    def analyze_Angles_values(self):
        boolean = True
        if self.axisXValue < definesValues.ANGLE_X_MAX_VALUE:
            boolean = boolean * True
        else:
            print("X axis invalid")
            boolean = boolean * False
        if self.axisYValue < definesValues.ANGLE_Y_MAX_VALUE:
            boolean = boolean * True
        else:
            print("Y axis invalid")
            boolean = boolean * False
        return boolean


    # -----------------------------------------------------------------------------
    # Function that analyzes the values of status msg and compose external msg
    # -----------------------------------------------------------------------------
    def compose_status(self):
        strMessageStatusToShow1 = "STATUS: \n"
        strMessageStatusToShow1 = strMessageStatusToShow1 + "Photodiode Value: " + str(self.photodiodeValue)
        if self.analyze_Photodiode_Value(self.photodiodeValue):
            strMessageStatusToShow1 = strMessageStatusToShow1 + " - Invalid to open the cap"
        else:
            strMessageStatusToShow1 = strMessageStatusToShow1 + " - Valid to open the cap"
        strMessageStatusToShow1 = strMessageStatusToShow1 + "\nOpen: " + str(self.fdcOpenValue) + \
                                      "\nClosed: " + str(self.fdcClosedValue) + "\nX Axis: " + \
                                      str(self.axisXValue) + "\nY Axis: " + str(self.axisYValue) + \
                                      "\nZ Axis: " + str(self.axisZValue)

        return strMessageStatusToShow1


    def checkStatus(self, statusmsg, open):
        if self.decode_Status_msg(statusmsg) == 1:
            # First checks the Limit Switches
            if(self.fdcOpenValue != open or self.fdcClosedValue == open):
                # To open the cap its needed to check more conditions

                if open == 1:
                    # Second checks the Photodiode value

                    if self.analyze_Photodiode_Value(self.photodiodeValue):
                        # Last checks the angles

                        if self.analyze_Angles_values():
                            return True
                        else:
                            return False

                    else:
                        print("Photodiode value doesnt allow to open the cap")
                        return False

                else:
                    return True

            else:
                if open == 1:
                    print("The Limit Switches said that the cap is open")
                else:
                    print("The Limit Switches said that the cap is close")

                return False
        else:
            return definesValues.ERROR_READ_JSON



    def get_actual_time(self):
        strings = time.strftime("%Y,%m,%d,%H,%M,%S")
        t = strings.split(',')
        return t[3]+":"+t[4]+":"+t[5]+" "+t[2]+"/"+t[1]+"/"+t[0]



