#pragma once
//Modificada desde otro archivo

//buscar mi archivo --Comunicación i2c y serial Luisda osciloscopio.docx en mis documentos por si quieres saber mas del i2c y del serial y como pude entender y diseñe algunas funciones asi

//http://www.electronicaestudio.com/docs/ISTD-034.pdf   tenemos algunos comandos utiles

//http://bluehack.elhacker.net/proyectos/comandosat/comandosat.html aqui esta la explicacion de los comandos AT ahi esta el comando de capacidad de bateria

//https://es.wikipedia.org/wiki/Conjunto_de_comandos_Hayes

//https://blog.330ohms.com/2019/10/18/guia-definitiva-de-comandos-at-gsmbt/

//https://www.logicbus.com.mx/pdf/31/Comandos_AT_generales.pdf							algunos comandos generales ESTE ESTA BUENO

//https://www.groundcontrol.com/AT_Command_Reference_5_9_1_3.htm			este tiene un monton!!!!

//https://www.manualslib.com/manual/539989/Siemens-Tc63.html?page=480#manual  ESTE MANUAL NOS INDICA CUALES CONFIGURACIONES DE USUARIO PUEDEN SER GUARDADAS EN LA MEMORIA NO VOLATIL CON EL COMANTO AT&W EN LA PAGINA 480 APARTADO 21.5

//https://m2msupport.net/m2msupport/atcbst-select-bearer-service/ PUEDES ENCONTRAR CUALQUIER DESCRIPCION DE UN COMANDO AQUI

//documentacion oficial
//https://cdn-shop.adafruit.com/product-files/1946/SIM800+Series_TCPIP_Application+Note_V1.01.pdf

/*
AT+CGMI
SIMCOM_Ltd

OK
ATI
SIM808 R14.18

OK
AT+IPR=?
+IPR: (),(0,1200,2400,4800,9600,19200,38400,57600,115200,230400,460800)

OK
AT+IPR?
+IPR: 0

OK
AT+COPS?
+COPS: 0,0,"334020"

OK
AT+CGSN
869170031697544

OK
AT+CSCS?
+CSCS: "IRA"

OK
AT+CMGL=ALL
ERROR


*/


//ponemos su salto de linea al final de cada comando para que el modulo detecte el fin del comando
//los comandos tienen su salto de linea pero las respuestas no porque ellas no lo necesitan
//https://cdn-shop.adafruit.com/datasheets/SIM808_Hardware+Design_V1.00.pdf
//ficha tecnica

/*
Cuando el modulo esta apagado si tu le escribes el no responde ningun byte
este comando sirve para verificar la conecion y que el modulo este encendido
ya que solo responde AT ok, es decir no configura nada mas
8 BYTES
AT
OK

*/
#define AT_BASICO "AT"
#define ESPERADA_RESPUESTA_BASICO "OK"
#define BYTES_ESPERADOS_BASICO 8


//APAGAR EL MODULO NORMAL POWER DOWN
/*
Puede ser tambien enviando su pin D9 del modulo un pulso alto por 1 segundo.
Normal power down by sending the AT command “AT+CPOWD=1” or using the PWRKEY.
The  power  management  unit  shuts  down  the  power  supply  for  the  baseband
part  of  the  module,  and  only  the  power  supply  for  the  RTC  is  remained.
Software  is  not  active.  The  serial port is not accessible.
Power supply (connected to VBAT) remains applied.
nO HYA COMANDO PARA ENCENDERLO YA QUE AL HACER ESTO SE APAGA SU MODULO SERIE
para activarlo se tiene que presionar el boton o enviar un pulso alto al pin D9
31 bytes
T+CPOWD=1
NORMAL POWER DOWN

*/
#define AT_APAGA_MODULO_COMPLETO "AT+CPOWD=1"
#define ESPERADA_RESPUESTA_APAGA_MODULO_COMPLETO "NORMAL POWER DOWN"
#define BYTES_ESPERADOS_APAGA_MODULO_COMPLETO 31




//Comandos para saber datos del modulo, fabricacion nivel de bateria IMEI etc

/*
NOMBRE FABRICANTE 
AT+CGMI
SIMCOM_Ltd

OK

25BYTES
*/
#define AT_INFO_NOMBRE_FABRICANTE "AT+CGMI"
/*
iNFORMACION DEL PRODUCTO
ATI
SIM808 R14.18

OK

28 BYTES DE RESPUESTA
*/
#define AT_INFO_INFORMACION_PRUDCTO "ATI"
/*
QUE VELOCIDAD EN BAUDIOS SOPORTA SU COMUNICACION
AT+IPR=?
+IPR: (),(0,1200,2400,4800,9600,19200,38400,57600,115200,230400,460800)

OK

84 BYTES DE RESPUESTA
*/
#define AT_INFO_BAUD_RATE_SOPORTADO "AT+IPR=?"
/*
PARA PREGUNTAR EL BAUD RATE ACTUAL CONFIGURADO
POR DEFAULT NO TIENE UN CONFIGURADO SINO QUE AUTODETECTA LOS BAUDRATE
AT+IPR?
+IPR: 0

OK

25 BYTES
*/
#define AT_INFO_BAUD_RATE_ACTUAL "AT+IPR?"

/*
nOMBRE DE LA COMPAÑIA TELEFONICA
AT+COPS?
+COPS: 0,0,"334020"

OK

32BYTES

*/
#define AT_INFO_NOMBRE_COMPAÑIA_TELEFONICA "AT+COPS?"
/*
OBTENER EL IMEI DEL CHIP
AT+CGSN
AT+CGSN
869170031697544

OK

35 BYTES DE RSPESTA
*/
#define AT_INFO_IMEI "AT+CGSN"
/*
TIPO DE TEXTO NO TENGO IDEA QUE ES JAJA
AT+CSCS?
+CSCS: "IRA"

OK
*/
#define AT_INFO_TIPO_DE_TEXTO "AT+CSCS?"
/*
OBTENER LA INFORMACION DE LA BATERIA
27Bytes
AT+CBC
+CBC: 0,93,4147

OK

93%,4147mv
*/
#define AT_INFO_BATERIA "AT+CBC"
/*
Saber la informacion de la red a la que se conecto
AT+COPS?
+COPS: 0,0,"334020"

OK
32bytes

en este caso la red se llama 334020 quien sabe porque pero es telcel
en el internet en los ejemplos responden  +COPS: 0,0,"AT&T" CUANDO ES de esa compañia
*/
#define AT_INFO_NOMBRE_RED "AT+COPS?"
/*
sABER SI ESTA CONECTADO A UNA RED, SI LO ESTA RESPONDE
AT+CREG?
+CREG: 0,1

OK
23bytes
*/
#define AT_INFO_CONECTADO_A_RED "AT+CREG?"
/*
Revizar que se puedan hacer llamadas
AT+CCALR?
+CCALR: 1

OK

SI RESPONDE 1 ES QUE SE INICIALIZO BIEN EL MODULO GSM PARA REALIZAR Y RECIBIR LLAMADAS O MENSAJES
*/
#define AT_INFO_LLAMADAS_ACTIVAS "AT+CCALR?"
/*
El PIN del sim esta listo, algunos sim tienen el codigo pin de 4 digitos sin el no podras conectarte a la red
pero muchos sims no lo tienen entonces
si no hay7 que configurar el pin del sim para que se pueda conectar a una red
34 Bytes
AT+CPIN?
AT+CPIN?
+CPIN: READY

OK
*/
#define AT_INFO_PIN_STATUS "AT+CPIN?"	
/*
COnfigurar el pin del sim
remplazar las xxxx con el pin de 4 digitos

*/
#define AT_INFO_PIN_CONFIG "AT+CPIN=\"XXXX\""

/*
28 BYTES
AT+CSQ
AT+CSQ
+CSQ: 0,0

OK
En este caso el modulo nos devuelve el valor de la señan en ASU
EN UN EJEMPLO DICE SI ES 24, LA SEÑAL ES -65dbm
LOS RANGOS DE SEÑAL SON DE -53dbm(excelente) a -109dmb(marginal)
En las redes moviles 2G el RSSI expresado en dBm, siempre posee un valor negativo
La maxima potencia (en la misma torre) corresponde a -51dbm, y la minima con la que se puede establecer una conexion (la ubicacion mas alejada) corresponde a -113dbm
Y despues tenemos el ASU Arbitrary Stregth Unit, es otra unidad de medida expresada para la fortaleza de la señakl movil, a diferencia de los dBm su valor es proporcional
a la fureza de señal, en redes 2G oscila desde 0(laseñal mas debil) hasta 31(la señal mas fuerte)


Calcular entre dBm y asu
La relación entre asu y dBm es la siguiente:
dBm = 2 × ASU – 113
asu = 113 – dBm /2
lei que hay una tabla para relacionar los numeros que nos devuelve este comando con los dbm
pero me imagino que ha de tener una relacion por ejemplo 30 es -53dbm o excelente y con forme se aleja del 30 hacia abajo
trata de llegar a -109dbm osea mala. bueno no creo que se necesite en realidad buscar la tabla, si tomamos como que el 24 es -65dbm
todo por encima de 24 es mejor xD.


En ASU  Para redes 2G
17-31       Excelente, mas cerca de la torre -79 -> -51 dbm
12.5 - 17   Buena                           -88 -> -79 dbm
9 - 12.5    Regular                         -95 -> -88 dbm
4.5 - 9     Mala                            -104 -> -95 dbm
0 - 4.5     Debil                           -113 -> -104 dbm
0           Sin señal
https://norfipc.com/redes/intensidad-nivel-senal-redes-moviles-2g-3g-4g.php


*/
#define AT_INFO_RED_CALIDAD_DE_RED "AT+CSQ"	


//CONFIGURACION
/*
dEFINIR LOS BAUDIOS A LOS QUE QUEREMOS TRABAJAR, SI SE PONE 0 SERA AUTODETECTED
CADA QUE SE REINICIE EL MODULO, SE VOLVERA A LOS BAUDIOS CONFIGURADOS EN LA MEMORIA ATW
ESTO SERA GRAVE ENTONCES VAMOS A TOMAR TODOS LOS MODULOS LOS DE CHINA VIENEN CONFIGURADOS A 9600
LOS CAMBIAREMOS A 0 PARA QUE AUTODETECTEN Y LO GUARDAREMOS EN LA MEMORIA CON EL COMANDO ATW
AT+IPR=0
OK


32 BYTES
*/
#define AT_CONFIG_SET_BAUD_RATE___ATW "AT+IPR=XXXX" // AT+IPR=38400
/*
cONFIGURAR EL TIPO DE TEXTO, AUN NO TENGO IDEA QUE ES ESTO
AT+CSCS=”XXX”
*/
#define AT_CONFIG_TIPO_DE_TEXTO "AT+CSCS=”XXX”"
/*
Habilitar la carga de la bateria por defaul esta desactivada
17Bytes
AT+ECHARGE=1
OK

CADA QUE EL MODULO SE RESETEE SE REINICIAN LOS VALORES POR DEFAULT
*/
#define AT_CONFIG_BATERIA_HABILITAR_CARGA___ATW "AT+ECHARGE=1"
/*
DESABILITAR LA CARGA DE LA BATERIA
17Bytes
AT+ECHARGE=0
OK
*/
#define AT_CONFIG_BATERIA_DESABILITAR_CARGA___ATW "AT+ECHARGE=0"
/*
pARA PREGUNTAR SI LA CARGA DE BATERIA ESTA HABILITADA O DESABILITADA
27bytes
AT+ECHARGE?
+ECHARGE: 1

OK

AT+ECHARGE?
+ECHARGE: 0

OK
*/
#define AT_CONFIG_BATERIA_ESTADO_CARGA___ATW "AT+ECHARGE?"
/*
indicar una descripcion del error que se produce al enviar un comando AT
esta opcion esta desactivada por default asi, si hay algun error con un comando at
el modulo solo responde ERROR, si se activa esta opcion el modulo respondera con 
un CODIGO ESPEIFICO DEL ERROR
POR EJEMPLO AL QUERER ENVIAR UN SMS PERO SI ESTA CONFIGURADO COMO PDU Y NO COMO MODO TEXTO RESPONDE
AT+CMGS="+7121157686"
+CMS ERROR: 325

RESPUESTA DE CONFIGURACION
AT+CMEE=1
OK
*/
#define AT_CONFIG_REPORTAR_CODIGO_ERROR___ATW "AT+CMEE=1"
#define AT_CONFIG_DESACTIVAR_CODIGO_ERROR___ATW "AT+CMEE=0"
/*
ESTTUS DE LA CONFIGURACION DEL REPORTE DE ERRORES
*/
#define AT_CONFIG_STATUS_CODIGO_ERROR___ATW "AT+CMEE?"

//A LOS COMANDOS QUE LE PUSE AL FINAL ___ATW SIGNIFICA QUE PODEMOS GUARDAR ESAS CONFIGURACIONES CONE STE COMANDO
/*
GUARDAR CONFIGURACION DEL USUARIO EN MEMORIA NO VOLATIL DEL MODULO
UNA VES QUE ENVIAS ALGUNO DE ESTOS COMANDOS CONFIGURANDO EL MODULO
Y LUEGO ENVIAS EL COMANDO AT&W ESAS CONFIGURACIONES ANTERIORES SE
GUARDARAN EN LA MEMORIA NO VOLATIL DEL MODULO. DE ESA MANERA AUNQUE EL MODULO SE 
REINICIE NO SE TENDRA QUE VOLVER A CONFIGURAR TODO ESO
https://www.manualslib.com/manual/539989/Siemens-Tc63.html?page=481#manual AQUI VIENEN LOS COMANDOS QUE SE GUARDAN EN LA MEMORIA NO VOLATIL

LLAMADAS
AT+CIP IDENTIFICADOR DE LLAMADAS

MENSAJES
CMGF MODO DE MENSAJE PDU O TEXTO
CNMI NOTIFICACIONES DE MENSAJES NUEVOS

BATERIA
ECHARGE HABILITAR O NO HABILITAR EL CARGADOR DE BATERIA


*/
#define AT_CONFIG_GUARDAR_CONFIGURACIONES_EN_MEMORIA_NO_VOLATIL "AT&W"
#define AT_CONFIG_BORRAR_CONFIGURACIONES_DE_MEMORIA_NO_VOLATIL " AT&F"




//MINIMUM FUNCTIONALITY MODE ahorro de energia xD EL CONSUMO MEDIO ES DE 24.3ma medido con el osciloscopio, no se ven picos enormes de consumo causados por los modulos RF
/*
AT command “AT+CFUN” can be used to set the module to a minimum
functionality mode without removing the power supply. In this mode,
the RF part of the module will not work or the  SIM  card  will  not  be  accessible,
or  both  RF  part  and  SIM  card  will  be  closed,  and  the  serial  port  is  still  accessible.
The  power  consumption  in  this  mode  is  lower  than  normal  mode

29BYTES
AT+CFUN=0
+CPIN: NOT READY

ESO DE ARRIBA LO MANDA PRIMERO Y DESPUES DE UN RATITO MANDA UN OK PERO YA FUERA DE LA RESPUESTA PRIMERA
DESPUES QUE YA ESTA ACTIVO EL AHORRO DE ENERGIA Y VUELVES A ENVIARLE EL COMANDO PARA VERIFICAR AHORA TE RESPONDE CON UN OK
//15BYTES
AT+CFUN=0
OK


*/
#define AT_GENERAL_AHORRO_ACTIVAR___ATW "AT+CFUN=0"                         //PARA QUE ENTRE EN AHORRO DE ENERGIA, PRIMERO NOS ENVIA LA RESPUESTA DE NOT READY ES DECIR ESTA TRABAJANDO EN ELLO
#define ESPERADA_RESPUESTA_AHORRO_ACTIVAR "+CPIN: NOT READY"
#define ESPERADA_RESPUESTA_AHORRO_COMPROBAR "OK"

//este es algo complicado porque envia solo 9 bytes primero pero despues envia el otro pedacito que le hace falta
#define AT_GENERAL_AHORRO_DESACTIVAR___ATW "AT+CFUN=1"                     //PARA QUE SALGA DEL AHORRO DE ENERGIA Y TRABAJE NORMALMENTE
#define ESPERADA_RESPUESTA_AHORRO_DESACTIVAR "+CPIN: READY"         //NO USAR ESTA RESPUESTA PARA COMPROBAR YA QUE AL PARECER LA MANDO EN 2 PARTES POR SEPARADO
#define ESPERADA_RESPUESTA_AHORRO_COMPROBAR "OK"
/*9BYTES
AT+CFUN=

------pedacito que envia despues y despu
+CPIN: READY

-------- entonces ya no se solo lo vereficare con el comando despues de que se activo que nos diga ok
ESO DE ARRIBA LO MANDA PRIMERO Y DESPUES DE UN RATITO MANDA UN OK PERO YA FUERA DE LA RESPUESTA PRIMERA
DESPUES QUE YA ESTA ACTIVO EL AHORRO DE ENERGIA Y VUELVES A ENVIARLE EL COMANDO PARA VERIFICAR AHORA TE RESPONDE CON UN OK
//15BYTES
AT+CFUN=1
OK

*/









		//MODULO GPS
		/*
			18 BYTES
				AT+CGPSPWR=1
				OK

				----------
				para desactivar son igual 18 bytes
				AT+CGPSPWR=0
				OK
		*/
		#define AT_GPS_ACTIVA "AT+CGPSPWR=1"
		#define AT_GPS_APAGA "AT+CGPSPWR=0"
		#define ESPERADA_RESPUESTA_GPS_ACTIVA "OK"

/*
SE PUEDE REINICIAR EL MODULO GPS POR SEPARADO SI ES EN FRIO TARDARA MAS TIEMPO EN ENCONTRAR LA SEÑAL GPS
PERO SE REINICIARA COMPLETAMENTE
AT+CGPSRST=0
OK
*/
#define AT_GPS_REINICIAR_EN_FRIO "AT+CGPSRST=0"
/*
SE PUEDE REINICIAR EL MODULO GPS POR SEPARADO SI ES EN CALIENTE ENCONTRARA LA SEÑAL CASI INMEDIATAMENTE
QUISAS EL REINICIO NO SEA TAN PROFUNDO
AT+CGPSRST=1
OK
*/
#define AT_GPS_REINICIAR_EN_CALIENTE "AT+CGPSRST=1"

		/*61 bytes segun puerto serie
				AT+CGPSSTATUS?
				AT+CGPSSTATUS?
				+CGPSSTATUS: Location 3D Fix

				OK

		*/
		#define AT_GPS_STATUS "AT+CGPSSTATUS?"
		#define ESPERADA_RESPUESTA_GPS_STATUS_3D "Location 3D Fix"			//cuando ya capturo al 100% la señal gps
		/*61
				AT+CGPSSTATUS?
				AT+CGPSSTATUS?
				+CGPSSTATUS: Location 2D Fix

				OK

				*/
		#define ESPERADA_RESPUESTA_GPS_STATUS_2D "Location 2D Fix"			//cuando capturo solo un procentaje de la señal gps
		/*
				AT+CGPSSTATUS?
				AT+CGPSSTATUS?
				+CGPSSTATUS: Location Unknown

				OK

				*/
		#define ESPERADA_RESPUESTA_GPS_STATUS_APAGADO "Location Unknown"	//cuando el modulo gps esta apagado y preguntas su status
		/* 53 BYTES
				AT+CGPSSTATUS?
				AT+CGPSSTATUS?
				+CGPSSTATUS: Location Not Fix

				OK

				 */
		#define ESPERADA_RESPUESTA_GPS_STATUS_ENCENDIDO_SINLOCALIZACION "Location Not Fix"	//cuando el modulo gps esta encendido y preguntas su status pero no tiene localizacion aun
		

		#define GPS_APAGADO 0
		#define GPS_ENCENDIDO_SIN_SENAL 1
		#define GPS_ENCENDIDO_SENAL_MEDIA 2
		#define GPS_ENCENDIDO_SENAL_PERFECTA 3
		#define GPS_ERROR_COMANDO 255





		//si tratas de solicitar estos comandos al modulo con el gps apagado reportara error
		 //Manejo de gps
		//https://cdn-shop.adafruit.com/datasheets/SIM808_GPS_Application_Note_V1.00.pdf
		/*
		este devuelve las coordenadas en grados minutos y segundos pero esta rara su forma
		AT+CGPSINF=0
		+CGPSINF: 0,1947.718300,9952.733400,2546.200000,20200110005119.000,0,11,2.500200,278.420013

		OK
		para convertir estas coordenadas a decimal, hay que ser precabidos segun aqui estan separadas en
		19° 47' 71.83" y para convertir esto a decimales segun internet es (19° 47' 71.83'' N) = (19 + (47 / 60) + (71 / 3600)) = 19.80272
		99° 52' 73.34"														(99° 52' 73.34'' W) = (99 + (52 / 60) + (73 / 3600)) = -99.88627
		Si hacemos esa conversion asi e ingresamos los datos a google hearto la ubicacion esta incorrecta nos manda a la orilla del libramiento!!
		entonces para hacer la conversion correctamente solo hay que separar los grados y el numero restante dividirlo entre 60
		(19 + (47.718300/60)) = 19.795305
		(99 + (52.733400/60)) = 99.87889

		//lo unico malo es que no veo que este metodo nos entrege N o W para saber si la conversion debe ser + o -																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																				

		y ahora si nos salen las coordenadas correctas 
		
						Modo: 0.
						Latitud: 19° 47.7183.				AQUI LA DOCUNMENTAICON DECIA 19° 47' 71.83"
						Longitud: 99° 52.7334' .                   99° 52' 73.34"
						Altitud 2575.700
						Fecha en UTC: 2020-01-10-00:51:19.000,
						Tiempo de respuesta: 0.
						Número de satélites: 10.
						Velocidad: 1.777920 en nudos.
						Curso: 34.000000.
		*/
		#define AT_GPS_COORDENADAS_0_LAT_LONG_GRADOS_FECHA_HORA "AT+CGPSINF=0"

		/*
				+CGPSINF: 2,005616.000,0019.7952,N,-0099.8787,W,1,7,1.15,2543.1,M,-9.0,M,,

				Name 		Example Data 				Description

				Sentence	Identifier 				$GPGGA 	Global Positioning System Fix Data
				Time 		005616 						00:56:16 Z
				Latitude 	0019.7952, N			41d 24.8963' N or 41d 24' 54" N
				Longitude 	-0099.8787, W			81d 51.6838' W or 81d 51' 41" W

				Fix Quality:
				- 0 = Invalid
				- 1 = GPS fix
				- 2 = DGPS fix		1 						Data is from a GPS fix
				Number of Satellites 	7 					7 Satellites are in view
				Horizontal Dilution of Precision (HDOP) 	1.5 	Relative accuracy of horizontal position
				Altitude 			2543.1, M 				2543.1 meters above mean sea level
				Height of geoid above WGS84 ellipsoid 	-9.0, M 	-9.0 meters
				Time since last DGPS update 	blank 	No last update
				DGPS reference station id 	blank 	No station id
				Checksum 	*75 	Used by program to check for transmission errors
		*/		
		#define AT_GPS_COORDENADAS_2_GPGGA_HORA_LAT_LONG_DECIMAL  "AT+CGPSINF=2"
		//+CGPSINF: 4,1947.718100,N,9952.730700,W,005757.000,A,A
		#define AT_GPS_COORDENADAS_4_GLL_LAT_LONG_GRADOS_TIME "AT+CGPSINF=4"
		//+CGPSINF: 8,A,3,23,07,30,11,19,28,13,17,,,,,1.29,0.98,0.84
		#define AT_GPS_COORDENADAS_8_GPGSA_SATELITES_ACTIVOS "AT+CGPSINF=8"
		/*
		$GPGSV
		AT+CGPSINF=16
		+CGPSINF: 16,3,1,11,07,67,052,18,30,52,343,29,11,48,073,16,17,43,221,31
		+CGPSINF: 16,3,2,11,28,38,308,27,01,34,103,16,09,30,179,,19,19,217,28
		+CGPSINF: 16,3,3,11,08,16,037,,23,10,163,18,13,10,319,23
		SATELLITES EN VISTA
		*/
		#define AT_GPS_COORDENADAS_16_GPGSV_SATELITES_EN_VISTA "AT+CGPSINF=16"
		/*
		$GPRMC
		+CGPSINF: 32,010109.000,A,1947.7163,N,09952.7292,W,0.482,278.95,100120,,,A

		mODO ES EL 32
			Horario UTC: 01:01:09.
			Estado: A=Activo, V=Vacío.
			Latitud: 19° 47' 71.63" N.
			Longitud: 099° 52' 72.92" W.
			Velocidad en nudos: 0.482.
			Ángulo de derrota en grados: 278.95.
			Fecha: 10/01/20
			Variación magnética.
			Datos del Cheksum.
		*/
		#define AT_GPS_COORDENADAS_32_GPRMC_TIME_LAT_LONG_GRADOS_VEL_FECHA "AT+CGPSINF=32"
		/*
		request GPVTG string
		AT+CGPSINF=64
		+CGPSINF: 64,185.65,T,,M,1.547,N,2.867,K,A

		OK



		$GPVTG

		Track Made Good and Ground Speed.
		*/
		#define AT_GPS_SOLICITUD_64_GPVTG "AT+CGPSINF=64"
		/*
		$GPZDA
		AT+CGPSINF=128
		+CGPSINF: 128,023648.000,10,01,2020,00,00

		OK
		Date & Time

		UTC, day, month, year, and local time zone.

		ZDA,hhmmss.ss,xx,xx,xxxx,xx,xx
		*/
		#define AT_GPS_COORDENADAS_128_GPZDA_HORA_FECHA "AT+CGPSINF=128"



		/*
		//ya vi el problema con la hora es porque es UTC hora universal coordinada https://www.worldtimeserver.com/hora-exacta-UTC.aspx LA HORA UNIVERSAL DE AHI HAY QUE OBTENER EN TU ZONA HORARIA
		//lo unico que hay que hacer es, por ejemplo cuando tu defines la zona horaria para mexico pones, -6 o -5 para invierno o verano
		//en este caso indica las horas que tienes que restar al UTC para obtener tu hora local, por ejemplo ahorita me marca en horario UTC
		//2:16:21AM 10-enero-2020
		//si le quitamos 6 horas tendriamos 20:16:21 09-enero-2020
		//EL PROBLEMA ES LO MISMO LA HORA ESTA MAL Y LAS COORDENADAS CONVERTIDAS A DECIMAL NOS ARROJA
		//19.8032861,-99.88703888888888 NOS MANDA IGUAL ALADO DEL LIBRAMIENTO Y SI NOS DAMOS CUENTA AQUI NO DICE SI ES N W
		AT+CGPSINF=0
		+CGPSINF: 0,1947.718300,9952.733400,2546.200000,20200110005119.000,0,11,2.500200,278.420013

		OK

		// request Basic string
		Modo: 0.
				Latitud: 19° 47' 71.83".
				Longitud: 99° 52' 73.34".
				Altitud 2575.700
				Fecha en UTC: 2020-01-10-00:51:19.000,
				Tiempo de respuesta: 0.
				Número de satélites: 10.
				Velocidad: 1.777920 en nudos.
				Curso: 34.000000.




		AT+CGPSINF=1
		ERROR




		//GPGGA  	Global Positioning System Fix Data  http://aprs.gids.nl/nmea/#gga
		//ESTE METODO NOS RETORNA LAS COORDENADAS CONVERTIDAS YA EN DECIMALES SI LAS INGRESO ASI AL GOOGLE MAPS
		//YA ME DA LA UBICACION CORRECTA DE LA CASA!!! PERO NO SE QUE SIGNIFICAN LOS DEMAS DATOS, NO VEO LA FECHA O LA ROTACION
		//// request GGA string --eso dice en este link https://gist.github.com/olivermontes/5900439f95ac8423ef22
		AT+CGPSINF=2
		+CGPSINF: 2,005616.000,0019.7952,N,-0099.8787,W,1,7,1.15,2543.1,M,-9.0,M,,

		OK

		Name 		Example Data 				Description
		Sentence	Identifier 				$GPGGA 	Global Positioning System Fix Data
		Time 		170834 						17:08:34 Z
		Latitude 	4124.8963, N			41d 24.8963' N or 41d 24' 54" N
		Longitude 	08151.6838, W			81d 51.6838' W or 81d 51' 41" W

		Fix Quality:
		- 0 = Invalid
		- 1 = GPS fix
		- 2 = DGPS fix		1 						Data is from a GPS fix

		Number of Satellites 	05 					5 Satellites are in view
		Horizontal Dilution of Precision (HDOP) 	1.5 	Relative accuracy of horizontal position
		Altitude 			280.2, M 				280.2 meters above mean sea level
		Height of geoid above WGS84 ellipsoid 	-34.0, M 	-34.0 meters
		Time since last DGPS update 	blank 	No last update
		DGPS reference station id 	blank 	No station id
		Checksum 	*75 	Used by program to check for transmission errors





		AT+CGPSINF=3
		ERROR





		// request GLL string
		AT+CGPSINF=4
		+CGPSINF: 4,1947.718100,N,9952.730700,W,005757.000,A,A

		OK

		Geographic Position, Latitude / Longitude and time.

		eg1. $GPGLL,3751.65,S,14507.36,E*77
		eg2. $GPGLL,4916.45,N,12311.12,W,225444,A


				   4916.46,N    Latitude 49 deg. 16.45 min. North
				   12311.12,W   Longitude 123 deg. 11.12 min. West
				   225444       Fix taken at 22:54:44 UTC
				   A            Data valid


		eg3. $GPGLL,5133.81,N,00042.25,W*75
					   1    2     3    4 5

			  1    5133.81   Current latitude
			  2    N         North/South
			  3    00042.25  Current longitude
			  4    W         East/West
			  5    *75       checksum









		AT+CGPSINF=5
		ERROR



		AT+CGPSINF=6
		ERROR





		AT+CGPSINF=7
		ERROR




		“GPGSA”
		AT+CGPSINF=8
		+CGPSINF: 8,A,3,23,07,30,11,19,28,13,17,,,,,1.29,0.98,0.84

		OK

		GPS DOP and active satellites

		eg1. $GPGSA,A,3,,,,,,16,18,,22,24,,,3.6,2.1,2.2*3C
		eg2. $GPGSA,A,3,19,28,14,18,27,22,31,39,,,,,1.7,1.0,1.3*35


		1    = Mode:
			   M=Manual, forced to operate in 2D or 3D
			   A=Automatic, 3D/2D
		2    = Mode:
			   1=Fix not available
			   2=2D
			   3=3D
		3-14 = IDs of SVs used in position fix (null for unused fields)
		15   = PDOP
		16   = HDOP
		17   = VDOP






		AT+CGPSINF=9
		ERROR

		AT+CGPSINF=9
		ERROR
		AT+CGPSINF=10
		ERROR
		AT+CGPSINF=11
		ERROR
		AT+CGPSINF=12
		ERROR
		AT+CGPSINF=13
		ERROR
		AT+CGPSINF=14
		ERROR
		AT+CGPSINF=15
		ERROR


		$GPGSV
		AT+CGPSINF=16
		+CGPSINF: 16,3,1,11,07,67,052,18,30,52,343,29,11,48,073,16,17,43,221,31
		+CGPSINF: 16,3,2,11,28,38,308,27,01,34,103,16,09,30,179,,19,19,217,28
		+CGPSINF: 16,3,3,11,08,16,037,,23,10,163,18,13,10,319,23

		OK

		$GPGSV

		GPS Satellites in view

		eg. $GPGSV,3,1,11,03,03,111,00,04,15,270,00,06,01,010,00,13,06,292,00*74
			$GPGSV,3,2,11,14,25,170,00,16,57,208,39,18,67,296,40,19,40,246,00*74
			$GPGSV,3,3,11,22,42,067,42,24,14,311,43,27,05,244,00,,,,*4D


			$GPGSV,1,1,13,02,02,213,,03,-3,000,,11,00,121,,14,13,172,05*67


		1    = Total number of messages of this type in this cycle
		2    = Message number
		3    = Total number of SVs in view
		4    = SV PRN number
		5    = Elevation in degrees, 90 maximum
		6    = Azimuth, degrees from true north, 000 to 359
		7    = SNR, 00-99 dB (null when not tracking)
		8-11 = Information about second SV, same as field 4-7
		12-15= Information about third SV, same as field 4-7
		16-19= Information about fourth SV, same as field 4-7







		AT+CGPSINF=17
		ERROR
		AT+CGPSINF=18
		ERROR
		AT+CGPSINF=19
		ERROR
		AT+CGPSINF=20
		ERROR
		AT+CGPSINF=2
		ERROR
		AT+CGPSINF=21
		ERROR
		AT+CGPSINF=22
		ERROR
		AT+CGPSINF=23
		ERROR
		AT+CGPINF=24
		ERROR
		AT+CGPSINF=25
		ERROR
		AT+CGPSINF=26
		ERROR
		AT+CGPSINF=27
		ERROR
		AT+CGPSINF=28
		ERROR
		AT+CGPSI�=29
		ERROR
		AT+CGPSINF=30
		ERROR
		AT+CGPINF=31
		ERROR


		// request GPRMC string
		//LO RARO DE ESTA INFO ES LA FECHA, ESTA ADELANTADA POR HORAS AQUI AUN ES 09-01-2020 19:00 es porque me da la hora UTC HAY QUE RESTARLE 6 HORAS PARA QUE NOS DE l ahora local
		//LAS COORDENADAS CONVERTIDAS A DECIMALES DAN 19.8030556  -99.88666666666666
		//NOS MANDA ALADO DEL LIBRAMIENTO DE ATLACOMULCO
		AT+CGPSINF=32
		+CGPSINF: 32,010109.000,A,1947.7163,N,09952.7292,W,0.482,278.95,100120,,,A

		OK

			mODO ES EL 32
			Horario UTC: 01:01:09.
			Estado: A=Activo, V=Vacío.
			Latitud: 19° 47' 71.63" N.
			Longitud: 099° 52' 72.92" W.
			Velocidad en nudos: 0.482.
			Ángulo de derrota en grados: 278.95.
			Fecha: 10/01/20
			Variación magnética.
			Datos del Cheksum.


			$GPRMC

		Recommended minimum specific GPS/Transit data

		eg1. $GPRMC,081836,A,3751.65,S,14507.36,E,000.0,360.0,130998,011.3,E*62
		eg2. $GPRMC,225446,A,4916.45,N,12311.12,W,000.5,054.7,191194,020.3,E*68


				   225446       Time of fix 22:54:46 UTC
				   A            Navigation receiver warning A = OK, V = warning
				   4916.45,N    Latitude 49 deg. 16.45 min North
				   12311.12,W   Longitude 123 deg. 11.12 min West
				   000.5        Speed over ground, Knots
				   054.7        Course Made Good, True
				   191194       Date of fix  19 November 1994
				   020.3,E      Magnetic variation 20.3 deg East
				   *68          mandatory checksum






		AT+CGPSINF=33
		ERROR
		AT+CGPSINF=34
		ERROR
		AT+CGPSINF=35
		ERROR
		AT+CGPSINF=36
		ERROR
		AT+CGPSINF=37
		ERROR
		AT+CGPSINF=38
		ERROR
		AT+CGPSINF=39
		ERROR
		AT+CGPSINF=40
		ERROR
		AT+CGPSINF=41
		ERROR
		AT+CGPSIN=42
		ERROR
		AT+CGPSINF=43
		ERROR
		AT+CGPSINF=44
		ERROR
		AT+CGPSINF=45
		ERROR
		AT+CGPSINF=46
		ERROR
		AT+CGPSINF=47
		ERROR
		AT+CGPSINF=48
		ERROR
		AT+CGPSINF=49
		ERROR
		AT+CGPSINF=50
		ERROR
		AT+CGPSINF=52
		ERRO

		// request GPVTG string
		AT+CGPSINF=64
		+CGPSINF: 64,185.65,T,,M,1.547,N,2.867,K,A

		OK



		$GPVTG

		Track Made Good and Ground Speed.

		eg1. $GPVTG,360.0,T,348.7,M,000.0,N,000.0,K*43
		eg2. $GPVTG,054.7,T,034.4,M,005.5,N,010.2,K


				   054.7,T      True track made good
				   034.4,M      Magnetic track made good
				   005.5,N      Ground speed, knots
				   010.2,K      Ground speed, Kilometers per hour


		eg3. $GPVTG,t,T,,,s.ss,N,s.ss,K*hh
		1    = Track made good
		2    = Fixed text 'T' indicates that track made good is relative to true north
		3    = not used
		4    = not used
		5    = Speed over ground in knots
		6    = Fixed text 'N' indicates that speed over ground in in knots
		7    = Speed over ground in kilometers/hour
		8    = Fixed text 'K' indicates that speed over ground is in kilometers/hour
		9    = Checksum













		// request GPZDA string   $GPZDA - Date & Time
		T+CGPSINF=128
		+CGPSINF: 128,023648.000,10,01,2020,00,00

		$GPZDA

		Date & Time

		UTC, day, month, year, and local time zone.

		$--ZDA,hhmmss.ss,xx,xx,xxxx,xx,xx
		hhmmss.ss = UTC
		xx = Day, 01 to 31
		xx = Month, 01 to 12
		xxxx = Year
		xx = Local zone description, 00 to +/- 13 hours
		xx = Local zone minutes description (same sign as hours)






		OK



		Serial.println("*************************************************");
			Serial.print("Basic string: ");
			Serial.println(Basic_str);
			Serial.print("GGA string: ");
			Serial.println(GGA_str);
			Serial.print("GLL string: ");
			Serial.println(GLL_str);
			Serial.print("RMC string: ");
			Serial.println(RMC_str);
			Serial.print("VTG string: ");
			Serial.println(VTG_str);
			Serial.print("ZDA string: ");
			Serial.println(ZDA_str);


			//https://at-sky.com/gsmgprs-gps/28-ung-dung-gps-tren-sim908.html este link expilca
			AT+CGPSINF=?



			 Feedback:  +CGPSINF:(0,2,4,8,16,32,64,128)

			 //https://at-sky.com/gsmgprs-gps/28-ung-dung-gps-tren-sim908.html
			 //https://cdn-shop.adafruit.com/datasheets/SIM808_GPS_Application_Note_V1.00.pdf
			 If [mode] is:
			2^1: Parameters see appendix A.1 “$GPGGA” [1]    2
			2^2: Parameters see appendix A.2 “GPGLL” [1]    4
			2^3: Parameters see appendix A.3 “GPGSA” [1]    8
			2^4: Parameters see appendix A.4 “GPGSV” [1][2] 16
			2^5: Parameters see appendix A.5 “GPRMC” [1]   32
			2^6: Parameters see appendix A.6 “GPVTG” [1]   64
			2^7: Parameters see appendix A.7 “GPZDA” [1]   128


			http://aprs.gids.nl/nmea/
			 19 Interpreted sentences

		   $GPBOD - Bearing, origin to destination
		   $GPBWC - Bearing and distance to waypoint, great circle
		   $GPGGA - Global Positioning System Fix Data
		   $GPGLL - Geographic position, latitude / longitude
		   $GPGSA - GPS DOP and active satellites
		   $GPGSV - GPS Satellites in view
		   $GPHDT - Heading, True
		   $GPR00 - List of waypoints in currently active route
		   $GPRMA - Recommended minimum specific Loran-C data
		   $GPRMB - Recommended minimum navigation info
		   $GPRMC - Recommended minimum specific GPS/Transit data
		   $GPRTE - Routes
		   $GPTRF - Transit Fix Data
		   $GPSTN - Multiple Data ID
		   $GPVBW - Dual Ground / Water Speed
		   $GPVTG - Track made good and ground speed
		   $GPWPL - Waypoint location
		   $GPXTE - Cross-track error, Measured
		   $GPZDA - Date & Time

		*/





























#define RESPUESTA_CORRECTA 0
#define RESPUESTA_INCORRECTA 1
#define RESPUESTA_NO_HAY_BYTES_DISPONIBLES 2
#define RESPUESTA_NUMERO_DE_BYTES_RECIBIDOS_NO_CONCUERDA 3










//SMS
//
//Referencias

//https://www.developershome.com/sms/operatingMode2.asp

//https://www.logicbus.com.mx/pdf/31/Comandos_AT_generales.pdf			//creo este esta muy completo de aqui saco el 90% de los sms
//https://doc.qt.io/archives/qtopia4.3/atcommands.html					//AQUI VIENEN ALGUNOS QUE PODRIAN SERVIR COMO DEFINIR EL ALMACENAMIENTO PREDETERMINADO


/*
			cuando el modulo reciba un mensaje enviara al serie
			+CMTI: "SM",11

			que es basicamente los mensajes disponibles, el modulo guarda los mensajes en la memoria de la sim
			y luego con otro comando tienes que consultarlo, a menos que actives la configuracion que hace enviar al modulo
			el contendido del mensaje en cuanto lo recibe
*/


/*
	EN LAS 10 X PON EL NUMEROM SI EL NUMERO ES DE MENOS DE 10 DIGITOS NO IMPORTA

	sustituir YY por el prefijo internacional en el caso de mexico +52 y las 10 x por el numero
	el modulo respondera si NO HAS ACTIVADO EL MODO TEXTO CON ERROR 
	si ya esta esta activo el modo texto responde ">"
	indicando que todo lo que le envies desde ahora sera el contenido del mensaje, si envias comandos at el modulo no hara nada mas que ingresarlos al contenido del mensaje
	si quieres cancelar envia el comando ESC

	AT+CMGS="+527121157686"
	>
	

	y para finalizar el envio tienes que usar el caracter ASCII 26
	en el caso de codigo se pone asi sim808.write(char(26)); 
	pero si lo haces en puerto serie es \u001A o \x1A

	AT+CMGS="+527121157686"   //envie este comando
	>								//el gps me respondio esto
	adffadasfdfadfafdsfadsfafs			//le escribi el mensaje que quice en el peurto serie se lo envias al modulo
										//se envia por codigo el caracter 26
	+CMGS: 23							//el gps responde que lo envio y en que posicion de la memoria se guardo el mensaje enviado

	OK

*/
#define AT_SMS_ENVIAR_SMS "AT+CMGS=\"+YYXXXXXXXXXX\"" 
#define AT_SMS_FINALIZAR_ENVIO_SMS (char(26)) 
#define AT_SMS_FINALIZAR_ENVIO_SMS_1 "\u001A"     //SE SUPONE QUE LA \u es para escapar cuaquier caracter unicode poniendolo asi me funciono en el puerto serie https://stackoverflow.com/questions/3042743/how-to-send-ctrlz
#define AT_SMS_FINALIZAR_ENVIO_SMS_2 "\x1A"     //SE SUPONE QUE LA este es similar sirve tambien para las cerrar el envio de informacion al TCP O GPRS http://fuenteabierta.teubi.co/2013/11/arduino-y-peticiones-http-con-el-sim900.html
#define AT_SMS_CANCELAR_ENVIO_SMS (char(27)) 
#define AT_SMS_CANCELAR_ENVIO_SMS_1 "\u001B"     //SE SUPONE QUE LA \u es para ESCRIBIR el numero decimal al que corresponde el caracter pero en poniendolo en formato exadecimal 
#define AT_SMS_CANCELAR_ENVIO_SMS_2 "\x1b"     //SE SUPONE QUE LA este es similar sirve tambien para las cerrar el envio de informacion al TCP O GPRS http://fuenteabierta.teubi.co/2013/11/arduino-y-peticiones-http-con-el-sim900.html
/*
Por ejemplo en esta tabla de ascii viene que el caracter 26 es el SUB que el modulo reconoce indicando que ya terminaste de enviar la informacion del cuerpo del mensaje, en decimal es 26 pero en hexadecimal es 1A
y para cancelar el cuerpo del mensaje y no enviar el mensaje es el caracter 27 ESC en exadecimal seria 1b
*/

//Configurar envio y recibir sms en modo texto, CADA QUE SE APAGE Y SE ENCIENDA HAY QUE CONFIGURAR EL SMS EN MODO TEXTO PARA PODER ENVIAR SMS ME PARECE QUE HAY UN COMANDO PARA GUARDAR LAS OPCIONES DEL USUARIO

/*
Que modos de envio sms soporta? 
AT+CMGF=?
+CMGF: (0,1)

OK

29 bytes de respuesta
The values 0 and 1 refer to SMS PDU mode and SMS text mode respectively. SMS PDU mode is the default mode if it is implemented on the mobile device.
*/
#define AT_SMS_INFO_MODOS_SMS_SOPORTADOS "AT+CMGF=?"		
/*
Cambiar el modo de sms a PDU
AT+CMGF=0
OK

20 BYTES
*/
#define AT_SMS_CONFIG_CAMBIAR_MODO_PDU___ATW "AT+CMGF=0"			
/*
Cambiar el modo de sms a TEXTO, hay que enviar este comando cada que se reinicie el modulo para que se puedan enviar en modo texto ya que por default es PDU
AT+CMGF=1
OK

20 BYTES
*/
#define AT_SMS_CONFIG_CAMBIAR_MODO_TEXTO___ATW "AT+CMGF=1"		

/*
https://infocenter.nordicsemi.com/index.jsp?topic=%2Fref_at_commands%2FREF%2Fat_commands%2Ftext_mode%2Fcnmi_read.html
 Cuando el modulo reciba un SMS enviara el contenido del mensaje en automatico por el puerto serie
 AT+CNMI=2,2,0,0,0
 OK
 21bytes

 Con esta configuracion cuando se reciva un nuevo sms mostrara el contenido en el puerto serie
 +CMT: "7121157686","","20/01/13,12:23:09-24"
holaaaa prueba de la configuracion

cuando entre un nuevo sms no podremos saber cuantos bytes fijos seran ya que el contenido del mensaje sera variable

*/
#define AT_SMS_CONFIG_NUEVO_MENSAJE_AVISO_MOSTRAR_CONTENIDO___ATW "AT+CNMI=2,2,0,0,0"	
/*
Con esta configuracion cuando entra un sms, manda eso por el puerto serie e indica donde se guardo, en este caso en el sim y en la posicion 7
+CMTI: "SM",7

16bytes, asi sera la respuesta aprox cuando entre un nuevo sms
*/
#define AT_SMS_CONFIG_NUEVO_MENSAJE_AVISO_BASICO___ATW "AT+CNMI=2,1,0,0,0"
/*
nO RECIBIR NOTIFICACIONES
*/
#define AT_SMS_CONFIG_NUEVO_MENSAJE_NO_NOTIFICAR___ATW "AT+CNMI=2,0,0,0,0"
/*
pREGUNTAR CONFIGURACION ACTUAL
*/
#define AT_SMS_INFO_NUEVO_MENSAJE_AVISO "AT+CNMI?"
/*
QUE MODOS DE CONFIGURACION SOPORTA PARA LOS AVISOS DE SMS
AT+CNMI=?
+CNMI: (0-3),(0-3),(0,2),(0,1),(0,1)

OK

para que sepas que parametros puedes mover y enviar
*/
#define AT_SMS_INFO_NUEVO_MENSAJE_AVISO_CONFIG_SOPORTADA "AT+CNMI=?"


/*
LEER MENSAJE EN LA POSICION DE MEMORIA EN MODO TEXTO, AT+CMGF=1 UNA VES CONFIGURADO SE RECIBEN ASI LOS DEMAS MESNAJES
reemplazar la x por el mensaje que se quiere leer
AT+CMGR=1

AT+CMGR=1
+CMGR: "REC UNREAD","UNOTV.COM","","19/12/30,22:27:56-24"
SI QUIERES TRABAJAR EN OTRO PAIS,CANADA TE OFRECE CHAMBA/VD:OMNAIVRACUADSRZSEE/UTNBEE RUMAITDSSSATJSuovm/HK

//SI VUELVES A LEER UN MENSAJE SE COLOCA COMO REC READ
AT+CMGR=1
+CMGR: "REC READ","UNOTV.COM","","19/12/30,22:27:56-24"
SI QUIERES TRABAJAR EN OTRO PAIS,CANADA TE OFRECE CHAMBA/VIE:OET NENLCUAESOO NE/UINIBRPEUEAITO U AUJSut.x1v

OK

EL MISMO MENSAJE EN PDU MODE
AT+CMGF=0		//PRIMERO LO CONFIGURAMOS EN PDU
AT+CMGR=1		//LUEGO LEEMOS EL MENSAJE

AT+CMGR=1
+CMGR: 1,"",161
0891254901001014F00410D055E7936A750D9F4D0009121032272654AA0D324285A4D16A5C529882A0D0A83CAA0145474895E1002788525A4E70254B9D23E009411A2AB3A30492E9F98A598A463A257BE751F81D

OK

sI EL INDEX ES INCORRECTO
AT+CMGR=50
OK
*/
#define AT_SMS_LEER_MENSAJE_EN_POSICION_MEMORIA "AT+CMGR=X"
/*
LOS MENSAJES SE ALMACENARAN EN LA MEMORIA CON ESTA CLASIFICACION

Text mode possible values	 PDU mode possible values	 Status of messages in memory 
“REC UNREAD”							0					received unread messages 
“REC READ”								1					received read messages 
“STO UNSENT”							2					stored unsent messages
“STO SENT”								3					stored sent messages 
“ALL”                                    4                                    all    messages    

AT+CMGL=?
+CMGL: ("REC UNREAD","REC READ","STO UNSENT","STO SENT","ALL")

OK
*/
#define AT_SMS_INFO_CLASIFICACION "AT+CMGR=X"

//CUIDADO CON TODOS LOS BYTES RETORNADOS DE ESTOS METODOS YA QUE RETORNARAN MUCHOS BYTES PORQUE DEVUELVE TODOS TODOS LOS MENSAJES DE GOLPE

/*
LEER LOS MENSAJES UNREAD lee todos los mensajes que estan sin leer, una vez leidos todos se pasaran a READ

AT+CMGL="REC UREAD"
+CMGL: 4,"REC UNREAD","UNOTV.COM","","20/01/01,16:11:38-24"
INICIA INSABI; ASI PUEDES TENER CONSULTA MEDICA GRATIS/SMRS:IAD 5AISDNCRZNPJMSYEO,AIRCBENAI UV AOOunvm/I

+CMGL: 5,"REC UNREAD","UNOTV.COM","","20/01/01,22:19:49-24"
GUARDIA NACIONAL LLEGARA A 50 NUEVAS REGIONES EN 2020/POUO U UIA ERCOET NOCMETSPOOIO  NONEOCNET TSuovm/I

+CMGL: 6,"REC UNREAD","UNOTV.COM","","20/01/02,15:55:36-24"
IMAGENES: FUERTES VENTARRNES DEJAN ESTOS DESTROZOS EN AAH/ATNERMEE AIND UIEOVDO NAT UR N
+CMGL: 7,"REC UNREAD","UNOTV.COM","","20/01/02,21:54:21-24"
VIDEO: VIENTOS TIRAN ARBOLES Y ESPECTACULARES EN CDMX YETSEO/S UOEBJD EE  RKTA TQET OPEDR UA NPN OGuo.m16

+CMGL: 8,"REC UNREAD","Telcel","","20/01/03,10:17:53-24"
Recarga de $10.00 de Saldo Amigo. Folio 341465 .Tus tarifa  XEACnsnIAncSg$.15 S .5 B$.5Maif 24omtlecm

+CMGL: 9,"REC UNREAD","UNOTV.COM","","20/01/03,17:30:24-24"
APROVECH: HABRA DESCUENTOS EN PAGO DE ESTE IMPUESTO/IRNPOEEEGRED UPRTQE
+CMGL: 10,"REC UNREAD","UNOTV.COM","","20/01/03,22:18:21-24"
GENARO GARCIA LUNA SE DECLARA INOCENTE EN EU/NUEVO ATAU EE EEENRKHYVROMUTSBNTSYBRA:NIAI EDNAA   OA nvm/J

+CMGL: 11,"REC UNREAD","7121157686","","20/01/07,14:52:48-24"
holaaaa

+CMGL: 12,"REC UNREAD","UNOTV.COM","","20/01/07,16:27:02-24"
CUIDADO: AHORA DROGAN A TU PERRO ARA ROBAR CASA/VIDE:EEGNI  PRORC O RROOFCSS NNPASLA  MG RPD nt.
+CMGL: 13,"REC UNREAD","UNOTV.COM","","20/01/07,22:23:31-24"
IRAN LANZA MAS DE UNA DOCENA DE MISILES A BASE DE TROA EE NIA/AA LNT E ANOD ATER/ UVNON,V E VA nt.x1

+CMGL: 14,"REC UNREAD","UNOTV.COM","","20/01/08,18:33:59-24"
TRUMP MANDA ESTE MENSAJE A IRAN TRAS ATAQUE DE AYER/CATNMMNODAINZ NIA EDJ 7 URO/CEO AN EMIRETSVDO otm/K

+CMGL: 15,"REC UNREAD","UNOTV.COM","","20/01/09,15:38:08-24"
INFORMACION INDCA QUE MISIL IRANI DERRIBO AVION:TRUDEUTRET JR EPRTR -0E SO DSA
 AAQUE EN ESCUELA,SEP VA CONTRA VIOLENCIA CON ESTAS MEDIDAS/DE INPOI,LJKRuovx1k

 SI YA NO HYA MAS MENSAJES QUE LEER RETORNA
 AT+CMGL="REC UNREAD"
OK

*/
#define AT_SMS_LEER_MENSAJES_CLASIFICACION_UNREAD "AT+CMGL=\"REC UNREAD\"" 
#define AT_SMS_LEER_MENSAJES_CLASIFICACION_READ "AT+CMGL=\"REC READ\"" 
#define AT_SMS_LEER_MENSAJES_CLASIFICACION_UNSENT "AT+CMGL=\"STO UNSENT\"" 
#define AT_SMS_LEER_MENSAJES_CLASIFICACION_SENT "AT+CMGL=\"STO SENT\"" 
#define AT_SMS_LEER_MENSAJES_CLASIFICACION_ALL "AT+CMGL=\"ALL\"" 

//en modo PDU es similar solo que hay que poner los numeros AT+CMGL=1

/*
Eliminar mensaje del almacenamiento elegido como predeterminado, puede ser SIM o Ram hasta donde se lee
<DelFlag>
0    Delete message at location <index>.
1	Delete All READ messages
2	Delete All READ and SENT messages
3	Delete All READ, SENT and UNSENT messages 
4   Delete All messages. 
*/

/*
Elimina el mensaje de la posicion X remplazar X por la posicion de memoria donde esta el sms 0,1,2,3,etc
Ej. AT+CMGD=3
*/
#define AT_SMS_ELIMINAR_MENSAJE_EN_POSICION "AT+CMGD=X,0"
/*
Remplazar X por que clasificacion de los mensajes quieres borrar
1	Delete All READ messages
2	Delete All READ and SENT messages
3	Delete All READ, SENT and UNSENT messages
4   Delete All messages.
*/
#define AT_SMS_ELIMINAR_MENSAJES_POR_CLASIFICACION "AT+CMGD=1,X"


//MODIFICAR EL ESTATUS O CLASIFICACION DE UN MENSAJE CREO QUE NO ME FUNCIONA U.U
/*
This command allows the manipulation of a message status. 
The accepted status changes are from READ to NOT READ and vice versa,
and also from SENT to NOT SENT and vice versa.
PDU	Mode		Text Mode
0			 "REC UNREAD"
1			  "REC READ"
2			  "STO UNSENT"
3              "STO SENT"

SI POR EJEMPLO TE LELGA UN MENSAJE AL MODULO, LO LEES, EL MODULO LO MARCARA COMO LEIDO
AQUI PODRIAS VOLVER A PONERLO EN NO LEIDO
*/
/*
*/
#define AT_SMS_CAMBIAR_STATUS_A_UNREAD "AT+WMSC=X,\"REC UNREAD\""
#define AT_SMS_CAMBIAR_STATUS_A_READ "AT+WMSC=X,\"REC READ\""
#define AT_SMS_CAMBIAR_STATUS_A_NOT_SENT "AT+WMSC=X,\"STO UNSENT\""
#define AT_SMS_CAMBIAR_STATUS_A_SENT "AT+WMSC=X,\"STO SENT\""
		















//COMANDO LLAMADAS
/*
REMPLAZAR LAS 10 X POR EL NUMERO DE TELEFONO A ELEGUIR
*/
#define AT_LLAMADA_REALIZAR "ATDXXXXXXXXXX;"	//se tiene que colocar despues el numero de telefono ATD7121157686; maximo 10 sigitos
#define AT_LLAMADA_RESPONDER_LLAMADA "ATA"	//si le estan llamando al modulo con ese comando atiende la llamada
#define AT_LLAMADA_COLGAR_LLAMADA "ATH"	//si se contesto o enlazo una llamada con este comando el modulo puede colgar la llamada
/*
20

TD7121157686;
OK

*/

//IDENTIFICADOR LLAMADA ESTE YA SE GUARDA POR DEFAULT EN LA MEMORIA NO VOLATIL DEL MODULO, AUNQUE SE REINICIE MANTIENEN LA CONFIGURACION
	/*
	identificador de llamada el default es desactivado, cuando nos llama un numero el modulo solo enviara RING por puerto serie
	al arduino, si queremos saber que numero nos esta llamando activamos el comnando y ahora nos enviara tambien el numero que nos llama con cada ring
	RING

	+CLIP: "7121157686",129,"",0,"",0

	RING

	+CLIP: "7121157686",129,"",0,"",0

	RING

	+CLIP: "7121157686",129,"",0,"",0

	RING

	+CLIP: "7121157686",129,"",0,"",0
	*/
#define AT_LLAMADA_CONFIG_IDENTIFICAR_LLAMADA_ACTIVADO___ATW "AT+CLIP=1"
#define AT_LLAMADA_CONFIG_IDENTIFICAR_LLAMADA_DESACTIVADO___ATW "AT+CLIP=0"
#define AT_LLAMADA_CONFIG_IDENTIFICAR_LLAMADA_DESACTIVADO___ATW "AT+CLIP?"
#define ESPERADA_RESPUESTA_IDENTIFICACOR_LLAMADA "OK"
	/*
	15BYTES
	AT+CLIP=1
	OK

	*/









//https://cdn-shop.adafruit.com/product-files/1946/SIM800+Series_TCPIP_Application+Note_V1.01.pdf
//COMANDOS CONEXION INTERNET
/*
Es usado para "conectar" el dispositivo al servicio de dominio de paquetes para poder transmitir por internet
Debe activarse antes de iniciar cualquier contexto de conexion
AT+CGATT=1
AT+CGATT=1
OK
13bytes
*/
#define AT_GPRS_ACTIVAR "AT+CGATT=1"
/*
Es usado para "DESCONECTAR" el dispositivo al servicio de dominio de paquetes para poder transmitir por internet
AT+CGATT=0
OK
13bytes
*/
#define AT_GPRS_DESACTIVAR "AT+CGATT=0"
/*
Preguntar el estado si esta activado o no para transmision de paquetes
AT+CGATT?
AT+CGATT?
+CGATT: 0

OK
32bytes
en este caso noe sta conectado o activado
*/
#define AT_GPRS_INFO_ESTADO_ACTUAL "AT+CGATT?"
/*
Que estados soporta las respuestas son 0 detach oi no conectadpo o 1 attach o conectado
AT+CGATT=?
+CGATT: (0,1)

OK
28BYTES
*/
#define AT_GPRS_INFO_ESTADOS_SOPORTADOS "AT+CGATT=?"
/*
iNVESTIGAR QUE HACE ESTE COMANDO
Check attach status EN ESTA PAGOINA SALE

AT+CGACT?
+CGACT: 1,0
+CGACT: 2,0
+CGACT: 3,0

OK
*/
#define AT_GPRS_INFO_CHECK_ATTACH_STATUS "AT+CGACT?"

/*
CONFIGURAR APN de la compañia telefonica
AT+CSTT="Nombre del Punto de Acceso o APN","usuario","contraseña"
Remplazar name, usuario, password
por ejemplo para Telcel mexico es
AT+CSTT="internet.itelcel.com","webgprs","webgprs2003"


AT+CSTT="internet.itelcel.com","webgprs","webgprs2003"
OK
Esta es la respuesta que entrega

una vez definido el APN al querer enviar este comando otra vez, el modulo responde ERROR
aunque lo cambies por otro diferente siue respondiendo error
hasta que reinicias el modulo completamente deja de marcarlo
AT+CSTT="","",""

Cuando no tiene apn definido o cada que se reinicia es
AT+CSTT?
+CSTT: "CMNET","",""

OK

CREO QUE UNA VEZ DEFINIDO EL APN LA UNICA MANERA DE BORRARLO Y CONFIGURAR OTRO NUEVO SERA REINICIANDO EL MODULO MANDANDO A TIERRA SU PATITA DE RESET O APAGANDOLO Y ENCENDIENDOLO CON
LA FUNCION QUE HICE, QUE ES MANDAR A 1LTU SU PIN DE ENCENDIDO PRO 1SEGUNDO Y DESPUES A 0 POR OTRO SEGUNDO
*/
#define AT_GPRS_APN_DEFINIR_APN "AT+CSTT=\"name\",\"usuario\",\"password\""
/*
Saber el APN actual configurado
si no hay uno configurado devuelve
AT+CSTT?
+CSTT: "CMNET","",""

OK

despues de configurar el de telcel RESPONDE
AT+CSTT?
+CSTT: "internet.itelcel.com","webgprs","webgprs2003"

OK

*/
#define AT_GPRS_APN_ACTUAL "AT+CSTT?"
/*
Saber los parametros que se le tienen que enviar para su configuracion
AT+CSTT=?
+CSTT: "APN","USER","PWD"

OK
39bytes
*/
#define AT_GPRS_APN_INFO_PARAMETROS "AT+CSTT=?"
/*
20BYTES
AT+CIIC
OK
LLAMA Y CONECTA LA COFIGURACION DE APN PARA INICIAR LA CONEXION PREVIAMENTE DEFINIDA CON AT+CSTT
AT+CIICR command brings up the GPRS or CSD call depending on the configuration previously set by the AT+CSTT command.


*/
#define AT_GPRS_APN_CONECTAR_CON_APN "AT+CIICR"		//AL ENVIAR ESTE COMANDO EL MODULO COMIENZA A PARPADEAR RAPIDO ya que se conecta con el apn definido a telcel y obtiene una ip
/*
OBTENER LA DIRECCION IP AT+CIFSR command returns the local IP address. It is imperative the the PDP context must have been activated before to get the IP address.
AT+CIFSR
10.79.252.164

responde eso!

Si no hay contexto gprs creado responde 
AT+CIFSR
ERROR

*/
#define AT_GPRS_APN_IP_OBTENER "AT+CIFSR"

/*
Iniciar conexion TCP or UDP
Parameters
- 0..7 - Connection number
- "TCP" or "UDP"
- Remote server IP address
- Remote server port
- remote domain name

AT+CIPSTART=?
+CIPSTART: ("TCP","UDP"),("(0,255).(0,255).(0,255).(0,255)"),(1,65535)
+CIPSTART: ("TCP","UDP"),("DOMAIN NAME"),(1,65535)

OK

AT+CIPSTART="TCP","116.228.221.51","8500"

AT+CIPSTART="UDP","116.228.221.51","9600"



despues de enviar este comando
AT+CIPSTART="TCP","www.prometec.net","80"
OK											//responde esto

CONNECT OK									//tarda un momentito y envia esto, se queda asi, "abierta la conexion"

AT+CIPSTART="TCP","www.kaliopeadmin.com.mx","80"




//haciendolo con este video el sistema si recibio la informacion https://www.youtube.com/watch?v=irI11IBUojs&t=574s
AT+CIPSTART="TCP","api.thingspeak.com",80
CONNECT OK
AT+CIPSEND
>GET https://api.thingspeak.com/update?api_key=2PIURY5OB1ZF0FRH&field1=080


SEND OK


AT+CIPSTART="TCP","162.241.93.228","80"
OK

CONNECT OK			para esta respuesta tardo alco mo 2 segundos


si vuelves a enviar o hacer la conexion envia
AT+CIPSTART="TCP","162.241.93.228","80"
ERROR

ALREADY CONNECT				de inmediato



AT+CIPSTART="TCP","162.241.93.228","80"
AT+CIPSEND
POST /gps_luisda.php HTTP/1.1\r\nHost: 162.241.93.228\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 31\r\n\r\ndatos=Sim808EnviandoDatosATabla\r\n\x1A


POST /gps_luisda.php HTTP/1.1\r\nHost: 162.241.93.228\r\nContent-Type: application/x-www-form-urlencoded\r\nKeep-Alive:\r\nConnection: Keep-Alive\r\nContent-Length: 31\r\n\r\ndatos=Sim808EnviandoDatosATabla\r\n\x1A


*/
#define AT_GPRS_INFO_INICIAR_CONEXION "AT+CIPSTART=\"PROTOCOLO\",\"IP-DOMINIO\",\"PORT\""
/*
para saber que parametros soporta este comando y hay que enviarle
*/
#define AT_GPRS_INFO_INICIAR_CONEXION_PARAMETROS_SOPORTADOS "AT+CIPSTART=?"

/*
Para comenzar a escribir una vez que se ha abierto la conexion exitosamente
AT+CIPSTART="TCP","162.241.93.228","80"		por ejemplo nos conectamos aqui
OK

CONNECT OK									despues de unos segundos responde
AT+CIPSEND									enviamos este comando
>											y responde con este simbolo significando que todo lo que escribamos desde ahora sera informacion a enviar, no funcionan los comandos at ya que los recibe como parametro de informacion a enviar

		despues de escribir la informacion deseada para enviarla se usa el comando char(26) o este \x1A o este \u001A todos apuntan al mismo comando el ASCII 26 SUB
		es el mismo que se usa para enviar los sms, o si quieres cancelar hay que enviar el comando char(27) ESC \x1b  \u001b    el numero decimal 27 en exadecimal es 1b

al enviar el caracter ESC el gps responde ok y comienzan a funcionar denuevo normalmente
*/
#define AT_GPRS_ENVIAR_INFORMACION "AT+CIPSEND"


/*
47 A 52BYTES
AT+CIPSTATUS
AT+CIPSTATUS
OK

STATE: CONNECT OK
-----------------------------------
CONSULTAR EL ESTATUS DE LA CONEXION
IP INITIAL Si no se ha configurado ningun APN y por lo tanto no hay ningon modo gprs activado aun
IP START Si ya se definio el APN por lo tanto la configuracion esta a la mitad, en este momento no se puede cambiar el APN a menos que se cierre con AT+CIPSHUT
IP GPRSACT Si se conecto correctamente a la red GPRS el led esta parpadeando rapidamente y ya se puede establecer una conexion con un servidor
IP STATUS significa que esta parpadeando rapidamente el led y que ya emos obtenido la direccion ip con AT+CIFSR		estamos en contexto GPRS
COONECT OK significa que esta parpadeando rapidamente el led y que hay una conexion establecida con un servidor!
TCP CLOSE significa que esta parpadeando rapidamente el led y pero que se cerro la conexion que se tenia con el servidor anterior Segimos en contexto GPRS
ERROR Si hay cualquier error al preguntar el status



*/
#define AT_GPRS_INFO_STATUS_DEL_CONTEXTO "AT+CIPSTATUS"
/*
Cerrar la conexion actual, si estas conectado con un servidor

AT+CIPCLOSE
CLOSE OK

si la conexion ya esta cerrada
AT+CIPCLOSE
ERROR

*/
#define AT_GPRS_CIERRA_CONEXION "AT+CIPCLOSE"
/*
AL ENVIAR ESTE COMANDO EL LED DEJA DE PARPADEAR RAPIDAMENTE SE CJERRRA EL CONTREXTO GPRS Y SE PUEDE DEFINIR UN NUEVO APN

AT+CIPSHUT
SHUT OK
*/
#define AT_GPRS_CIERRA_CONTEXTO "AT+CIPSHUT"				


/*
Sirve para obtener la ip de un dominio por ejemplo pongo 
AT+CDNSGIP=www.kaliopeadmin.com.mx
OK

+CDNSGIP: 1,"www.kaliopeadmin.com.mx","162.241.95.227"
AT+CDNSGIPwww.kaliopegps.com.mx
OK

y devuelve la ip que esta asignada a esa direccion, si la direcion aun no tiene la ip asignada 
no retorna nada

AT+CDNSGIPwww.kaliopegps.com.mx
OK

+CDNSGIP: 0,8

*/
#define AT_GPRS_UTILERIA_OBTENER_IP_DE_DOMINIO "AT+CDNSGIP=XX"







//RELOJ RCT INTERNO DEL MODULO, EL RELOJ NO ES MUY BUENO CON EL PASO DE DIAS TIENE DESFASE EN MINUTOS HASTA HORAS MEJOR USAR EL RELOJ DEL GPS
/*
Define la hora del reloj RTC incluido en el modulo la informacion de este reloj no se usa para saber a que hora entro un sms, al aprecer se usa la hora de la red
es un poco malo al parecer se comienza a desfazar minutos cuando esta apagado, no deberia se supone que para eso tiene su bateria
reemplazar las aa por el año, MM por el mes etc, en la xxx poner la zona horaria por ejemplo -06, -05 ,+12 etc
*/
#define AT_RTC_FECHA_HORA_DEFINIR "AT+CCLK=\"aa/MM/dd,hh:mm:ssXXX\""  //aa,MM,dd,hh,mm,ss año es en 2 digitos, la sona horaria puede ser -06 invierno mexico -05 verano mexivo
/*
36

AT+CCLK="20/01/08,13:49:10-06"
OK
SI hay error responde
41

AT+CCLK="08/01/2020,13:49:10-06"
ERROR
*/
#define AT_RTC_FECHA_HORA_CONSULTAR "AT+CCLK?"
/*
47 byte

T+CCLK?
+CCLK: "08/01/20,13:53:05-06"

OK
*/











//ALARMAS DEL MODULO CON RTC

/*
AT+CALA=”00/06/09,07:30” Note: set an alarm for June 9th, 2000 at 7:30 am
OK Note: Alarm stored


AT+CALA? Note: list all alarms
+CALA: “00/06/08,15:25:00”,0
+CALA: “00/06/09,07:30:00”,1
+CALA: “00/06/10,23:59:00”,2 Note: three alarms are set (index 0, 1, 2)


T+CALA=””,2 Note: delete alarm index 2

OK Note: Alarm index 2 deleted
*/
