# -----------------------------------------------------------------------------
# IMPORTS
# -----------------------------------------------------------------------------
import time
import adapted_mqtt as mqtt
import hmacSha256 as hmacSha256

Connected = False  # global variable for the state of the connection


# -----------------------------------------------------------------------------
# Read and save variables to start MQTT
with open("test.txt") as file:
    for lines in file:
        data = lines.split(";")

strBrokerAddress = data[0]
sPort = int(data[1])
strUser = data[2]
strPassword = data[3]

# -----------------------------------------------------------------------------
# Initialization calls to start MQTT
mqtt.init_mqtt(strBrokerAddress, sPort, strUser, strPassword)
while mqtt.Connected:  # Wait for connection
    time.sleep(0.1)
print(mqtt.subscribe("esp/responses"))


# -----------------------------------------------------------------------------
# Main section

i = 0
try:
    while i == 0:
        strUserPetition = input("1) Get Status \n"
                              "2) Open the cap \n"
                              "3) Close the cap \n"
                              "4) Exit\n")
        if strUserPetition != "4":  # In the future, change this to switcher, in other class

            if strUserPetition == "1":
                strPingMsg = hmacSha256.prepare_msg("PING", "secretKey")
                print(mqtt.publish("esp/order", strPingMsg))

        else:
            exit()
        """time.sleep(1)  # para que procese el callback
        hmac_main = hmacSha256.prepare_msg('PINGasdas', 'secretKey')
        checked = hmacSha256.check_authentication(hmac_main, 'secretKey1')
        print(hmac_main)
        print(checked)"""
        #  mqtt.publish("esp/order", hmac_main)
        # print(mensaje)
        #  **** AQUI PONER COSAS PARA CUANDO RECIBA MENSAJE
        #  mensaje = hmacSha256.prepare_msg('GET', 'secretKey')
        #  mqtt.publish("esp/order", mensaje)
        # i = 1
except KeyboardInterrupt:
    print('Desconectado')
    mqtt.disconnect()
