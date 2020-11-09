#pragma once
#include <Arduino.h>
#include <EEPROM.h>

class LuisdaManejoEEPROM
{
private:


#define _EELUISDA_ADDRESS_SD_ULTIMA_POSICION_ENVIADA 0		//guardaremos un long usara 4 bytes 0-3 sigiente posicion 4


#define _EELUISDA_ADDRESS_APN_NOMBRE_RED 4				//guardaremos el char array de 40 caracteres maximo 4-43 siguiente posicion 44
#define _EELUISDA_SIZEOF_APN_NOMBRE_RED 40			//usamos esto para definir el tamaño de los array que se este es el limite de caracteres que podremos guardar


#define _EELUISDA_ADDRESS_APN_USER 44			//iniciamos en la 44 guardaremos 40 caracteres terminaremos en la 83, siguiente posicion 84
#define _EELUISDA_SIZEOF_APN_USER 40			//usamos esto para definir el tamaño de los array que se este es el limite de caracteres que podremos guardar


#define _EELUISDA_ADDRESS_APN_PASSWORD 84			//iniciamos en la 84 guardaremos 40 caracteres terminaremos en la 123, siguiente posicion 124
#define _EELUISDA_SIZEOF_APN_PASSWORD 40			//usamos esto para definir el tamaño de los array que se este es el limite de caracteres que podremos guardar

	/*
	869170031697544
	Este es el IMEI normal de un modulo SIM lo guardaremos en la EEPROM para
	*/
#define _EELUISDA_ADDRESS_IMEI 124			//iniciamos en la 124 guardaremos 20 caracteres terminaremos en la 143, siguiente posicion 144
#define _EELUISDA_SIZEOF_IMEI 20			//usamos esto para definir el tamaño de los array que se este es el limite de caracteres que podremos guardar, el imei es de 15 dejamos 20 de tolerancia


#define _EELUISDA_ADDRESS_CONFIGURACIONES 144			//iniciamos en la 144 guardaremos una estructura 17 bytes caracteres terminaremos en la 160, siguiente posicion 161
#define _EELUISDA_SIZEOF_CONFIGURACIOENS 17			

	struct config {
		long tiempoEnvioCoordenadas = 0;
		long tiempoEnvioLecturasAcelerometro = 0;
		bool calibrarAcelerometro = false;
		long tiempoPing = 0;
		long tiempoEnviarDatosSD = 0;
		bool bloqueoDeMotor = false;
		int zonaHoraria = 0;
		bool enviarLecturasAcelerometro = false;
	};

public:

	config configuraciones;
	
	
	void begin();

	void recorrerIndicesMemoriaEEPROM();

	void escribirByteAbyteMemoria(byte valor, int limite);

	void setUltimaLecturaMemoriaSD(long info);

	long getUltimaLecturaMemoriaSD();



	String getAPN_nombreRed();

	void setAPN_nombreRed(String user20Caracteres);		

	String getAPN_user();

	void setAPN_user(String user20Caracteres);

	String getAPN_password();

	void setAPN_password(String password20Caracteres);


	String getIMEI();

	void setIMEI(String imei);



	

	void getConfiguraciones();
	
	void setConfiguracionesDesdeString(String cadena);

	void setConfiguraciones();

};

