# -----------------------------------------------------------------------------
# IMPORTS
# -----------------------------------------------------------------------------
import Coordinator as CoordinatorClass
import Defines as definesValues
import hmacSha256 as hmacSha256

class StateMachine:

    def __init__(self):
        self.coordinator = CoordinatorClass.Coordinator()
        self.sUserPetition = 0
        self.state = 0
        self.lastState = 0
        self.timeoutNumber = 0
        self.lastStatusMessage = ""
        self.coordinator.b_init_mqtt()

    # -----------------------------------------------------------------------------
    # Function to catch user entry and validates it
    # Estado 0
    # -----------------------------------------------------------------------------
    def user_petition_S0(self, userPetition):
        print("Estado 1")
        userPetition = 0
        while not userPetition:
            try:
                userPetition = int(input("1) Get Status \n"
                                               "2) Open the cap \n"
                                               "3) Close the cap \n"
                                               "4) Exit\n"))
                if not 1 <= userPetition <= 4:
                    raise ValueError
            except ValueError:
                userPetition = 0
                print("Invalid option")
        if userPetition == 0:
            self.state = 0
        elif userPetition == 4:
            self.coordinator.mqtt.disconnect()
            exit()
        else:
            self.state = 1
        self.sUserPetition = userPetition
        self.lastState = 0


    def pingMessage_S1(self, userPetition):
        """ First state: Send the PING msg

        Send PING message to the ESP32 to see if is online

        Args:
            userPetition: Petition introduced by the Python user

        """
        print("Estado 1")
        self.coordinator.mqtt.send_message('PING')
        self.lastState = 1
        self.state = 2


    def waitForMessage_S2(self, userPetition):
        """ Second state: Wait for a incoming message

        When the message arrives, it checks the authenticity of the message. Then, if its correct, it reads
        the message without the HMAC and if it is the message expected, it goes to the next state. If not,
        it goes to the zero state

        Args:
            userPetition: Petition introduced by the Python user
        """
        print("Estado 2")
        msg_type = 0
        if self.lastState == 1:
            msg_type = definesValues.MSG_TYPE_PING
        elif self.lastState == 3:
            msg_type = definesValues.MSG_TYPE_STATUS


        if msg_type == 0:
            self.lastState = 2
            self.state = 0

        elif self.coordinator.mqtt.wait_message():

            raw_Message = self.coordinator.mqtt.get_raw_message()

            # Check the authenticity
            if hmacSha256.check_authentication(raw_Message, "secretKey"):

                # Check if the message is the expected
                if self.coordinator.message_reader(hmacSha256.get_msg(raw_Message)) == msg_type:

                    # Then, depending of the last state, activate the next state
                    if self.lastState == 1:
                        self.state = 3

                    elif self.lastState == 3:
                        self.lastStatusMessage = hmacSha256.get_msg(raw_Message)

                        if self.coordinator.decode_Status_msg(self.lastStatusMessage) == 1:
                            self.lastStatusMessageTime = self.coordinator.get_actual_time()
                            self.state = 4
                        else:
                            print("Error - Cant decode JSON message")
                            self.state = 0

                    else:
                        self.state = 0
                else:
                    print("Error - No msg expected")
                    self.state = 0
            else:
                print("Error - No authentic")
                self.state = 0
        else:
            print("Error - Timeout")
            self.state = 0
        self.lastState = 2


    def getStatusMessage_S3(self, userPetition):
        """ Third state: Send the GETSTATUS msg

        Send the get status message

        Args:
            userPetition: Petition introduced by the Python user

        """
        print("Estado 3")
        self.coordinator.mqtt.send_message('GETSTATUS')
        self.lastState = 3
        self.state = 2


    def whichPetition_S4(self, userPetition):
        """ Fourth state: Checks the initial user petition

        Checks what option has chosen the user and goes to the corresponding state

        Args:
            userPetition: Petition introduced by the Python user

        """
        print("Estado 4")
        if userPetition == 1:
            self.state = 5
        elif userPetition == 2:
            self.state = 6
        elif userPetition == 3:
            self.state = 7
        else:
            print("Invalid petition")
            self.state = 0
        self.lastState = 4


    def printStatusMsg_S5(self, userPetition):
        """ Fifth state: Prints the Status of the ESp32

        The user wants to see the current Status of the ESP32, so this state has the purpose of print it

        Args:
            userPetition: Petition introduced by the Python user

        """
        print("Estado 5")
        print(self.coordinator.compose_status())
        print(self.lastStatusMessageTime)
        self.lastState = 5
        self.state = 0




    # -----------------------------------------------------------------------------
    # Function that manages the state machine
    # -----------------------------------------------------------------------------
    def StartStateMachine(self):

        switch = {
            0: self.user_petition_S0,
            1: self.pingMessage_S1,
            2: self.waitForMessage_S2,
            3: self.getStatusMessage_S3,
            4: self.whichPetition_S4,
            5: self.printStatusMsg_S5,
        }
        func = switch.get(self.state, lambda: None)
        return func(self.sUserPetition)