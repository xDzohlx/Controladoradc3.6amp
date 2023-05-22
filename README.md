# Controladoradc3.6amp
Controladora electronica de velocidad de 3.6 amperios hasta 25 volts V1.1
Controladora electronica de velocidad de 3.6 amperios hasta 25 volts V1.1 Esta es la versión 1.1 de la tarjeta controladora fue diseñada por Carlos Vega, la selección de componentes del driver y el microcontrolador por Aldo Bucio. Está basada en otra paca diseñada por Aldo bucio hace tiempo. Perdí el primer archivo de la placa pero estan los archivos gerber pero estan los de la version 1.1.

Especificaciones: 4 celdas de litio polímero, hasta 25 volts absolutos(por los capacitores de puede aumentar la tensión hasta 32 volts absolutos limitados por el regulador) 3.6 amperios por canal, protegida para más corriente se apaga y reinicia cuando se alcanza esta corriente. El driver de los motores es muy confiable y a prueba de balas lo que se necesita para el combate de robots Canales completamente mezclados para conducir un robot en modo diferencial. Se leen dos canales de pwm en cualquier protocolo de envío de 50 a 100 hz. También esta la version de firmware con lectura de ppm en un solo cable con decodificador de un canal para la activación de un servo u otro actuador. Los esquematicos y la placa estan diseñadas en Eagle.Fue programada en atmel studio 7 con el microcontrolador Attiny828.

Manual de operacion Caracteristicas

Gracias por usar la tarjeta controladora electrónica de velocidad. Características: Canal doble 3.6 A por canal Protección contra sobre corriente y sobre temperatura 2-6s de litio polímero 25 volts absolutos 7.2 Amp Canal doble Interfaz RC PWM, PPM Decodificador en modo PPM sin retraso. Canales mezclados o bidireccionales. Sin retraso de señal de radio Auto calibración al encendido BEC conmutada de alta eficiencia 88% 1 A hasta 2A continuos. (BEC) Respuesta lineal Adelante y atrás a máxima potencia. PWM de 32 Khz Freno del motor Tiempo de servicio muy largo puede estar encendida por más de 1 hr. Conector JST para batería. Conectores de servo para señales.

El modo de operación viene preprogramdo, como lectura independiente de PWM, PPM, mezclados, bidireccionales o canal sencillo de mayor corriente estos modos son el firmware de la controladora. Modo PWM. El canal 1 es el volate y el canal 2 es el acelerador, el primero controla los giros mientras que el segundo controla el acelerador y la reversa. Modo PPM. En el canal 1 se conecta la señal PPM, el canal 1 es el volante y el canal 2 es el acelerador. El cable del canal dos es un decodificador del canal 3. En cualquiera de los modos cuenta con auto calibración al encendido por lo que se tiene que dar un tiempo a esto de 5 segundos antes de mover cualquier valor del radiocontrol. El led muestra el estado de la controladora.
Un video probando la tarjeta: https://t.co/X4io4Tz9yK?amp=1

Otros videos arreglando issues:
https://youtube.com/live/U84N-yGLdbo?feature=share

https://youtube.com/live/5lfUKTcOBrQ?feature=share
