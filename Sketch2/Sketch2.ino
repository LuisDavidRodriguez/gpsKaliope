/*
 Name:		Sketch2.ino
 Created:	22/12/2019 18:10:23
 Author:	david
 //buscar mi archivo --Comunicaci�n i2c y serial Luisda osciloscopio.docx en mis documentos por si quieres saber mas del i2c y del serial y como pude entender y dise�e algunas funciones asi
 ahi expoiingo lo que he estado aprendiendo de las comunicaciones seriales y i2c para poder hacer este proyecto, lo vi en osciloscopio y todo

*/






//#define VERSION_SMD
//#define DEBUG




                  // https://www.visualmicro.com/post/2017/01/16/Arduino-Cross-Platform-Library-Development.aspx //problema con la libreria Time, nos aparecia Error en Linker referencia vacia hacia setTime, now etc las funciones que usaba de aqui, asi fue la solucion
                                    //aparte de ese error el compilador indicaba que no se encontraba el elemento tmElements_t cuando se llamaba la libreria SD1307RTC, ese problema lo causo antes y ahora que formatie mi lapton, y resulta que cuando inicias la extencion visualMicro, nos pide en que carpeta se instalo el IDE de arduino, que se tiene que descargar
                                    //entocnes resulta que por mas que yo llame aqui a Time.h y hacer clicl y agregar como liberia compratida en  el boton AddLibrary, todo indica que estamos llamando a la libreria nueva de Time, proque es asi
                                    //hay una libreria time.h viejita con t minuscula que no tiene tmelements pero la nueva Time con T mayuscula si la tiene, el problema es que aunque al presionar ctrl y clic en los renglones donde declaramos include <Time.h> visual studio nos manda a la libreria nueva la correcta
                                    //entocnes se cre que todo esta bien y es dificil encontrar el error, pero sigue ahi porque resulta que el ide de arduino en la ruta donde se instalo en mi caso C:\Program Files\Arduino\hardware\tools\avr\avr\include
                                    //ocurre que ahi tiene una libreria time.h entonces por algun motivo cuando se compila visual studio compila la libreria time.h minuscula en lugar de la Time.h que tengo en C:\Program Files\Arduino\libraries
                                    //y pues nunca encuentra la estructura tmelements, lo unico que hice fue entrar a la ruta donde esta time.h y cambiarle el nombre a timeAntigua.h y agregar las librerias correctas otra vez al proyecto
                                    //y pum se corrige
                                    //algo paso en el link de arriba y antes aparecia en el exprorador de soluciones la libreria Time y DS1307 con un simbolo y aparecia en Referencias aparecian las 2, ahora no lo hace y funciona no se porque lo agrege como decia el link veremos si no falla

#include <Time.h>
#include <DS1307RTC.h>
#include <EEPROM.h>
#include <Arduino.h>
#include <avr/wdt.h>                //incluimos la libreria del watch dog, recordar que a los arduinos nano debemos cambiar su bootloader por el der arduino Uno
#include <Wire.h>                   //la usaremos para manejar el i2C para comunicar los arduinos, hay que tener cuidado con la direccion de la pantalla ya que ella usa i2c tambien pero con la libreria liquidCristal si conviertes la direcicon de la pantalla 0x27 a decimal da 39, entonces mientras coloque las direcciones de los esclavos que no sea 39 estare bien
//#include <..\Adafruit_SSD1306\Adafruit_SSD1306.h> //librerias para el manejo de la oled 
//#include <..\Adafruit_GFX\Adafruit_GFX.h>           //para el manejo de la oled
#include <TimerFive.h>

#include "Pines.h"                  //los pines que definimos para este proyecto :D
#include "RetardosConMillis.h"      //clase para manejar los retardos, pero sin usar delays, se usan millis y condicionales
#include "Pantalla.h"
#include "Flujometro.h"
#include "ClaseManejoSim808.h"
#include <SD.h>
#include "LuisdaManejoEEPROM.h"
#include "LuisdaManejoSD.h"
#include "Utilerias.h"
#include "Acelerometro.h"



//La direccion del modulo RTC es fija y es 0x68 que equivale al 104
#define MI_DIRECCION_I2C 0X1                            //EN DECIMAL ES 1
#define DIRECCION_ESCLAVO_ACELEROMETRO_I2C 0X2                //EN DECIMAL ES 2 JAJA LO PONGO EN EXA PA QUE SE VEA PRO XD

//Cuidado a mayor velocidad por ejemplo 57600 cuando son datos de informacion largos es propenso el modulo sim a mas errores en 38400 esta bien puse 57600 y ocurrian mas errores imprevistos
/*
aqui esta el link del procentaje de errores marca que en 57600 el error es de 2% pero claro eso es usando el puerto serie independeinte que tiene el procesador
no uno emulado con la libreria Software serial, 
porque con datos cortos no hay problema la informacion se recibe bien y por eso cuando se envian al modulo sim 808 los comandos no hay problema porque son de 30 bytes maximo
pero cuando el mopdulo sim responde respuestas largas de 100bytes a esta velocidad en los ultimos bytes comienza ha haber erorres y es porque la velocidad de los relojes se desfaza
ahora al hacer una peticion a una pagina responde todo el encabezado http, este encabezado al final llega horrendamente deformado y se imprime asi en el puerto serie
al bajar la velocidad se puede ver que lo que retorna la web al modulo sim y se imprime en el monitor serial ya llega perfectamente la informacion a 38400 aun hay incordancia de datos
pero a 19200 ya es muy poca la discordancia


Quizas tengo entendido que algunos arduinos tienen 2 puertos serie reales para no emularlo con el software serial quizas ahi si soporte la velocidad mas rapida
Arduino 1 tiene 1 solo puerto UART
Arduino mega y DUE tienen 4 puertos UART reales!!!

si no nos quedara por seguridad quedarnos en 19200 para recibir los datos lo mas claros posibles, 
pero claro sacrificando velocidad en cada envio de comandos al sim pufff

Al inicio no tenia muhco este problema o mas bien no lo notaba, comence a notarlo cuando el modulo nos enviaba los 100 bytes de las coordenadas los grados de direccion que llegan al final de los 100bytes
llegaban deformes baje la velocidad y funciono
despues como solo estavamos enviando informacion al servidor no habia problema porque arduino escribia los datos al sim8080

pero notaba que cuando se devolvia el encabezado del la conexion al servidor llegaba deforme los ultimos datos, 

ahora como necesito conectarme al servidor cada cierto tiempo y el me retornara en el body del http la informacion de las configuraciones esto se vuelve un problema
porque el body llega despues de los encabezados http los encabezados son como 158 caracteres o bytes

SEND OK
HTTP/1.1 200 OK
Date: Thu, 20 Feb 2020 20:15:06 GMT
Server: Apache
Connection: close
Transfer-Encoding: chunked
Content-Type: text/html; charset=UTF-8
                                                --apartir de aqui es el body, nos dice cuantos datos son y despues el body en este caso nuestra memoria solo esta a 500 por lo tanto entraron al array los primeros 500 pero contando el encabezado solo entraron como 200 bytes del body
1711
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta http-equiv="X-UA-Compatible" content="ie=edge">
    <title>Aviso de Privacidad</title>
</head>
<body>
    <h1>Aviso de privacidad</h1>
    <h4>El presente aviso d
------------------------------------------------------esta es una respuesta que el servidor entrega a una peticion que hiso arduino atravez del modulo, nuestro array donde guardamos la info la pusimos a 500 bytes
SEND OK
HTTP/1.1 200 OK
Date: Thu, 20 Feb 2020 18:42:17 GMT
Server: Apache
Connection: close
Transfer-Encoding: chunked
Content-Type: text/html; charset=UTF-8

19
Hola sim80 cmoeta\n
0
_________________________________________________ esta es una peticion hecha a 38400 vemos que en los headers casi no hay errores pero en el body apesar que solo nos esta devolviendo 19 caracteres ya precenta un error deberia ser-> Hola sim 808 como estas\n
SEND OK
HTTP/1.1 200 OK
Date: Thu, 20 Feb 2020 20:36:30 GMT
Server: Apache
Connection: close
Transfer-Encoding: chunked
otn-p:tx/tl hreUF8

71
!OTPhm>
hm a=en<ed  <e hae=UF8>
 <eanm=vepr"otn=wdhdvc-ih nta-cl10"
  mt tpei=XU-optbe net"eeg"  tteAiod rvcd<tte<ha<by  <1Ai epiaia<h>
 <4E rsneaioepiaia eetbe ncmlmetd ldsusoe ae eea ePoec� eDtsProae  oei� elsPrilrs�AIE EC� o oiloe � NRVSADCAO#3,TNNI RLS OEI,MCOC� �xc,erepsbed eaassdtsproae,d s u
el � lsmso  es rtc�n uifrci�proa e� ii
___________________________________________________________________________________y a 57600 puff da pena xD deveria ser lo del aviso de prvacidad lo que se hiso en la prueba de 19200




    vemos que a 19200 la informacion hasta el body llega limpia y eso a esa velocidad aun nos arriesgamos un poco a errores pero ya 9600 es muy lento 

    entonces si a 38400 aun tenemos errores en los encabezados en la parte del body ni se diga, llega mas o menos bien pero caracteres erroneos

    eso sera un problema porque en el body vendran las configuraciones que el usuario haya seteado para este gps en el servidor
    entonces pufff lo mejor es bajar la velocidad si queremos que nuestras configuraciones en el body regresen bien

    el modulo sim del servidor recibe perfectamente la info se la envia a arduino a su velocidad programada pero el arduino por el software serial al paso del tiempo su reloj se desfaza

https://arduino.stackovernet.com/es/q/2229

https://arduino.stackovernet.com/es/q/10981             ---> 
A medida que aumentan los bps de datos en serie, la precisión de reloj requerida se vuelve más estricta.Cuando un par de errores porcentuales pueden estar bien a 9600 bps, es probable que se necesite un porcentaje inferior a 115200 bps.

En varias tarjetas, he medido errores de frecuencia del oscilador RC de aproximadamente el 2%, lo que según la pregunta How critical are UART frequencies? puede o no ser lo suficientemente bueno como para permitir comunicaciones en serie de alta velocidad.
<---------


Intentare hacer la prueba ahora en lugar de emular los puertos con la libreria SOFTWARE serial lo hare debido a que estoy trabajando con un arduino mega
usare otro puierto UART real pare ver si es capaz de manejar una velociad mas alta, creo que quizas si sea ese probmela

En Arduino UNO y Mini Pro los pines empleados son 0 (RX) y 1 (TX). 
En el caso de Arduino Mega y Arduino Due, que tienen cuatro puertos de serie,  lo conectare al puerto serie 3 por el motivo que es mas facir distingir en el codigo Serial3.print contra Serial.print y contra Serial1.print
asi evito errores al confundir Serial.print para enviar info al monitor serie de la computadora y Serial3.print al enviarlo al modulo sim, me explico?
el puerto serie 0 está conectado a los pines 0 (RX) y 1 (TX),
el puerto serie 1 a los pines 19 (RX) y 18 (TX) 
el puerto serie 2 a los pines 17 (RX) y 16 (TX),
y el puerto serie 3 a los pines 15 (RX) y 14 (TX).



Y asi es muchachos me acabo de sacar un premiesote!!! jajaja papaya de celaya
El problema es el Software serial. Modificamos nuestra clase del manejo de sim!
Ya no heredamos de software serial ya no podemos eredar
sino que simplemente los envias al puerto serial con SERIAL1.write

o si eliges el 3 con SERIAL#.write es todo era tan facil

lo unico malo es que ya no podra funcionar nuestra clase con un arduino nano pero eso por ahora no importa ya para tanta informacion que se maneja el nano o 1 quedan cortos en memoria
bueno ya sabemos como hacerlo detodas maneras guardo las partes de la libreria de modulo sim que usaba el software serial

rdtamos transmitiendo a 57600 bites y la informacion se recibe perfectamente podre un ejemplo de los tiempos que acabamos de reducir

para enviar los 1711 caracteres de la pagina aviso de privacidad el sim 808 envia 3.16 segundos datos a 19200 baudios

para el comando AT tardamos a 19200 8ms 
para el estatus del gps 40ms
imei 24ms
senal 25ms
gestor carga 28ms
bateria 23ms
estado gprs 23ms
estado de contexto con cipstatus 24ms
en total arduino en cosultar el estado del modulo tomando en cuenta le tiempo que tarda arduino en hacer sus operaciones tardo 1.1Segundo a 19200

Ahora la probaremos a 115200 woajajajaja son 6 veces mas rapido 

para enviar los 1711 caracteres de la pagina aviso de privacidad el sim 808 envia 3.16 segundos datos a 19200 baudios

para el comando AT
para el estatus del gps 7ms
imei 6ms
senal 4ms
gestor carga 4.6ms
bateria 4ms
estado gprs 4.7ms
estado de contexto con cipstatus 4.24ms
en total arduino en cosultar el estado del modulo tomando en cuenta le tiempo que tarda arduino en hacer sus operaciones tardo 184ms a 115200 omaiga!!!!! a 57600 la velocidad era de 421ms
y que tal la recepcion de la pagina de internet pues sale perfecto!!! como si fueran 19200 baudios!!

Perfecto perfecto perfecto!!! podremos trabajar con 115200 baudios sera una velocidad tremenda añadida a todo el proyecto, podriamos simplificar al no tener que usar el metodo
calcularTiempoDeEsperaProcesador, porque basicamente todas las respuestas entraran en menos de 10ms no importa lo largo que esta sea, por ejemplo la de las coordenadas que es de 100 bytes

lo que si noto es que en el osciloscopio cuando el modulo sim envia informacion a veces la manda un pedacito, hay una pequeñisima espera y despues el otro pedacito, no se poeuqe sera pero aun
asi es endemonadamente rapido!!!
Ya encontre el motivo de los pedaciotos al parecer algo a cambuiado en el mobulo cunaod se cambio la velociad ocurre que antes nos respindia esto 
AT+CIPSTATUS
OK
ahora nos devuelve 1 ves el comando que le escribimos y la otra nos devuelve lo que siempre, ahora sale un renglon de mas
AT+CIPSTATUS
AT+CIPSTATUS
OK

o por ejemplo asi
AT+CGPSINF=2                                                    --Aui nos devuelve el comando que le mandamos es el primer pedacito que se ve
AT+CGPSINF=2                                                    --apartir de aqui nos devuelve los datos pero primero su compando 
+CGPSINF: 2,202939.000,0019.7953,N,-0099.8789,W,1,7,1.38,2575.2,M,-9.0,M,,

o quizas antes no l vehiamos con el software serial a menso que haya cambiado una configuracion sin querer puff esto nos afecta solo si los metodos no los buscamos con strstr sino que les quitamos renglon a renglon nos afecta porque ahora hay un renglon de mas
si bajas la velocidad aun asi devuelve la confirmacion pero como es mas lento el tren de pulsos se ve continuo y a 115200 se ve la separacion pero como sea estan llegando un renglon de mas ahora que es la confirmacion del comando enviado


ahora cunaod consultamos las coordenadas cada 2 segundos no me siento mal porque ahora solo tarda 79ms en hacer las 4 consultas al modulo y entregarnos las coordenadas, con 57600 tardabamos 159ms por cada consulta de ubicacion
para recibir una peticion de cordenadas comop 
AT+CGPSINF=0
AT+CGPSINF=0
+CGPSINF: 0,1947.717000,9952.737000,2556.000000,20200220220604.000,0,12,0.388920,119.940002

OK
son ams de 100 bytes y solo tardamos 12ms en enviar la peticion y recibir toda la info del modulo
y anters tardabamos por cada consulta de 100bytes a 57600 y teniendo errores con SoftwareSerial al final en cadenas largas tardabamos como 152msms ahora todo se nota mucho mas fluido
ya que el procesador no lo dejamos esperando dentro del bucle que lee el buffer serial durante tiempos muy largos
y ni se diga cuanod lo tenia a 38400 y a 19200 como lo tendria que haber hecho por el maldito software serial para recibir mas estabilidad acambio.

*/
#define VELOCIDAD_PUERTOS_SERIE 115200 //esta sera la velocidad usada para configurar los puertos serie, es importante mantenerla porque le metodo que lee los datos la necesita para saber cuanto tiempo esperar una respuesta




#define PHP_GPS_ARCHIVO_LOCALIZACION "gps_recibe_datos.php"
#define PHP_ACELEROMETRO_LECTURAS_REALES "calibracion_recibe_datos.php"
#define PHP_ACELEROMETRO_ENVIAR_CONTADORES "acelerometro_recibe_datos.php"
#define PHP_EJEMPLO "gps_luisda.php"
#define PHP_FLUJOMETRO "flujometro_recibe_datos.php"

/*
1.2
-Corregimos el error del modulo sim, cuando el vehiculo 36 se quedo sin enviar datos todo el dia viernes el problema fue que cuando le enviabamos 
        el comando at para encender el GPRS no lo activaba, y en cada bucle el procesador respondia GPRS apagado activando GPRS, y nunca se pudo 
        activar, para resolver eso reinicie el modulo y ya pudo entrar. entonces añadi un contador donde con cada vez que se detecta gprs apagado
        este aumenta, cuando el gprs se activa se reinicia a 0, entonces solo aumentara cuando cada moemnto reponda gprs apagado, llegado a 4 
        en la parte del gestor de errores ahi se reiniciara el modulo.
-Corrigo los pulsos falsos devido a las vibraciones en la turbina del flujometro, y esto hace que se registren cargas de gasLP en los lugares donde
        hay muchas vibraciones, para eso aumente el minimo de pulsos requeridos para reconocer carga de 10 a 40, y ademas un contador que cada 2 
        segundos reinicia los pulsos que se hayan reconocido y no llegaron a 40 a 0, es decir tenemos 2 segundos para que los pulsos aumenten de 0 a 40
        para que ene ese momoento se reconosca carga de glp, y sabemos que esto es facil porque el flujo de una bomba es de 64Pulsos cada 500ms en dos
        segundos tendriamos 256 pulsos, o de igual cuando se conecta la mangera en lo que se nivela la presion la turbina girara

1.21
 -Corregimos la inicializacion de las salidas y entradas de switch glp y bloqueo y iniciamos el bloqueo en high para que el vehiculo no se apague cuando el arduino se reinicia

 1.22 
 -Actualizamos el filtro de cargas de GLP fantasmas para intentar resolver el problema que tiene el 29 cuando se queda 30 minutos registrando cargas
        aumentamos los pulsos minimos de 40 a 80 y recortamos el tiempo de reinicio de 2s a 1 segundo

1.23
-Actualizamos el metodo calcular flujometro, en los if que reinician el contador y retornan true o false
para salir de los bucles de carga de glp, esto para resolver el problema del 29 y otros vehiculos que se quedan en el bucle
de carga por 30 minutos en ocaciones, esto ocurria porque una ves dentro del bulce por cualquier manera posible
se seguian capturando 1 pulso cada 2 o 3 segundos y eso bastaba para nunca salir del bucle de carga ahora pedimos un minimo de 45 pulsos cada segundo
para reiniciar el TIMER y mantenernos dentro del bucle si no hay 45 pulsos minimos cada segundo entonces el TIMER no se reinicica
y saldremos a los 15 segundos del bucle si o si
VALORES DE LOS FILTROS. FILTRO CARGA DETECTADA MINIMO 80 PULSOS EN UN SEGUNDO. FILTRO MANTENER BUCLE VIVO DE CARGA 45 PULSOS CADA SEGUNDO


1.24
-Actualizamos el metodo cerrar sim808->gprsCerrarEnvioDeInformacionPOST() para resolver el problema de los datos duplicados
    ese error solamente ocurria con los datos guardados en la memoria SD, o bueno era donde era mas notorio ya que se enviaban 
    5 10 o hasta 52 veces repetidos los mismos datos, en varias ocaciones envio 1000 bytes de una sola ocacion pero no se recibia la respuesta
    200 ok entonces el sistema no actualizaba la posicion final aunque el modulosim habia respondido SEND OK. Entonces el sistema volvia a intentar otra vez el envio
    y ocurria lo mismo SEND OK pero no se recibia 200 ok del server, esto se repitio hasta 52 veces y al ver la info en el servidor, habia 52 veces la misma informacion
    es decir por enviar 1000kb realmente se estaban enviando 52000 lo mismo, y asi varias ocaciones, haciendo que se tardara mucho mas en enviar la informacion de la SD
    y consumiendo dinero en saldo telcel. guardo los archivos de la investigacion que hice en el escritorio en la carpeta memoria del 29
    probablemente podriamos comenzar a ver perdida de datos, auqnue no creo porque todos los datos que segun no habia 200ok estan en la base de datos entonces seria muy raro
    y si no quizas el problema es el tiempo limite de 5 segundos que no le alcanza a responder el server 200ok porque cuando si recibimos toda la respuesta 
    el sistema me indica el procesador espero en el bucle durante 4164 de un tiempo limite de 5000 se activo la busqueda de respuesta:true
    y cuando no hay 200ok pero si SEND OK indica aesto:
    El procesador espero en el bucle durante: 5001 De un tiempo limite de: 5000 Se activo busqueda de respuesta:false
    es decir estamos saliendo por el limite de los 5 segundos y ya no esperamos mas tiempo quizas esta tardando un poco mas de lo debido.
    tendriamos que aumentar el tiempo limite en el comando que se envia en bool ClaseManejoSim808::gprsCerrarEnvioDeInformacionPOST()
    en esta linea: info = enviarComandoSinEvaluar("", 5000, "Date:");
    pero tendriamos el limite del watch dog de maximo 8 segundos y eso tendriamos que reiniciar el watch dog justo antes de netrar a ese comando,
    quizas sea un problema esperar mas de 8 segundos prefiero optar por la opcion que ya realice de retornar true en lugar de false en el metodo
    VALORES DE LOS FILTROS. FILTRO CARGA DETECTADA MINIMO 80 PULSOS EN UN SEGUNDO. FILTRO MANTENER BUCLE VIVO DE CARGA 45 PULSOS CADA SEGUNDO
*/


#define VERSION_SOFT 1.24




ClaseManejoSim808 sim808(PIN_ON_OFF_SIM_808, PIN_RESET_SIM_808);//enetonces el pin de recepcion Rx sera el 7 conectado a transmision de la sim y el de transmision Tx el 8 conectado a Recepcion de la sim
//usa 238 bytes87

//Pantalla pantalla1 = Pantalla(0x27); //usa 87 bytes

File myFile;

LuisdaManejoEEPROM luisdaEE;

Acelerometro acelerometro(DIRECCION_ESCLAVO_ACELEROMETRO_I2C);




//creamos nuestro objeto retardos para manejar como si fueran delays pero sin detener el programa
RetardosConMillis retardoEnviarDatosEnSD = RetardosConMillis(10000); //por cada instancia creada de retardos se apartan 12bytes
RetardosConMillis retardoWire = RetardosConMillis(3000);
RetardosConMillis retardoEstadoDelModulo = RetardosConMillis(10000);
RetardosConMillis retardoSolicitudDeCoordenadas = RetardosConMillis(); //no definimos el tiempo porque podremos manejar este tiempo en tiempo de ejecucion en este caso se guardara en la eeprom el valor
RetardosConMillis retardo1Segundo = RetardosConMillis(1000);
RetardosConMillis retardoAdministrarRelojRTC = RetardosConMillis(80000);
RetardosConMillis retardoactualizarCoordenadasPrincipales = RetardosConMillis(2000);
RetardosConMillis retardoPingAlServidor = RetardosConMillis(60000);                                         //Ponemos estos valores por default puesto que se configuraran en tiempo de ejecucion con los datos del usuario
RetardosConMillis retardoEnviarLecturasActualesAcelerometro = RetardosConMillis(60000);                                         //Ponemos estos valores por default puesto que se configuraran en tiempo de ejecucion con los datos del usuario




//creamos nuestro objeto flujometro
Flujometro flujometro1(PIN_DIGITAL_FLUJOMETRO);       //solo usa 18 bytes

bool controlEnvioDeInstruccion1 = false;



bool estadoDelModuloSim = false;                    //iniciamos esta variable como si el modulo estuviera apagado
byte estatusDelGps = GPS_APAGADO;
byte fuerzaDeLaSenal = 0;
bool sePuedenHacerConexionesGPRS = false;
bool memoriaSDInicializadaCorrectamente = false;
String bodyConfiguracionesAnterior = "";            //aqui guardaremos el string del ultimo body de configuraciones recibido para si las nuevas configuraciones son iguales no entrar al proceso de asignacion en eeprom y ahorrar tiempo

byte contadorGestorDeErroresModuloSim = 0;
byte contadorNumeroDeVecesGPRSApagado = 0;          //este contador solo lo usaremos para resolver el error que tiene el modulo sim cuando por mas que se envia el comando para activar el gprs este no se activa


bool accVehiculo;           //invertir la lectura del digital read, si esta el vehiculo encendido el read dara 0, este debera eviar 1 al servidor, cuando se apage leera 1 y debera enviar 0 al servidor
bool glpVehiculo;           //cuando el sistema de glp esta apagado el digital read leera 1 y tendremos que enviar 1 al server indicando que va a gasolina, cuando el glp se activa el read leera 0 y enviara 0 al server







void setup() {
    wdt_disable();
    

   retardoEnviarDatosEnSD.inicializarMillis();
   retardoWire.inicializarMillis();
   retardoEstadoDelModulo.inicializarMillis();
   retardoSolicitudDeCoordenadas.inicializarMillis();
   retardo1Segundo.inicializarMillis();
   retardoAdministrarRelojRTC.inicializarMillis();
   retardoactualizarCoordenadasPrincipales.inicializarMillis();
   retardoPingAlServidor.inicializarMillis();
   retardoEnviarLecturasActualesAcelerometro.inicializarMillis();

   pinMode(PIN_SWICH_VEHICULO, INPUT_PULLUP);
   pinMode(PIN_GLP_VEHICULO, INPUT_PULLUP);
   pinMode(PIN_BLOQUEO_VEHICULO, OUTPUT);
   digitalWrite(PIN_BLOQUEO_VEHICULO, HIGH);

   wdt_enable(WDTO_8S);                                    //lo activamos con un tiempo de 4s, si en alguna parte del programa no se llega al comando wdt_reset(), y este tiempo se agota significa que el arduino se quedo colgado en algun punto y entonces el watch dog lo resetea en automatico
  

   

                                             //desactivamos el wathc dog en lo que se hacen las configuraciones
   
    sim808.begin(VELOCIDAD_PUERTOS_SERIE);                                    //iniciamos la comunicacion del sim 808 a 19200 SI TU VES CON UN OSCILOSCOPIO CADA PULSO O BIT LE TOMARA 50us si doblas la velocidad a 38400 sera de 25us en cambio si la bajas a 9600 tomara 100us, el modulo es compatible con 19200 con 38400 ya no funciona y con menos creo no muy bien
    Serial.begin(VELOCIDAD_PUERTOS_SERIE);                                    //Se tiene que inicializar el puerto serie del sim y del la computador a la misma velocidad
    Serial.println(F("WhatchDogActivado"));
    Serial.print(F("Iniciando Setup, version soft: ")); Serial.println(String(VERSION_SOFT));




    

   flujometro1.begin(1000,15000);
   //pantalla1.inicializar();                   //Esta linea da problemas no descomentar hace que arduino no inicie y en el osiloscopio se queda en un bucle infinito pulsando su salida SDA y SCL
 
    

    
    

    Serial.println(F("Inicializando Memoria Sd"));   
    
    if (!SD.begin(PIN_CS_SD)) {
        Serial.println(F("No se pudo inicializar SD"));
        //return;
        memoriaSDInicializadaCorrectamente = false;
    }
    else {
        Serial.println(F("inicializacion SD exitosa"));
        memoriaSDInicializadaCorrectamente = true;
    }
    
    


    
    //luisdaEE.setAPN_nombreRed("internet.itelcel.com");
    //luisdaEE.setAPN_user("webgprs");
    //luisdaEE.setAPN_password("webgprs2003");
    
    
    luisdaEE.getConfiguraciones();
    retardoEnviarDatosEnSD.setRetardo(luisdaEE.configuraciones.tiempoEnviarDatosSD);
    retardoEnviarLecturasActualesAcelerometro.setRetardo(luisdaEE.configuraciones.tiempoEnvioLecturasAcelerometro);
    retardoSolicitudDeCoordenadas.setRetardo(luisdaEE.configuraciones.tiempoEnvioCoordenadas);
    retardoPingAlServidor.setRetardo(luisdaEE.configuraciones.tiempoPing);



    

    
    
       

    

   
   Wire.begin(MI_DIRECCION_I2C);                                         //inicializamos el puerto i2c o lo unimos como sea xD como este sera maestro no ponemos direccion, si quremos por ejemplo multimaestros tenemos que poner la direccion de este arduino aqui, para que primero se comunique con el esclavo y luego el esclavo se haga maestro y se comunique con el
   Wire.onReceive(onReceiveEvent);
   Serial.println(F("Wire Iniciado"));



   //RTC.set(1521378450);                             //Ponemos una hora random en el modulo solo lo dejo para debug para ver si el modulo funciona 18/3/2018,8:7:36
   setTime(RTC.get());//usa wire
   

            


            


            
            //La primera vez que cargo el Sketch a un arduino nuevo hay que programar su eeprom, despues lo borramos
            //luisdaEE.setConfiguracionesDesdeString("STATUSKALIOPEOK,60,20,0,20,10,0,-6,0");

           
            //Timer5.initialize(5000000);
            //Timer5.attachInterrupt(solicitarContadoresAcelerometro);


            Serial.println(F("SetupFinalizado"));
    }


void loop() {
    wdt_reset();



    


    if (retardo1Segundo.seHaCumplido(true)) {
        //EEPROM.get(4, gato);
        //EEPROM.get(0, valor);
        //Serial.println(luisdaEE.getUltimaLecturaMemoriaSD());

        if (!memoriaSDInicializadaCorrectamente) {
            
            if (SD.begin()) {
                memoriaSDInicializadaCorrectamente = true;
            }
            else {
                Serial.println(F("No se ha podido inicializar la memoria SD"));
                memoriaSDInicializadaCorrectamente = false;
            }
            
        }

        
        

        Serial.println(Utilerias::horaEnZonaHoraria(now(), luisdaEE.configuraciones.zonaHoraria));  

        


        /*
        Leemos las entradas digitales del vehiculo
        */

        accVehiculo = !digitalRead(PIN_SWICH_VEHICULO);         //invertimos la lectura
        glpVehiculo = digitalRead(PIN_GLP_VEHICULO);            //dejamos la lectura igual
        if (luisdaEE.configuraciones.bloqueoDeMotor) {
            digitalWrite(PIN_BLOQUEO_VEHICULO, LOW);
        }
        else {
            digitalWrite(PIN_BLOQUEO_VEHICULO, HIGH);
        }

        Serial.print(F("Lectura Pin numero ")); Serial.print(String(PIN_SWICH_VEHICULO)); Serial.print(F(" Switch si es 1 vehiculo encendido, 0 apagado: ")); Serial.println(String(accVehiculo));
        Serial.print(F("Lectura Pin numero ")); Serial.print(String(PIN_GLP_VEHICULO)); Serial.print(F(" combustible si es 1 vehiculo gasolina, 0 glp: ")); Serial.println(String(glpVehiculo));
        Serial.print(F("Lectura Pin numero ")); Serial.print(String(PIN_BLOQUEO_VEHICULO)); Serial.print(F(" Bloqueo motor si es 1 vehiculo activado, 0 vehiculo desactivado: ")); Serial.println(String(digitalRead(PIN_BLOQUEO_VEHICULO)));
        


    }
    


    
  



    




    
    if (retardoEstadoDelModulo.seHaCumplido(true)) {

        Serial.println(F("\n\n\n\n--------Configuraciones Del Modulo--------\n\n\n\n"));
        Serial.print(F("EnvioCoo ")); Serial.println(String(luisdaEE.configuraciones.tiempoEnvioCoordenadas));
        Serial.print(F("LecAcel "));  Serial.println(String(luisdaEE.configuraciones.tiempoEnvioLecturasAcelerometro));
        Serial.print(F("CalibAcel ")); Serial.println(String(luisdaEE.configuraciones.calibrarAcelerometro));
        Serial.print(F("Ping ")); Serial.println(String(luisdaEE.configuraciones.tiempoPing));
        Serial.print(F("SD ")); Serial.println(String(luisdaEE.configuraciones.tiempoEnviarDatosSD));
        Serial.print(F("BloqMoto ")); Serial.println(String(luisdaEE.configuraciones.bloqueoDeMotor));
        Serial.print(F("ZonaHor ")); Serial.println(String(luisdaEE.configuraciones.zonaHoraria));
        Serial.print(F("EnviarLecturasAcel ")); Serial.println(String(luisdaEE.configuraciones.enviarLecturasAcelerometro));













        
        //Preguntamos si el modulo sim esta apagado o sin conexion, si esta apagado, lo activamos
        estadoDelModuloSim = sim808.getEstatusModuloEstaApagadoEncendido();

        if (!estadoDelModuloSim) {
            Serial.println(F("El modulo esta apagado o sin conexion fisica al arduino, o el modulo no tiene nada de señal gsm y no esta respondiendo, activando Modulo..."));
            //en algunas ocaciones cuando enviamos la informacion de un mensaje o el cuerpo de info gprs el modulo sim responde con su inductor >
                //en ese momento todo comando que le envies el modulo lo toma como parte del cuerpo del mensaje, entonces en ocaciones
                //por alguna razon mis metodos que envian la info del mensaje y que envian el caracter de salida char(26) para que el modulo comience a recibir normalmente los comandos
                //no llega o no lo detecta y el gps se queda recibiendo la informacion, entonces el modulo esta encendido y este metodo me dice que el gps esta apagado
                //porque no responde correctamente con AT ok sino que el modulo le responde el mismo comando que le envia porque se esta ingresando como parte de la informacion a enviar
                //para resolverlo voy a enviar aqui el comando de finalizacion de mensaje y ya sea que el modulo se quede en ese estado entraremos aqui y lo sacara de ahi o lo encendera
                //como sea para apagar el modulo se requiere un pulso mas largo que 1 segundo creo es 1.5 o 2, por lo tanto el comando de activar gps no hara nada

            sim808.enviarComandoSinEvaluar(AT_SMS_FINALIZAR_ENVIO_SMS_2, _MENOR_A_128_BYTES,nullptr);
            sim808.enciendeModuloConDelays(); //aqui detenemos el prosesador 1.4segundos
            sim808.resetDelModuloDelays();

            estatusDelGps = GPS_APAGADO;
            sePuedenHacerConexionesGPRS = false;
        }
        else {


            Serial.println(F("Modulo sim808 encendido\n\n"));



            Serial.println(F("Comprobando estado del pin de la tarjeta sim"));
            sim808.getStatusPinSim();



            //si esta encendido comprobamos en que estado esta el modulo gps           
           Serial.println(F("Comprobando estado de GPS"));
           estatusDelGps = sim808.getStatusGps();
           switch (estatusDelGps)
            {
            case GPS_APAGADO:
                Serial.println(F("Gps apagado, activando gps..."));                
                sim808.activarGps();
                break;
            case GPS_ENCENDIDO_SIN_SENAL:
                Serial.println(F("Gps activado, sin senal gps, adquiriendo..."));
                break;
            case GPS_ENCENDIDO_SENAL_MEDIA:
                Serial.println(F(" Gps activado,Calidad de senal gps: media, intentando mejorar la seal"));
                break;
            case GPS_ENCENDIDO_SENAL_PERFECTA:
                Serial.println(F(" Gps activado,Calidad de senal gps: Perfecta"));                
                break;
            case GPS_ERROR_COMANDO:
                Serial.println(F(" Error al enviar el comando status gps mala respuesta o no hay bytes"));
                break;
            default:
                break;
            }














            Serial.println(F("Obteniendo IMEI para guardarlo en la EEPROM..."));
            String imeiTemporal = sim808.getIMEI();    
            Serial.println(imeiTemporal);
            if (imeiTemporal.length() > 10) {
                luisdaEE.setIMEI(imeiTemporal);
                Serial.println(F("Imei guardado en la EEPROM"));
                Serial.println(luisdaEE.getIMEI());
            }
            else {
                Serial.println(F("El modulo ha devuelto un IMEI vacio es solo un error temporal"));
            }










            Serial.print(F("\n\nConsultando la calidad de la senal de red "));
            fuerzaDeLaSenal = sim808.getFuerzaSenal();
            Serial.print(F("La fuerza de la senal es: "));
            Serial.println(String(fuerzaDeLaSenal));
            Serial.println(F("0 sin senal___ 0-4.5 Debil ___ 4.5-9 Mala ___ 9-12.5 Regular ___ 12.5-17 Buena ___ 17-31 Excelente"));
            









            Serial.println(F("Revizando si el gestor de carga de bateria del modulo esta activado"));
            if (sim808.getGestorBateriaActivado()) {
                Serial.println(F("El gestor de bateria esta activado"));
            }
            else {
                Serial.println(F("El gestor de bateria esta desactivado, activando gestor..."));
                sim808.setActivarDesactivarGestorBateria(true);
            }






            Serial.println(F("Actualizando los niveles de la bateria"));
            sim808.actualizarInformacionBateria();
               
            












            Serial.println(F("\n\nRevisando el estado del modulo GPRS"));
            if (!sim808.gprsComprobarEstado()) {
                //si el gprs esta apagado lo activamos
                Serial.println(F(" GPRS apagado, activando gprs"));
                sim808.gprsActivar_desactivar(true);

                sePuedenHacerConexionesGPRS = false;

                //Aqui ocurrio algo extraño en las practicas con los carros paso que el gprs siempre estaba apagado
                /*
                se enviaba el comando de revizarlo y devolvia que estaba apagado y activando gprs, el comando de activar respondia error
                esto ocurria una y otra y otra vez, deecho el vehiculo 36 dejo de enviar datos de todo el viernes por culpa de eso
                y el mudulo sim en efecto no entraba en gprs el foquito parpadeaba lento es decir con señal gsm solamente
                entonces lo unico que hice fue reiniciar el modulo sim808 y en cuanto lo reinicie, ahora si se activo el gprs
                entonces lo que haremos sera almacenar en el gestor de errores, cuando el sim gprs esta apagado, que en teoria jamas deberia
                de estarlo y reiniciaremos el modulo
                */

                contadorNumeroDeVecesGPRSApagado++;         //aumentamos esta variable cada que entremos aqui y el gprs este apagado, esta variable se reiniciara cuando responda que ya se encendio, si nunca se enciende esto seguira aumentando hasta que llege a 5 y entonces reiniciaremos el modulo
                Serial.print(F("Aumentando contador numero de veces apagado seguidas para evitar error donde no se puede activar el gprs -->")); Serial.println(String(contadorNumeroDeVecesGPRSApagado));
                //------>LuisdaManejoSD::escribirArchivo(_LUISDA_SD_NOMBRE_LOCALIZACION, "Modulo gprs esta apagado");

            }
            else {
                Serial.println(F("El GPRS esta activado"));
                contadorNumeroDeVecesGPRSApagado = 0;               //reiniciamos el contador siempre que este encendido para qque no reinicie el modulo sim

                //preguntamos si el modulo esta en contexto gprs creado exitosamente
                
                switch (sim808.gprsStatusDelContextoCIP())
                {

                case 0:
                    //si no hay ningun contexto gprs abierto, abrimos uno
                    
                    Serial.println(F(" No hay contexto gprs activo, iniciando y entrando en modo gprs"));
                    //AT+CSTT="internet.itelcel.com","webgprs","webgprs2003"
                    sim808.gprsAbrirContextoGPRS("internet.itelcel.com", "webgprs", "webgprs2003");
                    sePuedenHacerConexionesGPRS = false;
                    break;

                case 1:
                    //Si indica IP START significa que se definio el apn pero solo eso
                    //esto es por algun error que nuestro metodo solo envio el apn pero no continuo con los otros pasos de la conexion
                    //qias arduino se detuvo entonces el modulo se quedo a la mitad entonces ahora cerraremos todo otra vez

                    Serial.println(F("Solo se definio el APN pero no se siguieron los demas pasos por algun error, volviendo a definir apn"));
                    //AT+CSTT="internet.itelcel.com","webgprs","webgprs2003"
                    sim808.gprsAbrirContextoGPRS("internet.itelcel.com", "webgprs", "webgprs2003");
                    sePuedenHacerConexionesGPRS = false;
                    break;



                case 2:
                    //Si indica GPRSACT significa que se definio el apn se cambio a modo gprs esta parpadeando rapido pero al solicitar una direccion Ip ocurrio un error

                    Serial.println(F("Activamos el modo GPRS el led esta parpadeando rapido pero No obtubimos una direccion IP"));
                    //AT+CSTT="internet.itelcel.com","webgprs","webgprs2003"
                    sim808.gprsAbrirContextoGPRS("internet.itelcel.com", "webgprs", "webgprs2003");
                    sePuedenHacerConexionesGPRS = false;
                    break;

                case 5:
                    //ubo un error en mis pruebas depronto por mas que intentaba conectarce al servidor nos devolvia 
                    //TCP CONNECTING y nunca resolvia la conexion tardaba mucho al final marcana fail
                    //pero ya despues como de 30 segundos
                    //para resolver eso solo reinicie el modulo. Quizas tambien funcione con SHUT para que se cierre el contexto
                    //y se vuelva a abrir desde 0
                    Serial.println(F(" Esta en proceso de conexion ya tardo demaciado revisar datos estamos obteniendo TCP CONNECTING"));
                    contadorGestorDeErroresModuloSim++;

                    sePuedenHacerConexionesGPRS = false;


                    break;

                case 7:
                    Serial.println(F(" Ocurrio un error al tratar de preguntar el estatus del contexto"));
                    contadorGestorDeErroresModuloSim++;
                    sePuedenHacerConexionesGPRS = false;
                    break;
                
                default:
                    Serial.println(F("Estamos en modo GPRS contexto abierto exitosamente Se pueden hacer conexiones"));

                    sePuedenHacerConexionesGPRS = true;
                    
                    //sim808.gprsAbrirConexionConServidor("TCP", "162.241.93.228", "80");
                    /*
                    Una vez que el modulo gprs esta activo y estamos conectados a la red, vamos a abrir la conexion al servidor
                    esto lo hacemos para que la conexion ya este abierta y pueda ser usada en la parte de abajo por otros codigos para enviar
                    datos, de esta manera no tendremos que esperar a que nos responda COnect OK al menos en la primera conexion donde lo usen
                    por ejemplo al llegar a donde se envian las coordenadas, deberemos preguntar por el status del cip si responde CONNECT OK 
                    lo bueno es que eso ya es una respuesta inmediata es decir ya no nos detenemos mas tiempo por crear una conexion al servidor y esperar 1 o 2 segundos el connect ok
                    si ya esta la conexion creada simplemente enviamos los datos, pero si responde algo diferente de CONNECT OK entonces si abrimos la conexion alla abajo

                    lo que pasara es que:
                   aqui se abrira la conexion si antes no estaba abierta y se quedara asi
                    se quedara abierta, si el codigo de las coordenadas cumple su tiempo reviza si la respuesta del CIPSTATUS   es CONNECT Ok si lo es envia los datos directos ahi ahorramos tiempo del procesador
                    YA QUE NO se teine que volver a abrir la conexion

                    si hay otro codigo abajo que envie datos como el acererometro, como la conexion que se abrio aqui ya la uso el  de las coordenadas, se tendra que crear una nueva conexion y lo hara en automatico
                    porque el status respondeta quizas TCP CLOSE o algo diferente pero no CONNECT OK

                    quizas seria bueno porque ya no tienes que abrir una conexion nueva cada que quieres enviar datos sino que aqui ya deja una lista, pero seria bueno si solo en todo el codigo te conectas una sola vez

                    lo malo es que como todas las conexiones se cierran cada que envias datos entonces si por ejemplo ahora abajo del codigo que envio las coordenadas
                     queremos enviar los datos del acelerometro porque ya se vencio su timer casualmente al mismo tiempo del de las coordenadas
                     entonces igual tendremos que abrir la conexion ahi y el beneficio de abrir la conexion aqui no habra sido muy bueno y quizas se vuelve algo mas
                    revoltoso y dificil de mantener el codigo, y es probable que no resolvamos mucho abriendo la conexion primero aqui para que despues la use otro codigo
                    sino que aqui se abre, si nos responde ok o no
                    no pasa nada porque en los siguentes codigos lo que haran es preguntar el estado de la conexion donde les dira que esta conectado
                    si esta conectado enviamos la info, si no entonces no la enviamos y guardamos en la sd y esperamos hasta que este codigo se vuelva a llamar al siguiente loop 
                    para abrir la conexion. ahora si necesitas que se abra la conexion la puedes abrir cuando quieras pero tendriamos que esperar los 2 o 3 segundos para obtener la respuesta
                    CONNECT OK
                    */
                    
                    
                    
                    break;
                }

                
              
            }





            

           
        }


        Serial.println(F("\n\n\n\n--------Fin Configuraciones Del Modulo--------\n\n\n\n"));
        
    }
    







    if (retardoPingAlServidor.seHaCumplido(true)) {


        Serial.println(F("\n\n\n\n----Obteniendo configuraciones del servidor-----\n\n\n\n"));

        if (sePuedenHacerConexionesGPRS) {

            if (sim808.gprsAbrirConexionConServidor("TCP", "162.241.93.228", "80", contadorGestorDeErroresModuloSim)) {


                if (sim808.gprsEnviarInformacionPOST("162.241.93.228", "pruebasim808.php", "", luisdaEE.getIMEI(), "", "", "")) {
                    //si se envio correctamente la informacion y se recibio la respuesta del server tendremos guardado el body de la respuesta recibida
                    //enviaremos ese body a nuestra clase que manejara el arduino GObernador del vehiculo.

                    if (bodyConfiguracionesAnterior != sim808.gprsGetBodyUltimo()) {
                        Serial.println(F("El body configuraciones recibido es diferente actualizamos configuraciones"));
                        luisdaEE.setConfiguracionesDesdeString(sim808.gprsGetBodyUltimo());


                        retardoEnviarDatosEnSD.setRetardo(luisdaEE.configuraciones.tiempoEnviarDatosSD);
                        retardoSolicitudDeCoordenadas.setRetardo(luisdaEE.configuraciones.tiempoEnvioCoordenadas);
                        retardoEnviarLecturasActualesAcelerometro.setRetardo(luisdaEE.configuraciones.tiempoEnvioLecturasAcelerometro);
                        retardoPingAlServidor.setRetardo(luisdaEE.configuraciones.tiempoPing);



                        if (luisdaEE.configuraciones.calibrarAcelerometro) {
                            if (acelerometro.calibrar()) {
                                luisdaEE.configuraciones.calibrarAcelerometro = false;
                                luisdaEE.setConfiguraciones();
                            }
                        }


                        bodyConfiguracionesAnterior = sim808.gprsGetBodyUltimo();
                    }
                    else {
                        Serial.println(F("El body configuraciones recibido es igual a uno anterior no actualizamos configuraciones"));
                    }

                }
                else {
                    Serial.println(F("Se genero un error en la peticion, NO SE APLICAN LAS CONFIGURACIONES"));
                }



                sim808.enviarComandoSinEvaluar(AT_GPRS_CIERRA_CONEXION, _MENOR_A_128_BYTES, nullptr);
                //aqui no me importa lo que responda si la cierra o responde error                          

            }

            Serial.println(F("\n\n\n\n----Fin Obteniendo configuraciones del servidor-----\n\n\n\n"));

        }
        else {
            Serial.println(F("\n\n\n\n----Fin Obteniendo configuraciones del servidor No se pueden hacer conexiones GPRS-----\n\n\n\n"));
        }

    }








    if (retardoAdministrarRelojRTC.seHaCumplido(true)) {


        Serial.println(F("\n\n\n\n--------Administrar Reloj RTC--------\n\n\n\n"));



        administrarRelojRTC();


        Serial.println(F("\n\n\n\n--------Fin Reloj RTC--------\n\n\n\n"));


    }



    
    
  
  //actualizar coordenadas de acceso rapido   
    if (retardoactualizarCoordenadasPrincipales.seHaCumplido(true) && (estatusDelGps == GPS_ENCENDIDO_SENAL_MEDIA || estatusDelGps == GPS_ENCENDIDO_SENAL_PERFECTA)) {
        sim808.actualizarLatLongSpeedAccesoRapido();
        Serial.print(F("Lat: "));
        Serial.print(String(sim808.getLatitudRapido(),6));
        Serial.print(F("\tLon: "));
        Serial.print(String(sim808.getLongitudRapido(),6));
        Serial.print(F("\tVel: "));
        Serial.print(String(sim808.getVelocidadRapido()));
        Serial.print(F("\tCurs: "));
        Serial.println(String(sim808.getDireccionRapido()));
    }
    
    



    
    //Envio de coordenadas al servidor   
    if (retardoSolicitudDeCoordenadas.seHaCumplido(true)) {

        Serial.println(F("\n\n\n\n--------Enviar Coordenadas al servidor--------\n\n\n\n"));



        if (estatusDelGps == GPS_APAGADO) {
            //NO CONSULTAMOS LAS COORDENADAS HASTA QUE EL GPS SE INICIE Y ESCRIBIMOS EN LA SD QUE EL GPS NO ESTABA ENCENDIDO Y LA HORA
            //SI INTENTAMOS ENVIAR UN COMANDO QUE CONSULTE COORDENADAS CON EL MODULO GPS APAGADO RESPONDERA ERROR
            //enviarStringComoCharArray(sim808.consultarFechaHoraDelModulo(), 132);
            //enviamos al modulo sd la respuesta
            //Serial.println(sim808.consultarFechaHoraDelModulo());

            LuisdaManejoSD::escribirArchivo(_LUISDA_SD_NOMBRE_LOCALIZACION, "estatus gps apagado");


        }

        if (estatusDelGps == GPS_ENCENDIDO_SIN_SENAL) {
            //SI NO TIENE SE�AL GPS PONDREMOS SIN SENAL EN LA SD Y LA HORA
        } 



        if (estatusDelGps == GPS_ENCENDIDO_SENAL_MEDIA || estatusDelGps == GPS_ENCENDIDO_SENAL_PERFECTA) {
            

            

            String localizacion = sim808.getLocalizacion(_CADENA_RETORNO_FECHAHORA_LAT_LON_ALT_VEL_DIR_PRECI_SATELL_CALIDAD, luisdaEE.configuraciones.zonaHoraria);
            localizacion += "," + String(accVehiculo) + "," + String(glpVehiculo);

            /*
            Cuando la señal sea muy baja o cuando el gprs no se ha podido iniciar guardaremos todas las ubicaciones del gps en la memoria sd, y evitaremos enviar las coordenadas
            al servidor para no perder tiempo
            */
            if (sePuedenHacerConexionesGPRS && fuerzaDeLaSenal >= 5) {

                if (sim808.gprsAbrirConexionConServidor("TCP", "162.241.93.228", "80", contadorGestorDeErroresModuloSim)) {

                    if (!sim808.gprsEnviarInformacionPOST("162.241.93.228", PHP_GPS_ARCHIVO_LOCALIZACION, localizacion, luisdaEE.getIMEI(), "", "", "")) {

                        Serial.println(F("guardando info en la sd no se recibio sendOk"));
                        LuisdaManejoSD::escribirArchivo(_LUISDA_SD_NOMBRE_LOCALIZACION, localizacion);

                    }

                }
                else {
                    Serial.println(F("guardando info en la SD fracaso la conexion"));
                    LuisdaManejoSD::escribirArchivo(_LUISDA_SD_NOMBRE_LOCALIZACION, localizacion);
                }

            }
            else {
                Serial.println(F("guardando info en la SD, No se pueden hacer conexiones GPRS o la señal es menor a 5"));
                LuisdaManejoSD::escribirArchivo(_LUISDA_SD_NOMBRE_LOCALIZACION, localizacion);
                
            }
       
            
            
           
            

        }



        sim808.enviarComandoSinEvaluar(AT_GPRS_CIERRA_CONEXION, _MENOR_A_32_BYTES, nullptr);        //Se haya o no abierto una conexion aqui la cerramos para evitar que algun metodo de abajo tenga problemas al intentar abrir una conexion que ya esta abierta y de la nada el servidor la cierra a medio envio de datos

        Serial.println(F("\n\n\n\n--------Fin Enviar Coordenadas al servidor--------\n\n\n\n"));
        
    }
    








    //enviar datos sd
    if (retardoEnviarDatosEnSD.seHaCumplido(true)&& memoriaSDInicializadaCorrectamente) {

  
    Serial.println(F("\n\n\n\n--------Enviar datos de la SD--------\n\n\n\n"));



    


    /*
    Antes de abrir una conexion preguntamos si existe un archivo de los que queremos enviar
    porque al abrir una conexion tardamos un poco de tiempo    
    */

    long tamanoDeLocalizacion = LuisdaManejoSD::getTotalSizeOf(_LUISDA_SD_NOMBRE_LOCALIZACION);
    long tamanoDeAcelerometro = LuisdaManejoSD::getTotalSizeOf(_LUISDA_SD_NOMBRE_ACELEROMETRO);
    long tamanoDeFlujometro = LuisdaManejoSD::getTotalSizeOf(_LUISDA_SD_NOMBRE_FLUJOMETRO);

    long posicionInicialLocalizacion = LuisdaManejoSD::getUltimaPosicionEnviada(_LUISDA_SD_NOMBRE_LOCALIZACION_ULTIMA_POSICION);
    long posicionInicialAcelerometro = LuisdaManejoSD::getUltimaPosicionEnviada(_LUISDA_SD_NOMBRE_ACELEROMETRO_ULTIMA_POSICION);
    long posicionInicialFlujometro = LuisdaManejoSD::getUltimaPosicionEnviada(_LUISDA_SD_NOMBRE_FLUJOMETRO_ULTIMA_POSICION);

    Serial.print(F("Archivo Local: "));
    Serial.print(String(tamanoDeLocalizacion));
    Serial.print(F("\tPosicion inicial: "));
    Serial.println(String(posicionInicialLocalizacion));

    Serial.print(F("Archivo Acelerometro: "));
    Serial.print(String(tamanoDeAcelerometro));
    Serial.print(F("\tPosicion inicial: "));
    Serial.println(String(posicionInicialAcelerometro));


    Serial.print(F("Archivo Flujometro: "));
    Serial.print(String(tamanoDeFlujometro));
    Serial.print(F("\tPosicion inicial: "));
    Serial.println(String(posicionInicialFlujometro));

    //if (SD.exists(_LUISDA_SD_NOMBRE_LOCALIZACION) || SD.exists(_LUISDA_SD_NOMBRE_FLUJOMETRO) || SD.exists(_LUISDA_SD_NOMBRE_ACELEROMETRO)) {
    if (sePuedenHacerConexionesGPRS && (tamanoDeLocalizacion > 0 ||tamanoDeAcelerometro > 0 || tamanoDeFlujometro > 0) && fuerzaDeLaSenal >= 5) {
        Serial.println(F("Existen datos nuevos en la SD para enviar, Detenemos las interrupciones Wire i2c"));

        Wire.end();


      



        

            /*
            Si hay datos en la sd sobre localizacion entonces enviamos esos datos
            cuando no haya mas datos en automatico el metodo enviar 1000 bytes de SD se encarga de borrar el archivo de la SD
            asi evitamos que arduino entre a los demas metodos si el archivo no existe, aunque los demas metodos se encargan de identificar
            si el archivo a leer existe o no, pero asi evitamos que el procesador malgaste tiempo
            */
            if (tamanoDeLocalizacion > 0) {
                
                if (sim808.gprsAbrirConexionConServidor("TCP", "162.241.93.228", "80", contadorGestorDeErroresModuloSim)) {


                    LuisdaManejoSD::enviar1000bytesEnSDporSim808(&sim808, "162.241.93.228", PHP_GPS_ARCHIVO_LOCALIZACION, _LUISDA_SD_NOMBRE_LOCALIZACION, _LUISDA_SD_NOMBRE_LOCALIZACION_ULTIMA_POSICION, posicionInicialLocalizacion, luisdaEE.getIMEI());

                    /*
                    me aseguro de cerrar la conexion que cre porque las conexiones a veces tardan en cerrarse, por ejemplo ahorita
                    abri esta conexion al servidor, ya envio los datos con exito se supone que el servidor deberia de cerrarla inmediatamente
                    pero en ocaciones tarda, entonces al legar a otra parte del codigo que habre una conexion por ejemplo el envio de coordenadas
                    en ese momento le dice que la conexion ya esta establecida ALREADY CONNECT y justo cunado va a enviar los datos
                    el servidor cierra la anterior conexion y entonces el metodo que la estaba usando se queda a medias del envio y devuelve error
                    por lo tanto esa coordenada se guardara en la SD, para evitar eso aqui cerramos la conexion
                    */
                    sim808.enviarComandoSinEvaluar(AT_GPRS_CIERRA_CONEXION, _MENOR_A_32_BYTES, nullptr);
                    //aqui no me importa lo que responda si la cierra o responde error                          

                }

            }





            if (tamanoDeFlujometro > 0) {
                
                if (sim808.gprsAbrirConexionConServidor("TCP", "162.241.93.228", "80", contadorGestorDeErroresModuloSim)) {


                    LuisdaManejoSD::enviar1000bytesEnSDporSim808(&sim808, "162.241.93.228", PHP_FLUJOMETRO, _LUISDA_SD_NOMBRE_FLUJOMETRO, _LUISDA_SD_NOMBRE_FLUJOMETRO_ULTIMA_POSICION, posicionInicialFlujometro, luisdaEE.getIMEI());

                    /*
                    me aseguro de cerrar la conexion que cre porque las conexiones a veces tardan en cerrarse, por ejemplo ahorita
                    abri esta conexion al servidor, ya envio los datos con exito se supone que el servidor deberia de cerrarla inmediatamente
                    pero en ocaciones tarda, entonces al legar a otra parte del codigo que habre una conexion por ejemplo el envio de coordenadas
                    en ese momento le dice que la conexion ya esta establecida ALREADY CONNECT y justo cunado va a enviar los datos
                    el servidor cierra la anterior conexion y entonces el metodo que la estaba usando se queda a medias del envio y devuelve error
                    por lo tanto esa coordenada se guardara en la SD, para evitar eso aqui cerramos la conexion
                    */
                    sim808.enviarComandoSinEvaluar(AT_GPRS_CIERRA_CONEXION, _MENOR_A_32_BYTES, nullptr);
                    //aqui no me importa lo que responda si la cierra o responde error                          

                }


            }




            if (tamanoDeAcelerometro > 0) {

                if (sim808.gprsAbrirConexionConServidor("TCP", "162.241.93.228", "80", contadorGestorDeErroresModuloSim)) {

                    LuisdaManejoSD::enviar1000bytesEnSDporSim808(&sim808, "162.241.93.228", PHP_ACELEROMETRO_ENVIAR_CONTADORES, _LUISDA_SD_NOMBRE_ACELEROMETRO, _LUISDA_SD_NOMBRE_ACELEROMETRO_ULTIMA_POSICION, posicionInicialAcelerometro, luisdaEE.getIMEI());

                    /*
                    me aseguro de cerrar la conexion que cre porque las conexiones a veces tardan en cerrarse, por ejemplo ahorita
                    abri esta conexion al servidor, ya envio los datos con exito se supone que el servidor deberia de cerrarla inmediatamente
                    pero en ocaciones tarda, entonces al legar a otra parte del codigo que habre una conexion por ejemplo el envio de coordenadas
                    en ese momento le dice que la conexion ya esta establecida ALREADY CONNECT y justo cunado va a enviar los datos
                    el servidor cierra la anterior conexion y entonces el metodo que la estaba usando se queda a medias del envio y devuelve error
                    por lo tanto esa coordenada se guardara en la SD, para evitar eso aqui cerramos la conexion
                    */
                    sim808.enviarComandoSinEvaluar(AT_GPRS_CIERRA_CONEXION, _MENOR_A_32_BYTES, nullptr);
                    //aqui no me importa lo que responda si la cierra o responde error         
                }

            }









            Serial.println(F("Reactivamos las interrupciones Wire i2c"));

            Wire.begin(MI_DIRECCION_I2C);
            

    }
    else {
        Serial.println(F("No se pueden hacer conexiones GPRS o no existen datos nuevos en la SD para enviar, o la intencidad de la senal es muy debil"));
        Serial.print("Se pueden hacer conexiones: ");
        Serial.println((sePuedenHacerConexionesGPRS) ? "true" : "false");
        Serial.println("Intencidad de la senal " + String(fuerzaDeLaSenal));
    }

    


    

    
  


    Serial.println(F("\n\n\n\n--------Fin Enviar datos de la SD--------\n\n\n\n"));
    

}







    //enviar datos acelerometro al servidor las lecturas actuales para ver que este bien calibrado
if (luisdaEE.configuraciones.enviarLecturasAcelerometro && retardoEnviarLecturasActualesAcelerometro.seHaCumplido(true) && sePuedenHacerConexionesGPRS) {
    /*
    Abrimos una conexion para enviar las lecturas reales del acelerometro las cuales nos serviran para ver si encontramos algun error en las lecturas
    o saber si es necesario calibrar el sensor
    */

    if (acelerometro.requestActualizarDatosLecturasActuales()) {        //HACEMOS EL REQUEST Y SI TENEMOS DATOS DE RESPUESTA SE GUARDARAN EN LA ESTRUCTURA ENTONCES LOS ENVIAMOS


        Serial.println(F("\n\n\n\n----Enviando lecturas Acelerometro unica vez-----\n\n\n\n"));

        if (sim808.gprsAbrirConexionConServidor("TCP", "162.241.93.228", "80", contadorGestorDeErroresModuloSim)) {


            if (sim808.gprsEnviarInformacionPOST("162.241.93.228", PHP_ACELEROMETRO_LECTURAS_REALES, acelerometro.imprimirLecturasEnEstructura() + "," + Utilerias::imprimirFechaHoraDigitos(Utilerias::convertirHoraUTC(now(), luisdaEE.configuraciones.zonaHoraria)), luisdaEE.getIMEI(), "", "", "")) {
                //si se envio correctamente la informacion y se recibio la respuesta del server tendremos guardado el body de la respuesta recibida
                

                        
                Serial.println(F("Se han enviado las lecturas actuales del acelerometro al servidor"));
                

            }



            sim808.enviarComandoSinEvaluar(AT_GPRS_CIERRA_CONEXION, _MENOR_A_128_BYTES, nullptr);
            //aqui no me importa lo que responda si la cierra o responde error                          

        }

        Serial.println(F("\n\n\n\n----Enviando lecturas Acelerometro unica vez-----\n\n\n\n"));



    }

    }






    //enviamos y recibimos datos 
    
    if (Serial.available() > 0) {
        Serial3.write(Serial.read());        
        
    }
    
    if (Serial3.available() > 0) {
        Serial.write(Serial3.read());
        //Serial.println("holaaa");
    }

    
    
    










    if (contadorGestorDeErroresModuloSim >= 6 || contadorNumeroDeVecesGPRSApagado >=4) {
        //si ya obtuvimos 4 errores similares cada que se entre a estos procesos por ejemplo la entrada a esto es de 10s
        //en 40 segundos tendriamos 4 intentos
        sim808.resetDelModuloDelays();//ordenamos un reset del modulo, aquneuq creo que con un SHUT Ok deberia tener
        //pero si el shut ok retorna error significa que el error con el modulo es mas grave ahi si seria bueno resetearlo
        Serial.println(F("Demaciados errores con el modulo, el gestor de errores esta reiniciando el modulo "));
        //sim808.enviarComandoSinEvaluar(AT_GPRS_CIERRA_CONTEXTO, _MENOR_A_32_BYTES, nullptr);
        contadorGestorDeErroresModuloSim = 0;
        contadorNumeroDeVecesGPRSApagado = 0;
    }






   

    

    








    /*
   Si se detectan revoluciones en el acelerometro desde la ultima vez que se reinicio a 0 entraremos a un bucle para mantener el procesador
   concentrado en leer el acelerometro, en cada bucle while se calculan los datos del acelerometro, y este metodo a su vez retorna true
   usandose para mantenerse dentro del bucle while mientras se esten recibiendo entrada de glp en el flujometro, cuando no haya entrada de gas lp
   el metodo seguira retornando true hasta que pasen 10 segundos de la ultima entrada y al fin retornara false, saliendo del bucle while
   y dejando el procesador libre para el trabajo del acelerometro.

   Ahora se que no es bueno usar delays porque basicamente el procesador se queda atorado y no lo dejas hacer mas tareas, pero debido a que
   es lo que queremos, con el bucle while queremos que el procesador al detectar flujo no haga nadamas mas que leer el flujometro
   entonces creo que para mas facil usaremos un delay dentro del while este no nos afectara y definira el tiempo al que se calcularan los datos
   tanto de velocidad, el tamaño maximo del delay que podremos usar dependera de nuestro watch dog ya que al estar en delay el procesador no llegara a la instruccion
   que reinicia el watch dog, cosa diferente si usaramos la clase retardoConMillis, pero para nuestra aplicacion creo que esta muy bien es mas basico y claro

   Aunque el procesador este en un delay, las interrupciones se siguen contando sin problemas, durante esos 500ms en la variable estatica se tendran el numero total de interrupciones que ocurreron
   y al llegar a calcularFlujometro se usaran para calcular los datos de litros
   */
    if (flujometro1.flujoDetectado() && memoriaSDInicializadaCorrectamente) {

        Serial.println(F("\n\n\n\n*****Flujo detectado en el FLujometro de GLP, entrando en bucle para leer flujometro"));

        while (flujometro1.calcularFlujometro())
        {
            /*
            Al llamarce el calcularFLujometro ahora tenemos en sus campos del objeto flujometro almacenados los datos
            del ultimo calculo, esos datos los imprimiremos en el serial
            */
            wdt_reset();
            Serial.print(F("PulsosTemporale: ")); Serial.println(String(flujometro1.getPulsosTemporales()));
            Serial.print(F("LitrosPorMinuto: ")); Serial.println(String(flujometro1.getLitrosPorMinuto()));
            Serial.print(F("PulsosTotales__: ")); Serial.println(String(flujometro1.getPulsosTotales()));
            Serial.print(F("LitrosTotales__: ")); Serial.println(String(flujometro1.getLitrosTotales()));
            Serial.print(F("IntervaloTiem__: ")); Serial.println(String(flujometro1.getIntervaloTiempo()));
            Serial.println(flujometro1.temporalesToString());
            Serial.println();

            delay(1000);                         //contaremos las interrupciones totales cada 500ms, este valor debe de ser el mismo que se le pase a calcularFlujometro para que los datos de velocidad sean correctos
            LuisdaManejoSD::escribirArchivo(_LUISDA_SD_NOMBRE_FLUJOMETRO, flujometro1.temporalesToString() + ",,,,");
            
        }

        /*
        Cuando el metodo calcular flujometro detecte que ya no hay mas flujo de gas por el tubo despues de 10 segundos
        entonces retornara false saliendo del bucle while e indicando que el "Periodo de carga ha termionado"
        en esteo moemto deberiamos de consultar la variable total, pulsos totales enviarla y despues reiniciarla
        */

        Serial.println(F("*****El periodo de carga ha terminado, ya no se ha detectado flujo de GLP despues de 10 segundos, reiniciamos para un nuevo periodo de carga"));
        Serial.print(F("PulsosTotalesRegistrados: ")); Serial.println(String(flujometro1.getPulsosTotales()));
        Serial.print(F("LitrosTotalesRegistrados: ")); Serial.println(String(flujometro1.getLitrosTotales()));
        Serial.print(F("DuracionDelPeriodo___(s): ")); Serial.println(String(flujometro1.getPeriodoCargaReal()));

        String datos = flujometro1.totalesToString() + "," + String(sim808.getLatitudRapido(), 6) + "," + String(sim808.getLongitudRapido(), 6) + "," + Utilerias::imprimirFechaHoraDigitos(Utilerias::convertirHoraUTC(now(), luisdaEE.configuraciones.zonaHoraria));
        Serial.println(datos);
        Serial.print(F("\n\n\n\n"));
        LuisdaManejoSD::escribirArchivo(_LUISDA_SD_NOMBRE_FLUJOMETRO,datos);
        
    }


   

    







    




     

        






    



    

    
}





//si no uss el constructor de String para crear un String desde char array puedes usar esta funcion
String convertToString(char* a, int size)
{
    int i;
    String s = "";
    for (i = 0; i < size; i++) {
        s = s + a[i];
    }
    return s;
}




//Esta seria la funcion a seguir si lo que queremos es limitarnos a los 32 bytes de buffer
//en un byte enviamos el id le servira al esclavo para saber que informacion le va a llegar podria ser las coordenadas solo la hora etc
//el otro byte es usado para enviar la longitud de la informacion si solo son 20 caracteres o 25 etc para que sea variable la info
//y nos quedan 30 bytes para hacer un array y dentro enviar la informacion necesaria
bool enviarStringComoCharArray(String porEnviar, byte id) {

    char buffer[30];
    memset(buffer, '\0', 30);       //al inicializarlo con valorez vacios al enviar el array la longitud en bytes solo sera igual a los caracteres que tenga dentro, aunque lo definimos de 30 bytes todos los ponemos como vacios y solo llenamos 20 esos 20 son los que marcara como longitud, si no inicializamos el array vacio marca los 30 pero los sobrantes llenos de basura
    byte size = porEnviar.length();
    porEnviar.toCharArray(buffer, 30);

    Wire.beginTransmission(DIRECCION_ESCLAVO_ACELEROMETRO_I2C); // LA ENVIAREMOS AL ARDUINO QUE MANEJA EL MODULO SD
    Wire.write(id);
    Wire.write(size);
    Wire.write(buffer);


    if (Wire.endTransmission() == 0) {
        Serial.println(F("La transmision fue exitosa"));
        return true;
    }
    else {
        Serial.println(F("la direccion del esclavo la transmision fracaso, o el programa del esclavo"));
        return false;
    }

    
}


/*
Mi sistema lo que hara es cuando un esclavo no pueda esperar a que este maestro lo lea por ejemplo el modulo acelerometro
cuando se reinicie no tendra configuraciones, nuestro maestro cada cierto tiempo reenviara los datos de configuracion al esclavo
pero si el esclavo no puede esperar hasta que el maerstro le envie, entonces el esclavo se convertira en maestro y nos escribira
solo nos enviara
su direccion de esclavo 
y un id de la informacion que quiere porque quizas podria haber muchas

entonces lo que pasara es que aqui distinguiremos que esclavo fue y que informacion quiere para despues
reiniciar los retardos y enviarle la informacion en el siguiente loop,

no podemos escribir con Wire.write dentro de esta funcion el arduino muere.

Y otra no podian ser request porque los request solo solicitan un numero de bytes
pero en este caso no sabriamos a quien la solicito y no podemos enviar otro id en el reques
para saber que informacion le enviaremos. entonces creo que esta fue la mejor solucion, debido a que tendremos
varios esclavos.



*/
void onReceiveEvent(int howMany) {
    Serial.println(F("\n-OnReceiveEvent-\n"));
    Serial.print(F("Datos recibidos por el bus i2c bytes disponibles: "));
    Serial.println(String(howMany));
    wdt_reset();
    byte addressSolicitante = 0;
    byte idInformacionRequerida = 0;

    if (howMany) {

        addressSolicitante = Wire.read();
        idInformacionRequerida = Wire.read();
        Serial.print(F("Direccion del que envia: "));
        Serial.println(String(addressSolicitante));
        Serial.print(F("Id de la informacion que requiere el solicitante: "));
        Serial.println(String(idInformacionRequerida));

        if (addressSolicitante == DIRECCION_ESCLAVO_ACELEROMETRO_I2C) {
            if (idInformacionRequerida == 1) {
                //sabemos que nos esta pidiendo la estructura que contiene sus configuraciones lo que haremos e
                //debido a que no podemos iniciar transmisiones Wire por aqui no podremos enviar informacion directo aqui
                //con wireWrite
                //se detiene el arduino, lo mejor sera reiniciar variables que de alguna forma al salir de esta funcion
                //force el envio de la informacion en este caso forzaremos el retardo

                Serial.print(F("Forzamos el retardo "));
            }



            if (idInformacionRequerida == ACELEROMETRO_ID_CONTADORES) {
                //sabemos que nos enviara informacion de sus contadores de acelerometros nos enviara un array de 19 bytes
                byte size = Wire.read();


                
                byte buffer[19];                
                memset(buffer, '0', sizeof(buffer));

                Serial.println("Available " + String(Wire.available()));
                Wire.readBytes(buffer, size);
                Serial.println("Available After" + String(Wire.available()));

                
                String contadores = "";
                for (size_t i = 0; i < sizeof(buffer); i++)
                {
                    contadores += String(buffer[i]);


                    if (i < sizeof(buffer) - 1) {
                        contadores += ",";
                    }


                    
                
                }

                String posicionInfo = "," + String(sim808.getLatitudRapido(), 6) + "," + String(sim808.getLongitudRapido(), 6) + "," + String(sim808.getVelocidadRapido()) + "," + Utilerias::imprimirFechaHoraDigitos(Utilerias::convertirHoraUTC(now(), luisdaEE.configuraciones.zonaHoraria));
                Serial.print(contadores);
                Serial.println(posicionInfo);

                LuisdaManejoSD::escribirArchivo(_LUISDA_SD_NOMBRE_ACELEROMETRO, contadores, posicionInfo);
                
            }





        }






        Serial.println(F("\n-Fin OnReceiveEvent-\n"));

    }
    else {
        Serial.print(F("No se recibieron bytes disponibles"));
    }

}

bool enviarCharSinLimiteDeBytes(String informacion, byte id) {
    //enviaremos en cada transmision 1 byte del array total recibido
    //recibiremos la referencia al array que esta en la clase manejoSim
    //cada que el modulo recibe una respuesta inicializa todo el array de 100bytes a valores vacios
    //de esa manera aunque 
    byte size = informacion.length();
    char buffer[150];
    informacion.toCharArray(buffer, size);
    Serial.print(F("Tamano de la info a enviar "));
    Serial.println(String(size));
    Serial.println(informacion);
    //delay(50);

    //creamos un bucle donde comenzaremos y cerraremos una transmicion en cada bucle solo enviando un byte
    //pero enviaremos 2 bytes mas 1 con el id para que el esclavo sepa como manejar esta informacion
    //y otra con el tam�o total de la info a enviar para que el esclavo valla contando los bytes que ha recibido
    //y cuantos le faltan
    for (size_t i = 0; i < size; i++)
    {
        wdt_reset();
        Wire.beginTransmission(DIRECCION_ESCLAVO_ACELEROMETRO_I2C);

        Wire.write(id); //enviamos el id
        Wire.write(size); // enviamos el tama�o total de la informacion
        Wire.write(i);      //enviamos la posicion del iterador
        Wire.write(buffer[i]); //enviamos un byte de la informacion que vamos a enviar

        if (Wire.endTransmission() == 0) {
            //Serial.println("Transmision del bit " + String(i) + "    exitosa   " + "caracter enviado " + buffer[i]);
            //delayMicroseconds(30);
        }
        else {
            Serial.print(F(" ransmision del bit "));
            Serial.print(String(i));
            Serial.println(F(" fracaso, saliendo del bucle por favor reintente el envio"));
            return false;       //en este momento salimos del bucle y de toda la funcion asi podemos reintentar el envio al recibir el false en el metodo que llamo a esta funcion
        }
    }

    //si en ningun byte fracaso el envio entonces retornamos true
    return true;

    




}
    


void administrarRelojRTC() {
/*
Esta funcion nos ayudara a manejar el Modulo RTC y por manejar me refiero a que primero verificaremos que 
nuestro reloj conectado al arduino ya haya sido puesto a fecha anteriormente por ejemplo de fabrica el reloj viene configurado con la fecha
del 0-0-1970 0:0:0 ALGUNAS VECES NOS INDICA UNA FECHA COMO DEL 2071

si al preguntar la hora de nuestro modulo RTC nos devuelve esa fecha significa que nuestro reloj es la primera vez que se conecta a arduino
y lo que haremos sera ponerlo a fecha, para darnos cuenta de ello solo preguntaremos si la fecha que nos devuelve el reloj es mayor al 2015 Y MENOR A 2050
con eso pues ya sabremos que el reloj esta de fabrica o ya anterior tenia otra fecha

Ahora de donde sacaremos esa fecha? la obtendremos del modulo gps, el modulo gps nos entrega la fecha que tienen nos satelites!!
pero esta fecha la devuelven en UTC, es decir la estandar universal, para ello yo cree el metodo del sim que nos devuelve la fecha
y hora dada por el modulo gps en la zona horaria que queramos, pero creo yo que seria buena opcion que la hora del modulo RTC
la pongamos igual en formato UTC y que solo al preguntar la hora la convirtamos en la zona horaria que deseamos,

de igual manera la hora que nos entrege el gps la validaremos que sea mayor al 2015 debido a que a veces el modulo gps
ha entregado fechas 0-0-1970, PORQUE SI NO ESTA CONECTADO EL MODULO O ESTA APAGADO, el modulo obiamente no devuelve nada pero
la variable time de nuestro metodo llega en 0 es decir 1-1-1970, etnocnes es bueno que comprobemos esto ya que si no pondriamos nuestro reloj RTC conforme ese año

quizas porque no se ha establecido contacto con los satelites o porque el modulo gps esta apagado

y asi este arduino tendra una instancia Time que llevara el tiempo actual que le indique el reloj RTC
que este a la vez estara sincronizado con los satelites gps, si por cuaqluier motivo el modulo gps no tiene la hora
el reloj RTC de arduino la llevara sin problemas, esto porque como sincronizare varios procesadores por i2c por ejemplo
el procesador que manejara el flujometro y el acelerometro debe saber exactamente a que hora paso un evento

por ejemplo un impacto en el acelerometro registrado, entonces al arrancar ese procesador le preguntara a este procesador Maestro
que horas son, el esclavo sincronizara su reloj con la libreria TIme y asi cuando ocurra un evento tomara su tiempo y no el del maestro
de esa manera nos aseguramos que los eventos rapidos queden registrados con el tiempo exacto y no se desfacen por segundos


Los modulos RTC tienden a desfazarse unos minutos cada sierto tiempo para ello preguntaremos que nuestro modulo RTC y nuestro modulo GPS no tengan un desface de mas de 10 segundos
y si lo hay entonces el modulo RTC se pondra a tiempo con el modulo GPS

Ahora es mejor que la libreria time lleve el registro del tiempo en una variable con los millis de arduino en ves de estar preguntando a cada rato por i2c al modulo RTC que hora son


*/

    


    time_t horaDelModuloGPS = sim808.getDateGpsTimeZone(ZONA_HORARIA_UTC);
    Serial.print(F("Fecha UTC del modulo gps: "));
    Serial.println(String(day(horaDelModuloGPS)) + "-" + String(month(horaDelModuloGPS)) + "-" + String(year(horaDelModuloGPS)) + " " + String(hour(horaDelModuloGPS)) + ":" + String(minute(horaDelModuloGPS)) + ":" + String(second(horaDelModuloGPS)) + "\t\t");
    
    /*
    Aunque la libreria time ya la pusimos a tiempo en el setup lo volvemos a hacer aqui no es tan necesario
    pero como este metodo se llamara cada sierto tiempo seria bueno sicronizarlos por si la libreria Time llega a tener desfaces mayores
    */
    setTime(RTC.get());
    Serial.print(F("Fecha del Reloj RTC y libreria Time: "));
    Serial.print(String(day(now())) + "-" + String(month(now())) + "-" + String(year(now())) + " " + String(hour(now())) + ":" + String(minute(now())) + ":" + String(second(now())) + "\t\t");


    if (Utilerias::tiempoMayor2015(now())) {

        Serial.println(F("El modulo RTC ya ha sido configurado con una hora diferente a la de fabrica, revisando que el tiempo del RTC y del gps no sea diferente por 10 segundos..."));

        if (Utilerias::tiempoMayor2015(horaDelModuloGPS)) {

            if (Utilerias::desfaseDeTiempo10Segundos(now(), horaDelModuloGPS)) {
                RTC.set(sim808.getDateGpsTimeZone(ZONA_HORARIA_UTC));
                setTime(RTC.get());
                Serial.println(F("Se ha usado la hora del modulo GPS para setearla en el reloj RTC "));
            }

        }
    }
    else {


        Serial.println(F("La fecha del modulo RTC no esta configurada!"));
        Serial.println(F("Consultando hora UTC del modulo GPS... "));


        if (Utilerias::tiempoMayor2015(horaDelModuloGPS)) {

            Serial.println(F("La fecha que devolvio el modulo GPS es mayor al 2015 usamos esa hora para configurar el modulo RTC"));
            Serial.println(F("Consultando hora UTC del modulo GPS con zona Horaria "));
            RTC.set(sim808.getDateGpsTimeZone(ZONA_HORARIA_UTC));

            setTime(RTC.get());         //ponemos a tiempo la libreria time acorde a la nueva hora del modulo RTC
            Serial.println(F("Se ha usado la hora del modulo GPS para setearla en el reloj RTC "));
        }
        else {
            Serial.println(F("La fecha que devolvio el modulo GPS no es mayor al 2015, esperaremos a que tenga la señal correcta"));
        }


    }


}

















void solicitarContadoresAcelerometro() {

    //https://arduino.stackexchange.com/questions/9388/reading-i2c-sensors-with-a-timer-interrupt
    /*
     //Acoording with this link when an ISR ys called the interrups are disabeleted, but if i want to use the Wire library, it needs inteerrupt,
     so with this instruccion you enable the intterrups again, it's work, but in the page said, it's no recomended enable the intterupts, i don't know why, but until here it's works

     Llamaremos este codigo solo en el ISR de un Timer o interrupcion
    */
    sei();
    Timer5.detachInterrupt();
    
    Serial.println(F("\n-OnRequestEvent-\n"));

    byte bytesRecibidos = Wire.requestFrom(DIRECCION_ESCLAVO_ACELEROMETRO_I2C,19);
    
    if (bytesRecibidos == 19) {

        byte buffer[19];
        memset(buffer, '0', 19);

        Serial.println("Available " + String(Wire.available()));
        Wire.readBytes(buffer, 19);
        Serial.println("Available After" + String(Wire.available()));


        String contadores = "";
        bool contadoresConDatos = false;
        for (size_t i = 0; i < sizeof(buffer); i++)
        {
            contadores += String(buffer[i]);


            if (i < sizeof(buffer) - 1) {
                contadores += ",";
            }


            if (buffer[i] != 0) {
                /*
                Si todos los contadores que nos envia el esclavo estan en 0 entonces no guardarmos nada en la sd
                si llega alguno diferente de 0 entonces si lo enviamos.

                Ahora esto va en este arduino porque estamos haciendo que el esclavo nos mande todos los datos, tengan o no tengan datos activos en los contadores
                seria bueno que si queremos que este arduino no invierta tiempo recibiendo las interrupciones del acelerometro entonces el esclavo se configure
                para que envie solo los datos cuando alguno de los contadores tenga valores

                de esa manera podriamos grabar periodos mas cortos de tiempo como 2 segundos y el master solo invertira tiempo cunaod el esclavo detecte que los contadores estan activados

                */
                contadoresConDatos = true;
            }

        }


        if (contadoresConDatos) {
            String posicionInfo = "," + String(sim808.getLatitudRapido(), 6) + "," + String(sim808.getLongitudRapido(), 6) + "," + String(sim808.getVelocidadRapido()) + "," + Utilerias::imprimirFechaHoraDigitos(Utilerias::convertirHoraUTC(now(), luisdaEE.configuraciones.zonaHoraria));
            Serial.print(contadores);
            Serial.println(posicionInfo);

            LuisdaManejoSD::escribirArchivo(_LUISDA_SD_NOMBRE_ACELEROMETRO, contadores, posicionInfo);
        }
        else {
            Serial.println("Los contadores llegaron en 0");
        }
        

    }
    else {
        Serial.println(F("El esclavo acelerometro no respondio"));
    }

    Serial.println(F("\n-Fin OnRequestEvent-\n"));

    Timer5.attachInterrupt(solicitarContadoresAcelerometro);
}



