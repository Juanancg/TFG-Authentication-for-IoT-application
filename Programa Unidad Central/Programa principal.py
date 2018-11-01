import adapted_mqtt as mqtt
import time
import hmacSha256 as hmac_sha256

Connected = False  # global variable for the state of the connection


f = open("test.txt", "r")
file = f.read()
lines = file.split(";")

""" MQTT - Utilizando CloudMQTT """
broker_address = lines[0]
port = int(lines[1])
user = lines[2]
password = lines[3]

mqtt.init_mqtt(broker_address, port, user, password)

while mqtt.Connected:  # Wait for connection
    time.sleep(0.1)

mqtt.subscribe("esp/responses")
i = 0
try:
    while i == 0:
        time.sleep(1)  # para que procese el callback
        hmac_main = hmac_sha256.prepare_msg('PINGasdas', 'secretKey')
        # check_authentication

        #  mqtt.publish("esp/order", hmac_main)
        # print(mensaje)
        #  **** AQUI PONER COSAS PARA CUANDO RECIBA MENSAJE
        #  mensaje = hmac_sha256.prepare_msg('GET', 'secretKey')
        #  mqtt.publish("esp/order", mensaje)
        i = 1
except KeyboardInterrupt:
    print('Desconectado')

    mqtt.disconnect()
    #  client.loop_stop()
