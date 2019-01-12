# -----------------------------------------------------------------------------
# IMPORTS
# -----------------------------------------------------------------------------
import Coordinator as CoordinatorClass
import hmacSha256 as hmacSha256

data = "{\"STATUS\":{\"PHOTODIODE\":4095,\"FDC\":{\"FDC_O\":1,\"FDC_C\":0},\"ANGLE\":{\"X_AXIS\":-0.54,\"Y_AXIS\":-0.13,\"Z_AXIS\":0.77}}}"


coordinator = CoordinatorClass.Coordinator()

# Init MQTT broker
coordinator.b_init_mqtt()

coordinator.petition_manager()




# -----------------------------------------------------------------------------
# Main section
"""
i = 0
try:
    while i == 0:
        sUserPetition = coordinator.s_user_petition()

        if sUserPetition != "4":  # In the future, change this to switcher, in other class
            
            if strUserPetition == "1":
                strPingMsg = hmacSha256.prepare_msg("PING", "secretKey")
                print(mqtt.publish("esp/order", strPingMsg))
                print("Waiting for response")

                while not mqtt.bMessageReceived:
                    print(".")
                    time.sleep(0.5)
                strTempMsg = mqtt.strMessageReceived
                mqtt.bMessageReceived = False
                if hmacSha256.check_authentication(strTempMsg, "secretKey"):
                    strMessage = hmacSha256.get_msg(strTempMsg)
                    if strMessage == "PING":
                        strStatusMsg = hmacSha256.prepare_msg("Status", "secretKey")
                        mqtt.publish("esp/order", strStatusMsg)
                        print("Waiting for response")

                        while not mqtt.bMessageReceived:
                            print(".")
                            time.sleep(0.5)
                        strTempMsg = mqtt.strMessageReceived
                        mqtt.bMessageReceived = False

        else:
            exit()
        time.sleep(1)  # para que procese el callback
        hmac_main = hmacSha256.prepare_msg('PINGasdas', 'secretKey')
        checked = hmacSha256.check_authentication(hmac_main, 'secretKey1')
        print(hmac_main)
        print(checked)
        #  mqtt.publish("esp/order", hmac_main)
        # print(mensaje)
        #  **** AQUI PONER COSAS PARA CUANDO RECIBA MENSAJE
        #  mensaje = hmacSha256.prepare_msg('GET', 'secretKey')
        #  mqtt.publish("esp/order", mensaje)
        # i = 1
except KeyboardInterrupt:
    print('Desconectado')
    mqtt.disconnect()"""
