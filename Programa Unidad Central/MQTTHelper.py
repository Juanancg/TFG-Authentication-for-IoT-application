# -----------------------------------------------------------------------------
# IMPORTS
# -----------------------------------------------------------------------------
import paho.mqtt.client as mqtt_client
import hmacSha256 as hmacSha256
import time


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
        self.bMessageReceived = True  # **** ESTO ES NUEVO Y NO ESTA PROBADO
        print("Mensaje recibido :", str(message.payload.decode("utf-8")))
        self.strMessageReceived = str(message.payload.decode("utf-8"))  # **** ESTO ES NUEVO Y NO ESTA PROBADO

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
    # Function that manages messages timeouts and saves the message received
    # -----------------------------------------------------------------------------
    def wait_message(self):
        print("Waiting for response")
        while not self.bMessageReceived:
            # print(".")
            time.sleep(0.5)
        # .strTempMsgHashed = self.mqtt.strMessageReceived
        # print(self.strMessageReceived)
        self.bMessageReceived = False
        return 1  # TODO - Return true or false based on Timeouts

    # -----------------------------------------------------------------------------
    # Function that returns raw message
    # -----------------------------------------------------------------------------
    def get_raw_message(self):
        return self.strMessageReceived

    # -----------------------------------------------------------------------------
    # Function to disconnect from the mqtt client
    # -----------------------------------------------------------------------------
    def disconnect(self):
        self.client.publish("python/test", 'Disconnected')
        self.Connected = False
        self.client.disconnect()
        self.client.loop_stop()
