#include "Acelerometro.h"

Acelerometro::Acelerometro(uint8_t address)
{
	this->address = address;


}

/*
Escribiremos al esclavo del acelerometro su id que le indicara que debe hacer una calibracion en el sensor del acelerometro
*/
bool Acelerometro::calibrar()
{
	Wire.beginTransmission(address);
	Wire.write(ACELEROMETRO_ID_CALIBRAR);
	
	if (Wire.endTransmission() == 0) {
		Serial.println(F("Se ha enviado correctamente el comando de calibracion"));
		return true;
	}
	else {
		Serial.println(F("No se envio el comando de calibracion, revizar coneccion con el esclavo o direccion i2c"));
		return false;
	}


}

/*
@Return
true si se recibieron los datos y se actualizo la structura correctamente
false si no se recibieron datos del esclavo
	Llena la estructura con los datos actuales del acelerometro funcionara para saber si es necesario recalibrar el acelerometro
	preguntando frecuentemente cuales son los ges actuales. tenemos las lecturas analogicas, en fuerza g y tenemos los putos de calibracion
	0 ges que tienen guardados en la eeprom
*/
bool Acelerometro::requestActualizarDatosLecturasActuales()
{
	Wire.requestFrom(address,24);
	Serial.print(F("Bytes disponibles: "));
	Serial.println(String(Wire.available()));
	if (Wire.available() == 24) {
		Wire.readBytes((byte*)&lecturas, 24);

		Serial.print(F("Bytes disponibles despues: "));
		Serial.println(String(Wire.available()));
		return true;
	}
	return false;
}

/*
	Este metodo solo entrega el string de la estructura
	pero atento son los datos que tiene en la memoria este arduino desde el ultimo request
	para obtener la informacion actual primero hay que llamar al request de actualizar datos
	y despues llamas a este metodo que imprime los datos que se recibieron
	xAnalog,yAnalog,zAnalog,xGes,yGes,zGes,x0Ges,y0Ges,z0Ges,fecha,hora
	*/
String Acelerometro::imprimirLecturasEnEstructura()
{
	return
	String(lecturas.xAnalog) + "," + String(lecturas.yAnalog) + "," + String(lecturas.zAnalog) + "," +
		String(lecturas.xGes) + "," + String(lecturas.yGes) + "," + String(lecturas.zGes) + "," +
		String(lecturas.xPunto0ges) + "," + String(lecturas.yPunto0ges) + "," + String(lecturas.zPunto0ges);
}
