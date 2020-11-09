#pragma once
#include"Arduino.h"


//los declaramos como #define en lugar de constantes como (const byte DIG_AHORRO_ENERGIA 13;)
//porque asi no usamos la memoria ram del arduino para almacenarlos
//lo que hara el compilador con los define es que sustituira donde aparesca la palabra por el numero o contenido que represente
//antes de cargarlo al programa, es como si directo en el codigo pusieras el numero de pin en cada lugar como lo usas






#define PIN_DIGITAL_FLUJOMETRO 2		//este pin tiene habilitadas las interrupciones por hardware aqui se conectara el flujometro mega tiene 6 interrupciones pin 2,3,21,20,19,18






#define PIN_RX_PARA_SIM_808 15			//POR AQUI ARDUINO RECIBIRA DATOS DEL MODULO SIM QUE LOS ENVIARA EN SU PIN TX Corresponden al arduino mega al puerto UART 3
#define PIN_TX_PARA_SIM_808 14			//POR AQUI ARDUINO ENVIARA DATOS AL MODULO SIM QUE LOS RECIBIRA EN SU PIN rX Corresponden al arduino mega al puerto UART 3
#define PIN_ON_OFF_SIM_808 9	//ENVIAREMOS UN PULSO ALTO POR 1 SEGUNDO PARA ACTIVAR EL MODULO, SI YA ESTA ENCENDIDO AL ENVIAR OTRO PULSO ALTO SE APAGARA
//trate de usar el pin 7 y pin 9 como RX pero por algo el arduin no reconocia ese pin no entiendo porque como salida de datos funciona pero como entrada no
/*
https://www.arduino.cc/en/Reference/SoftwareSerial
Aqui esta ocurriendo algo dificiljajaj  cuando configuras en el software serial los pines a usar como RX y TX
al parecer el pin TX no es el problema cualquier pin que he probado funciona como TX y en el osciloscopio vez la salida de datos
el problema es con el pin RX no cualquiera me ha funcionado como ese pin, aunque hay entrada de datos en el osciloscopio
arduino cuando le pides el available, no hay datos disponibles, como si no los viera
primero probe con el pin 16

2no
3no
4no
5 no funciono como RX
6 no funciono como RX
7 no
8 no
9 no
10 si funciona lo esta usando el sim808
11 No
12 si funciono como RX
13 Si funciono como RX
14 no funciono
15 no funciono
16 no funciono 
17 no funciono
18 no funciono
19 no funciono como RX


62 A8  funciona





If using multiple software serial ports, only one can receive data at a time.
Not all pins on the Mega and Mega 2560 support change interrupts, 
so only the following can be used for RX: 10, 11, 12, 13, 14, 15, 50, 51, 52, 53, A8 (62), A9 (63), A10 (64), A11 (65), A12 (66), A13 (67), A14 (68), A15 (69).





Lo mismo me paso con el sim 808 habia configurado el pin 7 pero no funcionaba hasta que encontre el pin 10 que si funciono
los piones fisicamente se ven bien quizas es algo con el softwareSerial funciona de cierta manera quizas detecta interrupciones o algo
trate de buscar algun patron con los pinout

el arduino mega tiene como 40 pines digitales o no se xD hay que investigar y encontrar el porque unos si funcionan y otros no

en los nano no se que patron podriamos encontrar

Ahora ocurrio algo raro configure el pin 11 como RX del arduino que eviara los datos y este funciono cuando antes no funcionaba
pero el pin 10 dejo de funcionar entonces el sim 808 no se podia comunicar, despues cambie el pin al 12 para el arduino y aun el pin 
del sim 808 no funcionaba el 10
depues puse el pin 16 uno que se que no servia y entocnes comenzo a funcionar el pin 10, cambie denuevo el arduino al pin 12 que se supone que funciona
y dejo de funcionar el pin 10 otra vez

voy a piner el pin 13 a ver que onda tampoco funciono como es eso posible significa que hay errores en el software serial
voy a investigar mas a fundo, porque si intentabamos comunicar los arduinos por el software serial ahora resulta que estamos
limitados por el numero de pines que pueden funcionar y luego que depronto unos funcionen y dejen de funcionar cuando configuras otros
o a menos que el problema sea la libreria que no podamos tener demaciadas instancias


Parece que el error pueda ser en las instancias, si tu configuras 2 instancias con diferentes pines obviamente
en el setup tu pones el begin que se necesita de cada instancia

el primer begin es remplazado por el segundo beguin y solo funciona la instancia del segundo begin

si inviertes los begin funciona solo el begin de abajo, mmm necesito saber si algo ocurre






https://forum.arduino.cc/index.php?topic=574238.0
Ya salio el peinne!!!!


Creo que ssGps.listen() y ssGsm.listen() "son el problema". Bueno, para ser más exacto,  el problema está en la librería SoftwareSerial. Esta librería no puede recibir datos simultáneamente de dos puertos emulados a la vez. Por eso dispone de la función listen().

Sacado de la documentación de
SoftwareSerial::Listen():
Habilita el puerto serie del software seleccionado para escuchar. Solo un puerto serie de software se puede escuchar a la vez. Los datos que lleguen a otros puertos emulados serán descartados. Cualquier dato ya recibido se descarta durante la llamada a listen() (a menos que la instancia dada ya esté escuchando).

Esto significa que, si se está "escuchando" uno de los puertos emulados, se "pierden" los datos que transmita en ese momento cualquier otro puerto serie emulado. Supongo que no es lo que quieres, sino que lo que buscas es recibir datos simultaneamente de varios puertos series emulados. Para ello te recomiendo que busques otra librería de emulación de puertos series. Que no tenga, ni necesite, la función listen() (y obviamente deberías de quitar las llamadas a listen()).

Prueba, si quieres, con la librería https://github.com/SlashDevin/NeoSWSerial y ten en cuenta que está limitada a cuatro velocidades de transmisión: 9600 (por defecto), 19200, 31250 (MIDI) y 38400.

Otra opción sería que cambiaras a un Arduino con más puertos series "de verdad".

jaja tienes que turnar los turnos de escucha

con listen();

Bueno entonces no es buena idea usar los puiertos serie emulados para comunicar microprocesadores

*/

/*
En este pin colocar una resistencia de 2.2k puede ser tambien de 10k pero yo opte por 2.2k como pull up a 5v
porque el modulo se reinicia enviando su pin a 0v, lo que pasa con arduino cuando se reinicia, o cuando se carga un nuevo programa
es que antes de correr el setup todos los pines se ponen en un estado indeterminado, ni 0 ni 1, por lo regular el voltaje
cae a 0 pero en el osciloscpio se ven ruidos en la linea, podria insitar a resets involuntarios del modulo cuando el modulo arduino se reinicie
con esa resistencia en el osciloscopio se ve que solo durante 7us hay una caida y no hasta 0v, y despues se eleva a 5v, cuando se envia el pin a 0v porque se quiere
el reset este cae directo a 0 es decir funciona bien con la resistencia.

y antes durante todo el tiempo del reset el pin se quedaba en estado indeterminado por 3s o mas
*/
#define PIN_RESET_SIM_808 7			//enviaremos un pulso bajo por 100ms para reiniciar el modulo y despues lo ponemos a alto y ahi lo dejamos



//Pines usados para el modulo SD arduino mega puerto SPI, usammos el pin por default de arduino como SC
#ifndef VERSION_SMD

#define PIN_CS_SD 53							//lA PRIMERA VERSION QUE HICIMOS USA EL PIN POR DEFAULT CS DEL MEGA QUE ES EL 53

#else 

#define PIN_CS_SD 30							
#define PIN_CS_CAMARA1 31
#define PIN_CS_CAMARA2 32

#endif // !SMD


#define PIN_MOSI 51
#define PIN_MISO 50
#define PIN_SCK 52




//Pines Puerto I2C arduino Mega Embebed, estos al igual que los del i2c no es necesario ponerlos, es decir solo los pongo aqui para no perder de vista que piens ya estan usados,
//pero no es que los tengas que definir para que funcione
#define PIN_SDA 20
#define PIN_SCL 21




/*
Los drivers motados en el circuito de gobernador invierten el funcionamiento, por ejemolo el swich del carro envia 12v cuando se enciende, estos 12v entran a el driver del circuito que es un transistor
el cual al recibir los 12 voltios conduce a 0v el pin del arduino que estaba como pull up a 5 voltios, cuando se apaga el swich dejan de llegar los 12 voltios al transistor deja de conducir a tierra y el pin de arduino lee 1

Lo mismo con el de glp, cuando la computadora de gas se activa envia 12 voltios a las multivalvulas, que entran al transistor y este conduce a tierra los 5v del pin pull up de arduino leyendo 0
cuando va a gasolina la computadora de glo va apagada la base del transistor no se activa y el pin no se va a tierra mantentiendo 5v o 1

El bloqueo del vehiculo, esta salida estara conectada al imputpull up del gobernador estara siempre en 1, cuando queramos bloquear enviaremos un 0 por esta salida
para que el gobernador bloquee el motor y no lo enciuenda
*/
#define PIN_SWICH_VEHICULO 41				//ENTRADA DIGITAL INPUT PULL UP, PARA LEER SI EL SWICH DEL CARRO ESTA EN ON O OF, CUANDO CAE A 0 SERA ON, CUANDO ES 1 SERA OFF
#define PIN_GLP_VEHICULO 42					//ENTRADA DIGITAL INPUT PULL UP, PARA LEER SI EL VEHICULO VA A GLP O GASOLINA, SI CAE A 0 ES GASLP SI ES 1 ES GASOLINA
#define PIN_BLOQUEO_VEHICULO 40				//SALIDA DIGITAL, enviar a 1 cuando el vehiculo no se bloque, enviar a 0 cuando se quiera bloquear





//PINES DE PUERTOS SERIE O UART ARDUINO MEGA, Y NANO
/*
En Arduino UNO y Mini Pro los pines empleados son 0 (RX) y 1 (TX).
En el caso de Arduino Mega y Arduino Due, que tienen cuatro puertos de serie,  lo conectare al puerto serie 3 por el motivo que es mas facir distingir en el codigo Serial3.print contra Serial.print y contra Serial1.print
asi evito errores al confundir Serial.print para enviar info al monitor serie de la computadora y Serial3.print al enviarlo al modulo sim, me explico?
el puerto serie 0 está conectado a los pines 0 (RX) y 1 (TX),
el puerto serie 1 a los pines 19 (RX) y 18 (TX)
el puerto serie 2 a los pines 17 (RX) y 16 (TX),
y el puerto serie 3 a los pines 15 (RX) y 14 (TX).
*/









//estos son los pines definidos por default para el puerto SPI SON LOS DEL ARDUINO NANO!!! , o estan los que tienen en la cabecera
/*
#define PIN_CS 10							//chip select a veces se encuentra como SS es decir salve select o seleccion de esclavo
#define PIN_MOSI 11
#define PIN_MISO 12
#define PIN_SCK 13
*/







