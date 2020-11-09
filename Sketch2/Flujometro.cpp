#include "Flujometro.h"


unsigned long intervaloDeTiempo = 0;
double velLitrosPorMinuto = 0;


Flujometro::Flujometro(int numeroDePin) {
	this->numeroPin = numeroDePin;
	pinMode(numeroDePin, INPUT_PULLUP);	
}










/*
Activa y setea las interrupciones en el arduino
Si no se llama este metodo no se escucharan las interrupciones nunca
*/
void Flujometro::escucharFlujometro()
{
	//creamos el elscuchador de interrupciones, que detecte los falling, y le pasamos la funcion estatica a la que llamara cuando se reconosca la interrupcion
	attachInterrupt(digitalPinToInterrupt(this->numeroPin), Attach::aumentoPulsosFlujometroTurbinaGlp ,FALLING);
}
/*
Inicializamos el flujometro
@params
long tiempoPorCalculo	->Define cada cuanto tiempo se leeran las interrupciones calculadas para obtener la velocidad del fluido
long timeOutFinalizarCarga->Define el tiempo que deba transcurrir para dar por terminado el periodo de carga despues de que no hay mas interrupciones recibidas es decir ya no hay mas glp fluyendo
*/
void Flujometro::begin(unsigned long tiempoCadaCalculo, unsigned long timeOutFinalizarCarga) {

	//creamos el elscuchador de interrupciones, que detecte los falling, y le pasamos la funcion estatica a la que llamara cuando se reconosca la interrupcion
	attachInterrupt(digitalPinToInterrupt(this->numeroPin), Attach::aumentoPulsosFlujometroTurbinaGlp, FALLING);

	this->intervaloDeTiempo = tiempoCadaCalculo;
	this->timeOutTiempoUltimaLectura = timeOutFinalizarCarga;
	retardoFlujoDetectado.setRetardo(this->timeOutTiempoUltimaLectura);

	//this->retardoReiniciarPulsosParaFiltroDeCargasFantasmasPorVibraciones.setRetardo(2000);
}





bool Flujometro::calcularFlujometro()
{
	//detenemos la deteccion de interrupciones para que la variable estatica deje de incrementar en lo que calculamos
	detachInterrupt(digitalPinToInterrupt(numeroPin));	


	pulsosTemporales = Attach::pulsosFlujometroTurbinaGlp;	

	double interpolacionDeRpmPorMinuto = (60000 / intervaloDeTiempo) * pulsosTemporales;
	velLitrosPorMinuto = (interpolacionDeRpmPorMinuto * mililitrosPorPulso) / 1000;

	pulsosTotales += pulsosTemporales;
	Attach::pulsosFlujometroTurbinaGlp = 0;
	attachInterrupt(digitalPinToInterrupt(this->numeroPin), Attach::aumentoPulsosFlujometroTurbinaGlp, FALLING);						//activamos denuevo las interrupciones



	/*
	Cada que calculemos nuestros datos preguntaremos si la variable que cuenta llega es diferente de 0,
	si es diferente significa que sige fluyendo gas lp por el flujometro, entonces reiniciaremos el timer
	para y retornaremos true para indicar a algun otro metodo que se calcularon los datos y que ademas hay flujo de gasLp
	cuando ya no haya flujo, seguiremos retornando true hasta que el retardo se desborde es decir pasaron 10 segundos o el tiempo programado en el timer
	desde el ultimo pulso del flujometro detectado, una ves que pasen los 10 segundos retornaremos falso indicando que hemos terminado de detectar una carga de glp
	y pudioendo dejar el procesador disponible para su siguiente tarea

	si deja de haber flujo tendra 10 segundos antes de que se finalice la carga si vuelve a haber otro pulso entonces se reinicia el timer

	corregimos, nos dimos cuenta que el vehiculo 42 cuando deproton tenia mal las conexiones del glp
	lo hice entrar al bucle de carga de gas lp, y me di cuenta que aunque yo ya habia dejado de mover la turbina
	el sistema no salia del bucle de gas lp, el problema fue que dentro del bucle en cada impresion que hacia de informacion
	en un segundo se detectaban 0 pulsos y al siguiente se detectaba 1 solo pulso este pulso era ocacionado por las perturbacioens
	de la turbina o conecciones y asi continuo por un momento depronto 2 segundos marcaba 0 pulsos y  al siguiente 1 pulso
	despues 5 segundos no marcaba ningun pulso y al siguiente 1 pulso, eso hacia por logica de este programa que cuando los pulsos
	son diferentes de 0 reinicia el timer ocacionando que aunque se a una perturbacion tarde mucho mucho tiempo en 
	salir del bulce de carga, y es lo que ocaciona que por ejemplo el vehiculo 29 genere cargas de hasta 30 mminutos
	y eso mismo con otros vehiculos.

	entonces para resolver este problema vamos a cambiar la parte de abajo para que solo reinicie el timer cuando es algo logico
	que por ejemplo los pulsos detectados sean mayores a 10 o 15, los calculos se estan haciendo cada segundo
	recuerdo que cuando fuia el glp de verdad generaba pulsos de 64 cada 500ms y ya cheque un archivo flujometro  y por segundo hay 174 pulsos 120 o en solo una ocacion 50 pulsos pero solo en 1 segundo no en toda la carga
	el archivo tiene guardado informacion de 2 semanas de carga del vehiculo 29 eso nos da una buena base 
	entonces 15 o 20 pulsos incuso sera un buen filtro pero muy bajo podremos poner un filtro de 40 o 50 comodamente por segundo. de esa manera si detecta 1 pulso perdido
	no alcanzara para reiniciar el contador de glp. aun asi sidetecta depronto solo 40 pulsos tendra 15 segundos para volver a detectar un pulso alto de 40 para no detener la carga

	no nos tenemos que preocupar porque aunque sea 1 pulso se valla acomulando y al final alcance a llegar a los 20 pulosos minimos que pedimos
	puesto que en la parte de arriba siempre se reinicia a 0 la variable ATTACH
	*/

	if (pulsosTemporales > 45) {
		this->retardoFlujoDetectado.reiniciar();
		return true;
	}
	else {
		/*
		Si ya no hay flujo de glp seguiremos retornado true hasta que se desborde el timer
		*/

		if (retardoFlujoDetectado.seHaCumplido(true)) {
			this->millisFinPeriodoCargaFlujometro = millis();
			return false;
		}
		else {
			return true;
		}

	}


	






	
	




	/*Por ejemplo si tenemos un conteo de 10 rpm en 1 segundo, necesitamos saber aproximado
	  cuantas revoluciones son al minuto, pero tenemos que dividir el minuito en el tiempo de muestreo
	  un minuto son 60mil entre el tiempo de captura de rpm que es miln entonces queda de 60, multiplicamos 60 por las rpm capturadas
	  y nos da la escala del minuto
	  igual si el muestreo se toma en 400ms y se contaron 15rpm entonces 60mil/400 son 150 * 15rpm = 2250 rpm en ese minuto a esa velocidad
	  

	  Asignamos a todas las variables del objeto los valres capturados para que los puedamos obtener cuando lo deseemos
	  */


	
	
	
}



/*se tiene que llamar primero al metodo calcular flujometro por ejemplo 
en el programa principal de arduino se tiene que poner un timer por ejemplo 1 segundo cunado se cumple ese segundo llamas a
calcular flujometro, el cual parara la escucha de interrupciones, y toma el conteo que tuvo en el segundo hace los calculos
setea sus variables campos con los valores necesarios, y despues que acaba los calculos vuelve a escuchar las interrups
en ese momento despues de tu linea calcular flujometro tienes tus variables campo disponibles con la informacion actualizada para
mostrarlos en pantalla con los metodos de abajo, esto claro se hara dentro del mismo ciclo del timer, asi cuando se vuelva a cumplir
el timer de otro segundo los campos del flujometro se actualizaran con la nueva informacion
*/



/*
Retorna los pulsos que se obtubieron durante el periodo de tiempo que duro el calculo
si el calculo es cada 500ms retorna cuantos pulsos tenia la variable estatica en esos 500ms
*/
int Flujometro::getPulsosTemporales()
{
	return pulsosTemporales;
}


/*
Si hemos detectado pulsos retornamos true
En ocaciones a lo largo del dia el flujometro podria llegar a moverse y generar pulsos
estos pulsos a lo largo del dia podrian llegar hasta el minimo de este y hacer que 
entre en el bucle de deteccion de flujometro, el problema es que como tenemos 15 segundos despues
del ultimo pulso detectado para salir del bucle entonces escribiremos datos en valde.
*/
bool Flujometro::flujoDetectado()
{
	/*
	Si el contador de pulsos es mayor a 5 es decir desde el ultimo calculo del flujometro donde se reinicio a 0
	se han detectado nuevos pulsos en las interrupciones entonces retornamos true,

	*/



	if (Attach::pulsosFlujometroTurbinaGlp > 80) {
		millisInicioPeriodoCargaFlujometro = millis();
		this->pulsosTotales = 0;						//reiniciamos los pulsos totales con cada nueva carga registrada, debido a que en esta variable estaran los litros totales que se cargaron pero en la anterior carga
		return true;
	}
	else
	{
		/*
		Tenemos un problemita, ocurre que con las vibraciones del vehiculo durante el dia de trabajo la elice del flujometro depronto gira
		eso hace que el arduino detecte 1 pulso por ejemplo, el cual guarda en su memoria, cuando hay alguna otra vibracion vuelve a girar
		la elice y manda otro pulso de tal manera que ahora arduino tiene 2, eso ocurre en varias ocaciones durante el dia, cuando en el loop
		se llega a la parte donde se pregunta flujo detectado al comparar los 2 conteos que tiene pues retorna false, y no se genera la carga de gasLP
		pero con forme mas vibraciones van pasando llega el momento en que alcanzamos los 10 conteos minimos que pedimos en ese momento se registra una carga de combustible
		y sabemos que es por eso porque justo apenas la informacion que llega al servidor registra 11 pulsos totales, o 13 pulsos o 12 pulsos, y detecta mas cargas continuas
		cuando el carro esta en terraseria o en brechas de las rutas, en carreteras como autopiztas o carreteras principales no detecta las cargas por las vibraciones.

		Entonces para corregir esto, sabemos que cada loop del programa debido a las tareas que tiene que hacer el microcontrolador antes de llegar al punto que pregunta si hay flujo
		detectado tardara un poco de tiempo, en algunas ocaciones tardara 2,3,4,5 segundos en hacer unas tareas como enviar los datos de la SD, PERO cuando en ese loop no toca hacer ninguna tarea
		la vuelta del loop se realisa en milisegundos o menos, y esas ocaciones que el procesador no tiene nada que hacer lo podemos ver porque imprime en el monitor serial los segundos, pero claro ahi lo 
		imprime segundo a segundo porque tiene un temporizador que lo deja imprimir cada segundo, como aqui no tenemos temporisador entonces estamos leyendo casi inmediatamente el flujoDetectado,

		el plan es hacer que a esta funcion flujo detectado, no se entre aunque el procesador no tenga nada que hacer en un minimo de 2 segundos, eso lo lograremos añadiendo un timer, y cuando esos 2 segundos se cumplan
		lo que aremos sera reiniciar la variable pulsosFlujometroTurbinaGLP a 0, y como lo estamos haciendo en este else, damos por hecho que no se cumplio con el minimo de 10 pulsos para retornar verdadero

		por lo tanto al hacer esto logramos que cuando esten ocurriendo vibraciones momentaneas que hacen girar la turbina de gasLp, y aumenta el contador, al cumplir los 2 segundos y ver que el conteo no llega al minimo
		para decir que hay un flujo de gas entonces reiniciamos a 0, de esta manera aunque halla mas vibraciones futuras, como estamos reiniciando a 0 cada 2 segundos no se llegara al minimo para registrar la carga
		y asi desapareceran las cargas fantasmas, ahora porque 2 segundos yo creo que incluso podria ser mucho menos, ocurre que cuando hay un flujo real de gasLp, tendremos cerca de 64 pulsos cada 500ms, es decir 
		en 2 segundos tendriamos 256 pulsos reales registrados, incluso en el momento en que enchufan la mangera al carro, cuando se nivela la presion en ese momento la turbina gira en un pequeñisimo momento muchas vueltas
		entonces ya esta, incluso creo que podriamos subir las 10 vueltas minimas que le estamos pidiendo a 20 o quizas mas, y asi evitar mas cargas fantasmas. como cada 2 segundos se reiniciara a 0, tiene suficiente tiempo
		para que el flujo del gas eleve los pulsos al minimo requerido, oviamente si no lo hace significa que lo que se esta registrando son vueltas en la turbina debido a vibraciones del camino

		Le ponemos que sea mayor a 0 para que no este imprimiendo cada 2 segundos en la pantalla ese menesaje

		30-07-2020 actualizamos los pulsos minimos a 80 y bajamos el tiempo a 1 segundo, para tratar de corregir el problema que esta pasando con el 29
		envia muchas cargas e incluso muchas veces se queda detenido en un bucle de 20 o 30 minutos registrando carga
		*/

		if (this->retardoReiniciarPulsosParaFiltroDeCargasFantasmasPorVibraciones.seHaCumplido(true) && Attach::pulsosFlujometroTurbinaGlp > 0) {
			Serial.print(F("\n\n\n\nPulsos detectados en memoria-->\t")); Serial.println(String(Attach::pulsosFlujometroTurbinaGlp));
			Attach::pulsosFlujometroTurbinaGlp = 0;
			Serial.println(F("------FlujoDetectado, no se han superado mas de 80 pulsos en 1 segundos reiniciamos a 0 los pulsos detectados, debido a que podrian ser vibraciones las causantes de los pulsos de la turbina----\n\n\n\n"));
		}

		return false;
	}
}


/*
Retorna la velocidad obtenida del fluido en el periodo de calculo
si el tiempo de calculo es cada 500ms, te indica a que velocidad paso el fluido
durante esos 500ms
*/
double Flujometro::getLitrosPorMinuto(){
	return velLitrosPorMinuto;
}


/*
Retorna, el total de los pulsos detectados durante el periodo de carga
si cada 500ms se calculaba, los pulsos de esos 500ms se sumaban en esta variable
si el periodo de carga tardo 60 segundos en esta variable tendremos el total de pulsos
de esos 60segundos.
La variable se debera reiniciar a 0 con el metodo reiniciarValoresTotales
antes de iniciar un nuevo periodo de carga
*/
long Flujometro::getPulsosTotales()
{
	return pulsosTotales;
}


/*
Es necesario reiniciar la variable pulsos totales en el inicio de un nuevo periodo de carga
*/
void Flujometro::reiniciarPulsosTotales()
{
	pulsosTotales = 0;
}


/*
Retorna el valor total de litros cargados en el periodo de carga
no es necesario reiniciar esta variable debido a que solo calcula
de los pulsos totales se multiplica por los millilitros y devuelve los litros
es decir mientras se reinicie eliminar pulsos totales, este metodo multiplicara por 0
y devolvera 0 claro esta xD
*/
double Flujometro::getLitrosTotales()
{
	return (pulsosTotales * mililitrosPorPulso) / 1000;
}

/*
Nos entrega desde el momento en que fueron detectadas las primeras revoluciones con el metodo flujo detectado hasta
que se detectaron las ultimas revoluciones. nos da los segundos de flujo
*/
int Flujometro::getPeriodoCargaReal()
{
	long calculo = this->millisFinPeriodoCargaFlujometro - this->millisInicioPeriodoCargaFlujometro;
	calculo -= timeOutTiempoUltimaLectura;
	int periodo = (int) (calculo / 1000);
	return periodo;			//le quitamos el tiempo que espera para salir del periodo de carga y asi obtener los segundos reales de carga
}

/*
Por si necesitas saber cada cuanto tiempo esta calculando el flujo si cada 500ms o cada 1000ms
*/
long Flujometro::getIntervaloTiempo()
{
	return this->intervaloDeTiempo;
}




/*
Retornamos los datos del acelerometro del ultimo calculo en String
pulsosTemporales,LitrosPorMinuto,PulsosTotales,LitrosTotales,intervaloCalculo,TipoRenglonTemporal_FInal,Lat,long,fec,hr
59,12.11,351,0.60,500,t
retornaremos estos datos por si quieres imprimir en la memoria sd un renglon por cada calculo que haga el flujometro por ejemplo cada 500ms
*/
String Flujometro::temporalesToString()
{
	return String(getPulsosTemporales()) + "," + String(getLitrosPorMinuto()) + "," + String(getPulsosTotales()) + "," + String(getLitrosTotales()) + "," + String(getIntervaloTiempo()) + ",T";
}

/*
Retornamos los datos del flujometro finales, este se puede llamar al finalizar el periodo de carga para que entrege solo los resultados finales
pulsosTemporales,LitrosPorMinuto,PulsosTotales,LitrosTotales,duracionPerido,TipoRenglonTemporal_FInal
0,0,1954,3.34,7,F
*/
String Flujometro::totalesToString()
{
	return String(0) + "," + String(0) + "," + String(getPulsosTotales()) + "," + String(getLitrosTotales()) + "," + String(getPeriodoCargaReal()) + ",F";
}


