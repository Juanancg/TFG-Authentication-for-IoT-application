import paho.mqtt.client as mqttClient

Connected = False  # global variable for the state of the connection
client = mqttClient.Client("Python")
mensaje_recibido = ""
flag_msg_recibido = 0


def on_connect(client, userdata, flags, rc):
    if rc == 0:

        print('Conectado al servidor!')

        global Connected  # Use global variable
        Connected = True  # Signal connection

    else:

        print('Conexion fallida')


def on_message(client, userdata, message):
    global flag_msg_recibido  # **** ESTO ES NUEVO Y NO ESTA PROBADO
    flag_msg_recibido = 1  # **** ESTO ES NUEVO Y NO ESTA PROBADO
    print("Mensaje recibido :", str(message.payload.decode("utf-8")))
    global mensaje_recibido  # **** ESTO ES NUEVO Y NO ESTA PROBADO
    mensaje_recibido = str(message.payload.decode("utf-8"))  # **** ESTO ES NUEVO Y NO ESTA PROBADO


def init_mqtt(broker_address, port, user, password):
    # create new instance
    client.username_pw_set(user, password=password)  # set username and password
    client.on_connect = on_connect  # attach function to callback
    client.on_message = on_message  # attach function to callback

    client.connect(broker_address, port=port)  # connect to broker
    client.loop_start()


def subscribe(topic):
    client.subscribe(topic)


def publish(topic, mensaje):
    client.publish(topic, mensaje)


def disconnect():
    client.publish("python/test", 'Desconectado')
    client.disconnect()
    client.loop_stop()
