#pragma once
#include <Arduino.h>
#include <SoftwareSerial.h>
#include "RetardosConMillis.h"
#include <avr/wdt.h>   			//para manejar el watch dog porque usaremos delays en algunas funciones
#include "ComandosSim808.h"
#include <Time.h>
#include "Utilerias.h"


#define ZONA_HORARIA_UTC 0
#define ZONA_HORARIA_MEXICO_ESTANDAR_CST -6
#define ZONA_HORARIA_MEXICO_VERANO_CDT -5



#define _MENOR_A_32_BYTES 0
#define _MENOR_A_64_BYTES 1
#define _MENOR_A_128_BYTES 2
#define _MENOR_A_258_BYTES 3


#define _CADENA_RETORNO_FECHAHORA_LAT_LON_ALT_VEL_DIR_PRECI_SATELL_CALIDAD 90
#define _CADENA_RETORNO_LAT_LON_VEL_DIR 91




//creare una clase que herede todo de software serial parao poder incluir nuevos metodos para manejar mi modulo
class ClaseManejoSim808//:public SoftwareSerial					// heredamos todo lo publico de la clase Software Serial
{
private:
	uint8_t pinDeApagado;
	uint8_t pinDeReset;
	long baudiosConfigurados;
	
	char respuesta[300] = {};											//separamos de la memoria un array de 150 bytes para manejar los chars que nos retorne la respuesta tendra desde el indice 0 al indice 149 el dato mas largo de retorno sera cuando consultemos las coordenadas gps tendremos 108 bytes de retorno
	
	/*
		cuando estemos en la forma de escritura de sms o de envio de datos gprs lo pondremos en false para que 
		los posibles comandos que quiera enviar otra parte del codigo llamando a enviarComandoSinEvaluar no se envien
	*/
	bool sePuedenEnviarComandos = true;										

	/*
	Este contador se incrementara en cada metodo que llame al metodo enviarComandoSinEvaluar
	en algunas ocaciones el modulo puede que se quede trabado y no tengamos ninguna respuesta en bytes del modulo
	de ser asi si tenemos un determinado numero de estos errores podria ser que este ocurriendo algo mal con el modulo
	y lo reiniciaremos. por ejemplo me paso un poco seguido cunado estaba ptracticando lo del GPRS
	*/
	int contadorErroresNoHayBytesDisponibles = 0;
	/*
	Contaremos las respuestas que nos envie el modulo con la palabra ERROR
	podria ser nuestra culpa al tratar de enviar un comando mal escrito
	pero se supone que por eso tengo definidos los comandos el el header
	entonces a veces el modulo no se si fue mi culpa pero por mas que intentaba
	conectar el gprs solo respondia ERROR a los comandos que le enviaba
	de haber muchos errores de esos lo que hacia era reiniciar el modulo y comenzaba a funcionar otra vez correctamente
	*/
	int contadorErroresRespuestasDelModuloError = 0;
	
	
	RetardosConMillis* retardo1 = new RetardosConMillis();		

	RetardosConMillis* retardoNoMasEntradasEnElBuffer = new RetardosConMillis();




	/*
		consulta la fecha y hora obtiene el gps de los satelites la cual es UTC hora universal coordinada
	*/
	time_t fechaHoraGpsUTC();



	//COORDENADAS ACCESO RAPIDO-> antes de leerlas tenemos que llamar al metodo actualizar latlongspeedacesorapido
	float latitud = 0;
	float longitud = 0;
	int velocidad = 0;
	int direccion = 0;




	//BATERIA
	byte porcentajeBateria = 0;
	int voltajeBateria = 0;



	//gprs
	String bodyDeRespuestaUltimaConexion = "";


public:
	//Constructor de mi clase, los primeros dos son los atributos que se eredan de la clase Software serial y el ultimo es capto de esta clase. se tienen que poner en orden cronologico
	ClaseManejoSim808(uint8_t pinRx, uint8_t pinTx, uint8_t pinDeApagado, uint8_t pinDeReset);	
	/*
	Para usarse sin la libreria softwareSerial, el arduino debe de tener mas puertos fisicos uart como el mega
	*/
	ClaseManejoSim808(uint8_t pinDeApagado, uint8_t pinDeReset);	
	//Polimorfismo con el metodo begin
	virtual void begin(long speed);

	


	//este metodo enviara un comando leera la respuesta y nos la retornara en un char array de 100 caracteres 
	char* enviarComandoSinEvaluar(char* comando, long tiempoEspera, char* respuestaEsperada);

	long calcularTiempoDeEsperaDelProcesador(long cantidaDeBytesEsperadosComoRespuesta);




	//Indica si el modulo Sim esta encendido
	//este metodo hace uso de enviarComandoSinEvaluar, donde el procesdor se quedara dando vultas en un bucle esperando los datos del modulo durante 200ms
	//lo que significa que cada que se llame a esta funcion se llevara 200ms
	//RETURN
	//true   si la respuesta recibida al enviar el comando AT es Ok
	//falso si no hay nada de bytes disponibles o la respuesta esperada no es OK sino otra indicando que el modulo esta apagado
	bool getEstatusModuloEstaApagadoEncendido();
	void enciendeModuloConDelays();
	void resetDelModuloDelays();

	
	
	byte getStatusGps();		
	bool activarGps();
	bool apagaGps();





	//Consulta la fehca del reloj RC del modulo 
	//la limpia y entrega un string con la fecha y hora dividida en una coma
	//Return
	//20/01/08,22:21:22-06
	String consultarFechaHoraDelModulo();




	String getLocalizacion(byte cadenaDeRetorno, int zonaHoraria);

	void actualizarLatLongSpeedAccesoRapido();
	float getLatitudRapido();
	float getLongitudRapido();
	int getVelocidadRapido();
	int getDireccionRapido();







	String getIMEI();
	bool getStatusPinSim();
	bool setPinSim(String pin = "4Digitos");

	byte getFuerzaSenal();
	bool getGestorBateriaActivado();
	bool setActivarDesactivarGestorBateria(bool tipo);
	bool actualizarInformacionBateria();
	byte getPorcentajeBateria();
	int	 getVoltajeBateria();






	time_t getDateGpsTimeZone(int zonaHoraria);
	//dd/MM/aaaa hh/mm/ss
	String getDateGpsTimeZone_toStringFechaHora(int zonaHoraria);
	//dd/MM/aaaa
	String getDateGpsTimeZone_toStringFecha(int zonaHoraria);
	//hh:mm:ss
	String getDateGpsTimeZone_toStringHora(int zonaHoraria);






	bool enviarSMS(String zonaMundial, String numeroTelefono, String contenido);













	/*
	Pregunta por el estado del GPRS
	retorna:
	True o 1 si esta activado
	False o 0 si esta apagado
	2 si hay algun error
	*/	
	byte gprsComprobarEstado();
	/*
	Activa o desactiva el GPRS
	envia True para activarlo, False para apagarlo
	Retorna True si la accion se realizo con exito
	False si ocurrio un error, o el modulo esta apagado
	*/
	bool gprsActivar_desactivar(bool estado);

	byte gprsStatusDelContextoCIP();
	
	bool gprsAbrirContextoGPRS(String apnNombreRed, String apnUsuario, String apnPassword);

	bool gprsAbrirConexionConServidor(String TCP_UDP, String ip_DominioServidorPrincipal, String puerto, byte& gestorDeErrores);

	bool gprsEnviarInformacionPOST(String dominioSecundario, String archivoDeDestino, String informacion1, String informacion2, String informacion3, String informacion4, String informacion5);

	bool gprsAbrirEnvioDeInformacionPOST(String dominioSecundario, String archivoDeDestino, long tamanoDelCuerpoDelMensaje, String info2);

	bool gprsCerrarEnvioDeInformacionPOST();

	String gprsGetBodyUltimo();

	
};

