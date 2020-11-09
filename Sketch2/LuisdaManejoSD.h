#pragma once

/*
Creamos esta clace para manejar mas facilmente la memoria SD es una clase Statica ya que no tiene caso
crear objeto sobre esta clase cuando la libreria SD funciona sin objeto.


*/

#include <Arduino.h>
#include <SD.h>
#include "ClaseManejoSim808.h"

/*
Mantendremos los nombres de los archivos menores a 8 caracteres sin extencion, hasta donde se es lo que la libreria SD soporta
8 de nombre, 3 de extencion
*/

/*
Los arvhicvos terminados con UP
seran en donde se guardara la ultima posicion enviado al servidor
*/

#define _LUISDA_SD_NOMBRE_LOCALIZACION "LOCAL.txt"
#define _LUISDA_SD_NOMBRE_LOCALIZACION_ULTIMA_POSICION "LOCALUP.txt"			//aqui guardaremos la ultima posicion del archivo localizacion que fue enviada al servidor, la queria guardar en la eeprom pero tiene vida util me preocupa que se llege a terminar

/*
En este archivo se llevaran los registros del flujometro de GLP
*/
#define _LUISDA_SD_NOMBRE_FLUJOMETRO "FLUJ.txt"
#define _LUISDA_SD_NOMBRE_FLUJOMETRO_ULTIMA_POSICION "FLUJUP.txt"



#define _LUISDA_SD_NOMBRE_ACELEROMETRO "ACEL.txt"
#define _LUISDA_SD_NOMBRE_ACELEROMETRO_ULTIMA_POSICION "ACELUP.txt"



class LuisdaManejoSD
{

private:
	static File myFileL;

public:
	/*
	Guardare con estos 2 metodos un archivo donde solo se guardara un numero, ese numero me funcionara
	para saber hasta que posicion del archivo de coordenadas fue enviado ya al servidor, debido a que si el archivo
	se hace muy grande porque no hay datos gprs y el gps tiene que guardarlos todos en la memoria sd
	tengo que enviar parte por parte el archivo total, no puedo enviarlo todo por el peso que se podria acomular
	entonces envio un pedacito se en que posicion inicie y en cual termino, ahora guardo esa posicion final
	en la memoria Sd en el archivo UPOS para que si el arduino se reinicia no se pierda la informacion, queria hacerlo en la memoria EEPROM
	pero me preocupa la vida limitada de la memoria, y aparte por ejemplo si retiran la memoria SD de alguna manera tengo que saberlo
	y borrar la memoria guardada en la eeprom porque podria si se incerta una memoria nueva al no tener el archivo coordenadas
	y pori ejemplo se guardo la posicion 1000 en la eprom intentaria leer un arvhico vacio apartir de esa posicion.
	asi si retiran la SD por lo tanto tambien se retira el archivo que guarda la posicion final. bueno se que
	con mas tiempo y razonamiento no habria mayor problema si usamos la eeprom, en lo mientras me decanto por esta opcion se me hace comoda
	*/
	static bool setUltimaPosicionEnviadaLocalizacion(long number);
	static long getUltimaPosicionEnviadaLocalizacion();



	static bool setUltimaPosicionEnviada(String nombreDelArchivoUltimaPosicion,long number);
	static long getUltimaPosicionEnviada(String nombreDelArchivoUltimaPosicion);

	

	static bool escribirArchivo(String nombre, String informacion);
	static bool escribirArchivo(String nombre, String informacion, String informacion2);



	static String leerArchivoResponderStringHastaLineaNueva(String nombreArchivo, long& posicionInicial, long& posicionFinal, long& totalSize);




	static long getTotalSizeOf(String nombreArchivo);


	static void evaluarBytesDisponiblesParaEnvioHastaSaltoDeLinea(String nombreArchivoSD,long posicionInicial, long& posicionFinal, long& totalBytes, long& totalSize);


	static bool enviar1000bytesEnSDporSim808(ClaseManejoSim808* sim808, String dominioSecundario, String archivoDeDestinoEnServidor, String nombreDelArchivoSD, String nombreDelArchivoUltimaPosicion, long posicionInicial, String info2);

	static String quitarCaracteresNoAscii(String lineaPorRevizar);


	static void leerUnRenglon();




};

