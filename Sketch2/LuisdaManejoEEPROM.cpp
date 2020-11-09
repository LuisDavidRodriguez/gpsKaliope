#include "LuisdaManejoEEPROM.h"



/*
Demostramos que la Memoria eeprom puede ser recorrida como un array
*/
void LuisdaManejoEEPROM::recorrerIndicesMemoriaEEPROM()
{
	for (size_t i = 0; i < 300; i++)
	{
		char pff = EEPROM.read(i);
		Serial.println("Pos: " + String(i) + " valor " + pff);
	}
}

/*
Por ejemplo si quiero borrar algun dato que haya puesto, hasta cierto limite
la memoria eeprom esta incializada en 255, si quiero escribir 0 en cada byte lo hago aqui
el metodo evalua si la posicion final pasada es mayor al tamaño de la memoria eeprom
no lo ejecutara
*/
void LuisdaManejoEEPROM::escribirByteAbyteMemoria(byte valor, int limite)
{
#ifdef DEBUG
	Serial.println("Tamano de memoria EEPROM: " + String(EEPROM.length()) + " bytes");
#endif // DEBUG

	if (limite >= EEPROM.length()) {

#ifdef DEBUG
		Serial.println(F("Se paso un valor final a escribir mayor a la memoria eeprom"));
#endif // DEBUG

		return;
	  	}

	for (size_t i = 0; i <= limite; i++)
	{
		EEPROM.write(i,valor);
	}

}

/*
Guarda en la eeprom un long es usado para guardar 
la variable que sabe hasta que byte del archivo que esta
guardado en la SD se envio, enviaremos varios bytes en una sola conexion
si el servidor responde que 200 ok significa que ya los recibio hasta la ultima
posicion que enviamos, depues el siguiente envio lo hara apartir de la ultima posicion 
enviada
*/
void LuisdaManejoEEPROM::setUltimaLecturaMemoriaSD(long info)
{
	EEPROM.put(_EELUISDA_ADDRESS_SD_ULTIMA_POSICION_ENVIADA, info);
}
/*
Devuelve un un long almacenado en la meoria EEPROM
es el long usado para saber la ultima posicion del archivo SD que fue enviado
*/
long LuisdaManejoEEPROM::getUltimaLecturaMemoriaSD()
{
	long info;
	EEPROM.get(_EELUISDA_ADDRESS_SD_ULTIMA_POSICION_ENVIADA, info);
	return info;
}


/*
Obtiene el nombre de red guardado en la EEPROM
*/
String LuisdaManejoEEPROM::getAPN_nombreRed()
{
	char info[_EELUISDA_SIZEOF_APN_NOMBRE_RED];			//Creamos el char array con la misma medida que el que se guardo, asi el metodo .get sabra hasta donde leer de la memoria

	EEPROM.get(_EELUISDA_ADDRESS_APN_NOMBRE_RED, info);

	return String(info);								//llamamos al constructor string para que retorne un String porque recordar que en las funciones de c++ no podemos retornar por valor los arrays
}
/*
Guardamos el nombre enviado en la eeprom
Solo guardara 20 caracteres IMPORTANTE
*/
void LuisdaManejoEEPROM::setAPN_nombreRed(String nombreRed20Caracteres)
{

	char info[_EELUISDA_SIZEOF_APN_NOMBRE_RED];					//la eeprom no puede escribir objetos String, entonces se cre aun objeto string con s minuscula que es un char array, asi la eeprom ya sabe cuantos bytes escribira y cuantos regresara
	memset(info, '\0', _EELUISDA_SIZEOF_APN_NOMBRE_RED);//llenamos vacio todo el array equivale al ascii NULL

	nombreRed20Caracteres.toCharArray(info, _EELUISDA_SIZEOF_APN_NOMBRE_RED);
	EEPROM.put(_EELUISDA_ADDRESS_APN_NOMBRE_RED, info);	
	
}





String LuisdaManejoEEPROM::getAPN_user()
{
	char info[_EELUISDA_SIZEOF_APN_USER];			//Creamos el char array con la misma medida que el que se guardo, asi el metodo .get sabra hasta donde leer de la memoria

	EEPROM.get(_EELUISDA_ADDRESS_APN_USER, info);

	return String(info);								//llamamos al constructor string para que retorne un String porque recordar que en las funciones de c++ no podemos retornar por valor los arrays

}

void LuisdaManejoEEPROM::setAPN_user(String user20Caracteres)
{
	char info[_EELUISDA_SIZEOF_APN_USER];					//la eeprom no puede escribir objetos String, entonces se cre aun objeto string con s minuscula que es un char array, asi la eeprom ya sabe cuantos bytes escribira y cuantos regresara
	memset(info, '\0', _EELUISDA_SIZEOF_APN_USER);//llenamos vacio todo el array equivale al ascii NULL
	user20Caracteres.toCharArray(info, _EELUISDA_SIZEOF_APN_USER);
	
	
	
	EEPROM.put(_EELUISDA_ADDRESS_APN_USER, info);
}





String LuisdaManejoEEPROM::getAPN_password()
{
	char info[_EELUISDA_SIZEOF_APN_PASSWORD];			//Creamos el char array con la misma medida que el que se guardo, asi el metodo .get sabra hasta donde leer de la memoria

	EEPROM.get(_EELUISDA_ADDRESS_APN_PASSWORD, info);

	return String(info);								//llamamos al constructor string para que retorne un String porque recordar que en las funciones de c++ no podemos retornar por valor los arrays

}

void LuisdaManejoEEPROM::setAPN_password(String password20Caracteres)
{
	char info[_EELUISDA_SIZEOF_APN_PASSWORD];					//la eeprom no puede escribir objetos String, entonces se cre aun objeto string con s minuscula que es un char array, asi la eeprom ya sabe cuantos bytes escribira y cuantos regresara
	memset(info, '\0', _EELUISDA_SIZEOF_APN_PASSWORD);//llenamos vacio todo el array equivale al ascii NULL
	password20Caracteres.toCharArray(info, _EELUISDA_SIZEOF_APN_PASSWORD);



	EEPROM.put(_EELUISDA_ADDRESS_APN_PASSWORD, info);
}


String LuisdaManejoEEPROM::getIMEI()
{
	char info[_EELUISDA_SIZEOF_IMEI];			//Creamos el char array con la misma medida que el que se guardo, asi el metodo .get sabra hasta donde leer de la memoria

	EEPROM.get(_EELUISDA_ADDRESS_IMEI, info);

	return String(info);								//llamamos al constructor string para que retorne un String porque recordar que en las funciones de c++ no podemos retornar por valor los arrays

}

void LuisdaManejoEEPROM::setIMEI(String imei)
{
	char info[_EELUISDA_SIZEOF_IMEI];					//la eeprom no puede escribir objetos String, entonces se cre aun objeto string con s minuscula que es un char array, asi la eeprom ya sabe cuantos bytes escribira y cuantos regresara
	memset(info, '\0', _EELUISDA_SIZEOF_IMEI);//llenamos vacio todo el array equivale al ascii NULL
	imei.toCharArray(info, _EELUISDA_SIZEOF_IMEI);



	EEPROM.put(_EELUISDA_ADDRESS_IMEI, info);
}




void LuisdaManejoEEPROM::getConfiguraciones()
{
	EEPROM.get(_EELUISDA_ADDRESS_CONFIGURACIONES, configuraciones);


}


void LuisdaManejoEEPROM::setConfiguracionesDesdeString(String cadena)
{
	String auxiliar;
	//'STATUSKALIOPEOK,60000,30000,1,10000
	//0
	//
	//
	//______________
	cadena.remove(cadena.indexOf('\n'), cadena.length());//removemos el 0 de abajo y solo dejamos el primer renglon
	cadena.remove(0, cadena.indexOf(',') + 1);
	Serial.println(cadena);
	//60,30,1,10,10,1,-6,0
	auxiliar = cadena.substring(0, cadena.indexOf(","));	
	Serial.println(auxiliar);
	configuraciones.tiempoEnvioCoordenadas = (long) auxiliar.toInt()*1000;
	Serial.println(String(configuraciones.tiempoEnvioCoordenadas));


	cadena.remove(0, cadena.indexOf(',') + 1);
	Serial.println(cadena);
	//30,1,10,10,1,-6,0
	auxiliar = cadena.substring(0, cadena.indexOf(","));
	Serial.println(auxiliar);
	configuraciones.tiempoEnvioLecturasAcelerometro = (long)auxiliar.toInt()*1000;
	Serial.println(String(configuraciones.tiempoEnvioLecturasAcelerometro));


	cadena.remove(0, cadena.indexOf(',') + 1);
	Serial.println(cadena);
	//1,10,10,1,-6,0
	auxiliar = cadena.substring(0, cadena.indexOf(","));
	Serial.println(auxiliar);
	configuraciones.calibrarAcelerometro = (auxiliar.equals("1")) ? true : false;
	Serial.println(String(configuraciones.calibrarAcelerometro));


	cadena.remove(0, cadena.indexOf(',') + 1);
	Serial.println(cadena);
	//10,10,1,-6,0
	auxiliar = cadena.substring(0, cadena.indexOf(","));
	Serial.println(auxiliar);
	configuraciones.tiempoPing = (long)auxiliar.toInt() * 1000;
	Serial.println(String(configuraciones.tiempoPing));



	cadena.remove(0, cadena.indexOf(',') + 1);
	Serial.println(cadena);
	//10,1,-6,0
	auxiliar = cadena.substring(0, cadena.indexOf(","));
	Serial.println(auxiliar);
	configuraciones.tiempoEnviarDatosSD = (long)auxiliar.toInt()*1000;
	Serial.println(String(configuraciones.tiempoEnviarDatosSD));


	cadena.remove(0, cadena.indexOf(',') + 1);
	Serial.println(cadena);
	//1,-6,0
	auxiliar = cadena.substring(0, cadena.indexOf(","));
	Serial.println(auxiliar);
	configuraciones.bloqueoDeMotor = (auxiliar.equals("1")) ? true : false;
	Serial.println(String(configuraciones.bloqueoDeMotor));


	cadena.remove(0, cadena.indexOf(',') + 1);
	Serial.println(cadena);
	//-6,0
	auxiliar = cadena.substring(0, cadena.indexOf(","));
	Serial.println(auxiliar);
	configuraciones.zonaHoraria = auxiliar.toInt();
	Serial.println(String(configuraciones.zonaHoraria));




	cadena.remove(0, cadena.indexOf(',') + 1);
	Serial.println(cadena);
	//0
	auxiliar = cadena.substring(0, cadena.length()-1);
	Serial.println(auxiliar);
	configuraciones.enviarLecturasAcelerometro = (auxiliar.equals("1")) ? true : false;
	Serial.println(String(configuraciones.enviarLecturasAcelerometro));







	EEPROM.put(_EELUISDA_ADDRESS_CONFIGURACIONES, configuraciones);
}

/*
Reguarda la estructura en la EEPROM
si realizaste alguna modificacion en un valor de la estrucutrua es mejor que actualices su valor en la eeprom tambien
*/
void LuisdaManejoEEPROM::setConfiguraciones()
{
	EEPROM.put(_EELUISDA_ADDRESS_CONFIGURACIONES, configuraciones);
}



