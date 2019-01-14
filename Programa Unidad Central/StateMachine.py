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
        self.oldState = 0
        self.timeoutNumber = 0
        self.coordinator.b_init_mqtt()

    # -----------------------------------------------------------------------------
    # Function to catch user entry and validates it
    # Estado 0
    # -----------------------------------------------------------------------------
    def user_petition_S0(self, userPetition):
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
            self.oldState = 0
            self.state = 0
        if userPetition == 1:
            self.oldState = 0
            self.state = 1



    def pingMessage_S1(self, userPetition):
        print("he llegado al ping")
        self.coordinator.mqtt.send_message('PING')
        self.oldState = 1
        self.state = 2


    def waitForMessage_S2(self, userPetition):
        """ Second state: Wait for a incoming message

        When

        Args:
            userPetition: Petition introduced by the Python user

        Returns:
            The define value of the kind of message that has received
        """
        msg_type = 0
        if self.oldState == 1:
            print("El mensaje anterior es PING")
            msg_type = definesValues.MSG_TYPE_PING

        if msg_type == 0:
            self.oldState = 2
            self.state = 0

        elif self.coordinator.mqtt.wait_message():
            # Check authenticity
            print("Paso por aqui")
            strMessage = self.coordinator.mqtt.get_raw_message()
            print(strMessage)
            if hmacSha256.check_authentication(strMessage, "secretKey"):

                # Check if the message is the expected
                if self.coordinator.message_reader(hmacSha256.get_msg(strMessage)) == msg_type:

                    if self.oldState == 1:
                        self.state = 0
                        print("He llegado hasta el final")

                    elif self.oldState == 3:
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
        self.oldState = 2

    # -----------------------------------------------------------------------------
    # Function that manages the state machine
    # -----------------------------------------------------------------------------
    def StartStateMachine(self):

        switch = {
            0: self.user_petition_S0,
            1: self.pingMessage_S1,
            2: self.waitForMessage_S2
            #3: self.
            #4: self.
        }
        func = switch.get(self.state, lambda: None)
        return func(self.sUserPetition)