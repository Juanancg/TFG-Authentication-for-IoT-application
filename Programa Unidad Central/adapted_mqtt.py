import paho.mqtt.client as mqttclient

Connected = False  # global variable for the state of the connection
client = mqttclient.Client("Python")
received_msg = ""
flag_msg_received = 0


# Function needed to connect to the broker
# -----------------------------------------------------------------------------

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print('Conectado al servidor!')

        global Connected  # Use global variable
        Connected = True  # Signal connection

    else:
        print('Conexion fallida')


# Function called when a message arrives
# -----------------------------------------------------------------------------

def on_message(client, userdata, message):
    global flag_msg_received  # **** ESTO ES NUEVO Y NO ESTA PROBADO
    flag_msg_received = 1  # **** ESTO ES NUEVO Y NO ESTA PROBADO
    print("Mensaje recibido :", str(message.payload.decode("utf-8")))
    global received_msg  # **** ESTO ES NUEVO Y NO ESTA PROBADO
    received_msg = str(message.payload.decode("utf-8"))  # **** ESTO ES NUEVO Y NO ESTA PROBADO


# Function that initializes the mqtt connection
# -----------------------------------------------------------------------------

def init_mqtt(broker_address, port, user, password):
    # create new instance
    client.username_pw_set(user, password=password)  # set username and password
    client.on_connect = on_connect  # attach function to callback
    client.on_message = on_message  # attach function to callback

    client.connect(broker_address, port=port)  # connect to broker
    client.loop_start()


# Function to subscribe to a mqtt topic
# -----------------------------------------------------------------------------

def subscribe(topic):
    return client.subscribe(topic)


# Function to publish a message in a topic
# -----------------------------------------------------------------------------

def publish(topic, message):
    return client.publish(topic, message, 2, False)


# Function to disconnect from the mqtt client
# -----------------------------------------------------------------------------

def disconnect():
    client.publish("python/test", 'Disconnected')
    client.disconnect()
    client.loop_stop()
