# TFG Authentication for IoT application
Python and ESP32 files

El proyecto ha sido probado en la versión 3.6 de Pyhton y la 1.8.3 de Arduino.

Para lanzar el programa central, hay que abrir el archivo "*Programa principal.py*" y darle a F5 (Run Module) desde el IDLE de Python 

Para lanzar el nodo sensor, conectar el ESP32 al ordenador, abrir el Arduino IDE previamente configurado y subir el programa (Ver la sección programación del ESP32. Una vez subido, abrir un monitor serial, y observar como se conecta a la WiFi y luego al MQTT. Si tarda mucho en conectarse a la WiFi, darle al botón de reset o volver a subir el programa. Debe tener la alimentación conectada para que se conecte a la WiFi.


## Programación del ESP32:
La programación del ESP32 se realiza a traves del IDE de Arduino y conectando el ESP32 al ordenador a traves de un cable microusb
Para la utilización del ESP32 con el IDE de arduino, se debe configurar previamente el entorno. 
Seguir los pasos del [tutorial oficial][tuto].

Para subir el código al módulo ESP32, debe conectar el ESP32 mediante la conexión USB, darle a "Subir" y **pulsar el botón de boot de la placa**, hasta que vea que el log de arduino que se está subiendo el programa.


## Clave secreta:
La clave secreta utilizada para el cifrado de mensajes debe ser la **MISMA** en ambos sistemas (ESP32 y Python)
	
- **Python**: En el archivo "*Defines.py*" se encuentra la clave secreta en `SECRET_KEY`.
- **ESP32**:  En el archivo "*Coordinator.h*" hay un define llamado `SECRET_KEY`.
	

## Ajustar los valores críticos:
Los valores críticos son el valor del fotodiodo y del ángulo del eje X reportado por el MPU6050:

- **ESP32**: En el archivo "*Coordinator.h*" se encuentran los defines `X_AXIS_MAX_VALUE` y `PHOTODIODE_MAX_VALUE`.
> Tener en cuenta que el valor de oscuridad máximo del fotodiodo es 4095 (Apuntar con linterna para ver como disminuye hasta 0)
> El MPU6050 mide el eje x como la componente horizontal, es decir, si el telescopio se inclina hacia delante o hacia atrás, el
> valor de este eje será modificado, si se mantiene en la horizontal su valor será próximo a 0.

- **Python**: En el archivo "*Defines.py*" se encuentra los defines `ANGLE_X_MAX_VALUE`y `PHOTODIODE_MAX_VALUE`.


## Claves WiFi y MQTT

- **ESP32**: En las siguientes líneas de código del archivo "*Coordinator.h*" es donde deben introducirse los valores correspondientes:

> Cuidado no modificar los tipos

```sh
	/* WiFi */
	const char* ssid = "****************";
	const char* password =  "***********";
	/* MQTT */
	const char* mqttServer = "**********";
	const int   mqttPort = ******; 
	const char* mqttUser = "************";
	const char* mqttPassword = "********";
```

- **Python**: Existe un fichero llamado *config.txt* en el cual está diseñado para ser leido en el orden: *mqttServer*;*mqttPort*;*mqttUser*;*mqttPassword*
> Cuidado con los **;** ya que señalan las separaciones entre los valores



[tuto]: <https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/windows.md>