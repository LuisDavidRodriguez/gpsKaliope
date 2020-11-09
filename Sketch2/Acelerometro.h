#pragma once
#include <Wire.h>
#include <Arduino.h>


#define ACELEROMETRO_ID_CONTADORES 2
#define ACELEROMETRO_ID_CALIBRAR 10

class Acelerometro
{
private:
	uint8_t address;



    struct datosActuales {
        float xGes = 0;         //Se guarda la fuerza G obtenida usando el punto de calibracion 0g y la lectura analogica
        float yGes = 0;         //Se guarda la fuerza G obtenida usando el punto de calibracion 0g y la lectura analogica
        float zGes = 0;         //Se guarda la fuerza G obtenida usando el punto de calibracion 0g y la lectura analogica   
        int xAnalog = 0;        //Se guarda la lectura analogica  de 0 a 1023
        int yAnalog = 0;        //Se guarda la lectura analogica  de 0 a 1023
        int zAnalog = 0;        //Se guarda la lectura analogica  de 0 a 1023
        int xPunto0ges = 0;     //es el punto de calibracion 0g guardado en la eeprom
        int yPunto0ges = 0;     //es el punto de calibracion 0g guardado en la eeprom
        int zPunto0ges = 0;     //es el punto de calibracion 0g guardado en la eeprom

    }lecturas;

public:
	Acelerometro(uint8_t address);

	bool calibrar();

    
	bool requestActualizarDatosLecturasActuales();

    
    String imprimirLecturasEnEstructura();

};

