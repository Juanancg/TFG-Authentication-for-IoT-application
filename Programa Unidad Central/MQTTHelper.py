# -----------------------------------------------------------------------------
# IMPORTS
# -----------------------------------------------------------------------------
import paho.mqtt.client as mqtt_client
import hmacSha256 as hmacSha256
import time
import Defines as definesValues


class MQTTHelper:

    def __init__(self):
        # -----------------------------------------------------------------------------
        # Read and save variables to start MQTT
        data = ""
        with open("config.txt") as file:
            for lines in file:
                data = lines.split(";")

        # Info needed to MQTT Broker
        # self.mqtt = MQTTHelperClass.MQTTHelper()
        self.strBrokerAddress = data[0]  # TODO - Que pasa si no lee bien el data (Hacer un if)
        self.sPort = int(data[1])
        self.strUser = data[2]
        self.strPassword = data[3]

        self.Connected = False
        self.client = mqtt_client.Client("Python")
        self.strMessageReceived = ""
        self.bMessageReceived = False

    # -----------------------------------------------------------------------------
    # Function needed to connect to the broker
    # -----------------------------------------------------------------------------
    def on_connect(self, client, userdata, flags, rc):
        if rc == 0:
            print('Conectado al servidor!')
            self.Connected = True  # TODO Forma de sabersi estas conectado todo el rato para cambiar esta var

        else:
            print('Conexion fallida')

    # -----------------------------------------------------------------------------
    # Function called when a message arrives
    # -----------------------------------------------------------------------------
    def on_message(self, client, userdata, message):
        self.bMessageReceived = True
        self.strMessageReceived = ""
        print("Mensaje recibido :", str(message.payload.decode("utf-8")))
        self.strMessageReceived = str(message.payload.decode("utf-8"))



    def on_message_LW(self, client, userdata, message):
        print("Mensaje recibido :", str(message.payload.decode("utf-8")))
        strLastWillMsg = str(message.payload.decode("utf-8"))
        if(strLastWillMsg == '1'):
            var = True
            print("ESP32 Conectado")
        elif(strLastWillMsg == '0'):
            print("ESP32 Desconectado")
            var = False


    # -----------------------------------------------------------------------------
    # Function that initializes the mqtt connection
    # -----------------------------------------------------------------------------
    def init_mqtt(self):
        # create new instance
        self.client.username_pw_set(self.strUser, password=self.strPassword)  # set username and password
        self.client.on_connect = self.on_connect  # attach function to callback
        self.client.message_callback_add("esp/LastWill", self.on_message_LW)
        self.client.on_message = self.on_message  # attach function to callback

        self.client.connect(self.strBrokerAddress, port=self.sPort)  # connect to broker
        self.client.loop_start()
        while not self.Connected:  # Wait for connection
            print("Waiting for connection")
            time.sleep(0.1)
        self.subscribe("esp/responses")
        self.subscribe("esp/LastWill")
        return 1

    # -----------------------------------------------------------------------------
    # Function to subscribe to a mqtt topic
    # -----------------------------------------------------------------------------
    def subscribe(self, topic):
        return self.client.subscribe(topic)

    # -----------------------------------------------------------------------------
    # Function to publish a message in a topic
    # -----------------------------------------------------------------------------
    def send_message(self, message):
        messageTime = message + time.strftime("%H:%M:%S")
        str_message_to_send = hmacSha256.prepare_msg(messageTime, definesValues.SECRET_KEY)
        print(str_message_to_send)
        print(self.client.publish("esp/order", str_message_to_send))
        return 1  # TODO - Retornar TRUE o FALSE en funcion si ha podido envair el mensaje o no (Mirar paho)

    # -----------------------------------------------------------------------------
    #   \brief Function that manages messages timeouts and saves the message received
    #   \return True if it receives msg, false if reach timeout
    # -----------------------------------------------------------------------------
    def wait_message(self):
        print("Waiting for response")
        count = 0
        while (count < definesValues.MSG_TIMEOUT) and (not self.bMessageReceived):
            time.sleep(0.5)
            count += 0.5

        self.bMessageReceived = False
        if count >= definesValues.MSG_TIMEOUT:
            print ("count > 60")
            return False

        return True


    # -----------------------------------------------------------------------------
    # Function that returns raw message
    # -----------------------------------------------------------------------------
    def get_raw_message(self):
        return self.strMessageReceived


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

        elif raw_message == "CLOSE":
            return definesValues.MSG_TYPES_CLOSED


        elif raw_message == "OPEN":
            return definesValues.MSG_TYPES_OPEN


        else:
            return 0

    # -----------------------------------------------------------------------------
    # Function to disconnect from the mqtt client
    # -----------------------------------------------------------------------------
    def disconnect(self):
        # self.client.publish("python/test", 'Disconnected')
        self.Connected = False
        self.client.disconnect()
        self.client.loop_stop()



