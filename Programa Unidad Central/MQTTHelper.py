# -----------------------------------------------------------------------------
# IMPORTS
# -----------------------------------------------------------------------------
import paho.mqtt.client as mqtt_client
import hmacSha256 as hmacSha256
import time
import Defines as definesValues


class MQTTHelper:

    def __init__(self):
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

    # -----------------------------------------------------------------------------
    # Function that initializes the mqtt connection
    # -----------------------------------------------------------------------------
    def init_mqtt(self, broker_address, port, user, password):
        # create new instance
        self.client.username_pw_set(user, password=password)  # set username and password
        self.client.on_connect = self.on_connect  # attach function to callback
        self.client.on_message = self.on_message  # attach function to callback

        self.client.connect(broker_address, port=port)  # connect to broker
        self.client.loop_start()

    # -----------------------------------------------------------------------------
    # Function to subscribe to a mqtt topic
    # -----------------------------------------------------------------------------
    def subscribe(self, topic):
        return self.client.subscribe(topic)

    # -----------------------------------------------------------------------------
    # Function to publish a message in a topic
    # -----------------------------------------------------------------------------
    def send_message(self, message):
        str_message_to_send = hmacSha256.prepare_msg(message, 'secretKey')
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

    # -----------------------------------------------------------------------------
    # Function to disconnect from the mqtt client
    # -----------------------------------------------------------------------------
    def disconnect(self):
        # self.client.publish("python/test", 'Disconnected')
        self.Connected = False
        self.client.disconnect()
        self.client.loop_stop()



