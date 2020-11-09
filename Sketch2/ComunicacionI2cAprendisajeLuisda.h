#pragma once
#include <Wire.h>

/*en esta clase pongo mis aporendisajes y pruebas con el i2c para comunicarse entre arduinos deecho no hago archivo cpp lo defino en cabecera
porque solo lo quiero como una guia de lo que aprendi, el codigo es funcional toma y pega de aqui lo que necesites y funcionara claro haslo bien

HAY OTRA LIBRERIA DESCRITA EN EL SEGUNDO LINK QUE DICE I2C_Anything.h y parese que es de ayuda para enviar cualquier tipo de parametro
mas facil que usando la libreria Wire que como vemos es un poco dificil al enviar diferentes tipos de datos mayores a un byte
*/
//http://kio4.com/arduino/30conectardosarduinos.htm  //el primer ejemplo que vi
//http://www.gammon.com.au/forum/?id=10896  //como enviar enteros y mas informacion valiosa y por ejemplo onRequest que es cuando lees info del esclavo,
//cuando tu usas write el esclavo debe tener la funcion onreceibe es decir escuchara cunado recibe datos del maestro, el esclabo esta siendo escrito
//cuando el maestro le solicita informacion al esclavo el esclavo tiene su funcion Onrequest, que escucha si esta siendo leido por el maestro que le solicita datos
//y para eso el maestro en lugar de enviarle write le envia un Wire.requestFrom (SLAVE_ADDRESS, responseSize); ver la segunda referencia hay muchisimos ejemplos para entender
//comenzamos la transmision con el dispositivo 1

//segun lei el protocolo 12c es el mas usado para compartir iformacion entre microprosesadores en una misma placa en la induistria
//claro esta el Uart y el SPi pero segun el i2c es mejor para esa tarea por su capacidad de tener hasta 112 dispositivos esclavos
//con solo dos cables.

//https://www.luisllamas.es/estructuras-puerto-serie-arduino/ 
//para estructuras por favor leer esto esta genial! lo aplique solo en los request es decir cuando el master le solicita info al esclavo
//pero claro sera igual de excelente para los onReceive, es decir cuando el master le envia info al esclavo todo se facilito con el envio de estructuras!
//y tambien esta funcionando para los floats int etc, solo no funciono para los String con S mayuscula, los 2 arduinos se morian, porque por ejemplo
//el sistema indicaba que los caracteres eran 24 pero el peso en bytes solo 6, entonces no supe cual de los 2 valores usar para el envio por referencia
//si enviaba con 6 el mensaje iba mal decodificado y si lo enviaba ocn 24 se morian.
//creo que el problema es la variable String, funcionaria si la enviamos pero string es decir un charArray, pero como vemos en los ejemplos de 
//la estructura y el float enviado por referencia, en el arduino que recibe debemos de tener la estructura o la variable ya definida,
//entonces deberiamos de tener el char array ya definido con una meidda en especifico, y el string podria ser variable, como sea lo unico que nos 
//ahorrariamos al hacerlos asi es leer el bucle while byte a byte para añadirlo al String, y apuesto que se puedfe hacer asi porque al poder elviar
//una estructura y el float por referencia se pueden enviar arrays porque claro las estructuras son similares a los vectores, que son los arrays.
//solo abria que ver si podemos o definir de manera dinamica el array o simplemente quedarnos con la manera que ya tenemos de enviar los String
//al igual me causa duda el envio de estructuras que contengan dentro Strings no creo que valla a funcionar, poruqe el string se codifica cada caracter en un byte
//quizas si la estructura tuviera un char array pero seria el mismo detalle se tiene que declarar a una medida estable.
//hare pruebas y las confirmare

//OKAY crei con los metodos principales claro que se volvia dificil enviar floats, pero ahora con el ejemplo de las estructuras hise lo mismo
//con valores cualquiera, parece que la manera es usar Wire.readBytes y enviar las variables por referencia!!!
//asi podemos enviar cualquier valor que queramos al otro arduino!!!

//no olvidarse de poner las resistencias pull up en la linea de scl y sda, la resistencia ponerla de 10k cada una a cada linea
/*----------------------5v-------------------------------alimentacion
		|			|
		|			|
		2			2
		.			.
		2			2
		k			k	
		|			|
		|			|
	----SCL---------|---------------------------------- señal de reloj serial clock
					|
					|
					|
				-------------------SDA----------------- Señal de datos serial data



	________________________________________________________ GND la deben de compartir todos los dispositivos	


	Si no pones resistencia pull up la onda es muy fea xD no es ni cuadrada
	vi las ondas en el osiloscopio del reloj y de la cominicacion y la verdad que no se paresen en nada a cuadradas
	en el mismo link de arriba los chicos mencionan lo mismo que a mi, entonces si bajamos las resistencias 
	a 2.2k o 1k las ondas son casi cuadradas creo eso sera mejor, lo malo es que se incvrementara el consumo de los
	chips el unico detalle es que entre mas pequela es la resistencia la onda cuadrada es mejor
	pero cuando esta en los puntos de 0 voltios la linea se despega del 0 cada vez mas entre mas pequeño es el valor de la resistencia
	es decir, cuando el reloj o el arduino manda datos envia un 0 en el tren de pulsos, en ese pulso deberia llegar a en un mundo ideal a 0v
	pero en el caso de 1k llega a 251mv en el caso de 2.2k 188mv y con 10k son -62.7mv o 0v pero la ya no es una honda cuadrada para nada
	creo que pondre la de 2.2k la onda es casi cuadrada y estamos muy cerca del 0
*/

class ComunicacionI2c
{


public:

	/*
	maestro enviando datos a esclavo con escritura
	recuerda que se debe poner la instruccion en el setup del maestro

	void setup{
	    Wire.begin();           //inicializamos el puerto i2c o lo unimos como sea xD como este sera maestro no ponemos direccion, si quremos por ejemplo multimaestros tenemos que poner la direccion de este arduino aqui, para que primero se comunique con el esclavo y luego el esclavo se haga maestro y se comunique con el
	}

	y ya dentro de tu void loop llamas para iniciar la transmicion en la parte de codigo que uqieras hacerlo
	 antes de comenzar la transimision 


		Wire.beginTransmission(1);		//le pones la direccion del esclavo que recibira la info, el master esta escribiendo
		

		Wire.write("hola esclavo");
		Wire.write("hello");		//puedes enviar varios write estos bytes le llegaran al esclavo en una sula transmicion

		//para terminar la transmicion

		if (Wire.endTransmission() == 0) {
			Serial.println("La transmision fue exitosa");
		}
		else {
			Serial.println("Revisa la direccion del esclavo la transmision fracaso");

		}
	*/

	static void enviarString() {
		//-----------------------------------------------

		//https://forum.arduino.cc/index.php?topic=239808.0 // como enviar strings y no Strings
		//String hola = "340,gasLp,90c,30.5litros,quetaldaffdadf";      //39 caracteres
		String hola = "340,gasLp,90c,30.5litros";      //24 caracteres

		//Serial.println("Escribiendo en i2c " + String(sizeof("340, gasLp, 90c, 30.5litros")));



		//si envias un string cada caracter se codifica en un byte, se envia caractrer por caracter, y te indica la lin
				//Wire.write("id.a2");       //pone en el bufer, a la vez separa del bufer un maximo de 32bytes es lo maximo que se puede enviar en una sola transaccion
				//Wire.write("Hola");         //puedes escribir e enviar 2 o mas veces siempre y que no superes los 32 bytes por transmision
				//Wire.write("340, gasLp, 90c, 30.5litros1515151515151515");  //por ejemplo aqui supere los 32 bytes y en el esclavo solo recibimos los primeros 32 bytes



				//para enviar strings, aun no podemos enviar la variable String creada anteriormente solo pasandole el texto directo al metodo write
				//Wire.write("340, gasLp, 90c, 30.5litros");
				//Serial.println("Escribiendo en i2c " + String(sizeof("340, gasLp, 90c, 30.5litros")));

		//intentare enviarselo como referencia al string okay la referencia no funciona
		//Wire.write(&hola);

		//parese segun esto que un String con S mayuscula es la clase de String como cualquier otra ej jav apro ejemplo
		//pero arduino maneja string con s minuscula, que para el es un array de caracteres, entonces debemos convertir nuestro
		//String con S mayuscula a un string con minuscula, es muy similar a la parte donde enviamos un float, lo que creamos 
		//es un vector donde ponemos los caracteres
		//primero conocemos el tamaño del string para asi crear el array con esa medida, pero no podemos pasarle una variable al array y ya
		//para definir el tamaño en tiempo de ejecucion debemos usar punteros //https://www.lawebdelprogramador.com/foros/Dev-C/622083-definir-arrays-en-tiempo-de-ejecucion-y-como.html
		//si tratas de definir un array como siempre sin punteros char array[aqui le metes una variable del tamaño] el compilador te dira que no puedes usar una variable que tiene que ser constante para definir el tamaño

		int medida = hola.length(); Serial.println("Cantidad de caracteres del String " + String(medida));


		//esto es para hacer definir el tiempo de ejecucion el tamaño del array, pero por lo visto no es tan necesario porque la funcion to charArray lo redefine
		//char* my_string_con_minuscula;                // puntero que apuntara al array creado dinamicamente, recuerda que todo array o vector en c++ es un puntero automaticamente
		//my_string_con_minuscula = new char[medida];   //creamos el nuevo objeto por referencia aqu se reserva de la memoria ram un arreglo de la medida que queramos de chars o int lo que quieras

		char my_string_con_minuscula[32];
		hola.toCharArray(my_string_con_minuscula, medida);


		Serial.println("Medida del vector my_string_con_minuscula " + String(sizeof(my_string_con_minuscula)));
		//no se si deberia ser la misma con la que se creo es decir con la longitud del string, la medida que nos da es con la que se creo en su cosntante,
		//cuando la creo con su constante y no dinamica con puntero nos da la medida del vector correctamente pero con los punteros da 2 no se porque
		//o si con el numero 8 que se le pasa al metodo que convierte el string hola al char array.
		//no se porque nos dice que la medida del vector es 2 aunque yo ponga manualmente 28 sigue siendo 2, 
		//al hacerlo mas pequeño con 2 el esclavo solo recibe 3 bytes 1 y 2 correctos 3 dañado
		//si se define con la medida del string se solo llegan 7 bytes me imagino que es por el 8 que esta en la funcion
		//toCharArray, entonces creo que esa funcion lo que hace es redefinir el array si eso es cierto no necesitaria 
		//crearlo dinamico con punteros? y solo ponerle el macymo tamaño de 32 bytes?ya que quizas esa funcion lo redimenciona
		//asi es, cree una nueva definicion del array sin ser dinamico declarandolo con un tamaño de 2 o incluso 0 y al esclavo le siguen llegando los 
		//7 bytes me imagino que entonces si se redimenciono 
		//okay perfecto funciono puedes crear el array fijo con cualquier medida, cuando llamas a toCharArray y le pasas el array, el lo 
		//me imagino que crea una nueva instancia que con la meidda que le pasas, cuando pasabamos 8 enviaba 7, ahora lo que hice fue
		//enviarle la medida del string que ibamos a enviar escon el .leght es decir en este ejemplo 28 y mando solo 27 bites el mesnaje
		//se recibio completo excepto que faltaba la s final, es decir hay que enviare el tamaño + 1 para que quepa el mensaje completo

		//importante le agrege mas caracteres al string y esta muriendo arduino se reinicia pero no entiendo porque se muere en la linea donde se imprime
		//el tamaño del string en el puerto serie. okay el problema al parecer es al llamar al renglon tocharArray si comento esa linea marca el tamaño bien 
		//y no se muere pero obio no lo podremos enviar. ahora si antes el array estaba como logitud 1 lo cambie a longitud 40 y ahora no se murio
		//y se envio el mensaje aunque claro solo le llegaron 32 bytes es decir escribio hasta 340,gasLp,90c,30.5litros,quetald
		//quizas para esto si sera bueno definir el vector dinamico con los punteros que ya puse del ejemplo arriba, o quizas baste solo con definirlo
		//a su maximo de 32 de tamaño aunque sea fijo y validar que la variable medida no exceda los 32, aunque no se si quizas podraimos desperdiciar
		//un poco de memoria del arduino porque se supone que reservamos 32 bytes de la ram, y quizas si el mensaje es mas corto nos convendria 
		//no reservarla toda. no lo se lo dejamos a criterio. cuando la defini como 32 aunuqe el mensaje fuera de 39 trabajo bien
		//obio solo llegaron 32 bites al esclavo
		//si defino el tamaño del array con 32, el tamaño me indica los 32 pero el mensaje solo es de 24, el 24 lo pasamos a la funcion toCharArray
		//y al esclavo solo le llegan 23 bytes, es decir el tamaño del arreglo al parecer sigue siendo 32 o se separo 32 bytes de la memoria cuando se creo
		//pero al final de alguna manera solo le llegaron los 23 bytes al esclavo, es decir quizas se redimenciono, porque si no me imagino
		//que al esclavo le llegarian los restantes bytes de 32 vacios pero los marcaria como disponibles o no se xD
		//pero asi funciona muy bien!! ya sea con punteros o longitud fija del macimo de 32.



		//ahora si enviamos el string con s minuscula xD que es un vector de caracteres para arduino si quieres enviar un mensaje fijo, pues lo escribes dentro
		//del Wire.write("Hola este es un id"), si quieres armar el mensaje con variables tienes que convertirlo a char array, la primera opcion
		//creo que seria util como para enviar id que concuerden con cierta info que se recibira, por ejemplo primero escribes 
		//Wire.write("idRpm1");
		//Wire.write(valor);    le enviamos el valor que sea jaja aqui lo resumi porque claro hay que enviar ese valor si es entero o float o string etc
		//entonces asi podriamos recibir mucha info por partes sin necesidad de unirla todo en un string quizas y saber como vamos a tratar esa info
		//dependiendo de su id, quizas aun no lo pruebo yu pienso bien pero podria ser una buena opcion

		Wire.write(my_string_con_minuscula);


		//creo que en el esclavo lo recibiremos como si fuera un String enviado directamente al write ("hola este es el mensaje")
		//y asi es funciono bien!

		//-----------------------------------


	


	};
	

	static void enviarInt() {
		//---------------------------------------
		//para enviar enteros de mas de 2 bytes debemos partirlo enviar primero el bit mas pesado y luego el menos pesado, 
		/*

		el escalvo debera rearmarlo con:

						void receiveEvent (int howMany)
				 {

				 // we are expecting 2 bytes, so check we got them
				 if (howMany == 2)
				   {
				   int result;

				   result = Wire.read ();
				   result <<= 8;
				   result |= Wire.read ();

				   // do something with result here ...

				   // for example, flash the LED

				   digitalWrite (LED, ledVal ^= 1);   // flash the LED

				   }  // end if 2 bytes were sent to us

				  // throw away any garbage
				  while (Wire.available () > 0)
					Wire.read ();

				  }  // end of receiveEvent





		*/

		int revoluciones = 3545;
		Serial.println("Escribiendo en i2c " + String(sizeof(revoluciones)));
		Wire.write(highByte(revoluciones));
		Wire.write(lowByte(revoluciones));
		//--------------------------------


	};

	static void enviarFloat() {
		//--------------------------
		   //Para enviar Floats estos ocupan 4 bytes hay que convertirlos a su representacion en string
		   //https://medium.com/@sandhan.sarma/sending-floats-over-i2c-between-arduinos-part-1-4e333d8ca578


		float temperatura = 10.595;
		char temperaturaBufer[4];
		Serial.println("Escribiendo en i2c " + String(sizeof(temperatura)));

		dtostrf(temperatura, 1, 2, temperaturaBufer);       //lo convertimos a su reprecentacion string leer mas acerca de este metodo el 7 y 2 no se para que sean, el 7 creo que indica el tamaño porque el esclabo dice que tenemos 7 bytes disponibles puse 4 y baajo a 5 disponibles puse 1 y 2 y se quedo en 5 disponibles el esclavo recibe bien la informacion
		Serial.println("Tamano bufer " + String(sizeof(temperaturaBufer)));
		Wire.write(temperaturaBufer);
		//-------------------------------


	};


	//AQUI USO LA MISMA MANERA QUE CON LAS ESTRUCTURAS, PARECE QUE ES MAS FACIL
	static void enviarFloatPorReferencia() {
		if (retardoWire.seHaCumplido()) {


			Wire.beginTransmission(1);








			float dato = 10.95;
			Serial.println("Medida del string en bytes " + String(sizeof(dato)));


			Wire.write((byte*)&dato, 4);





			if (Wire.endTransmission() == 0) {
				Serial.println("La transmision fue exitosa");
			}
			else {
				Serial.println("Revisa la direccion del esclavo la transmision fracaso");

			}





		}
	}

	//TRATE DE APLICAR LO MISMO PARA ENVIAR STRINGS PERO CREO NO FUNCIONO SE MUEREN LOS ARDUINOS creo que va a ser enviarlo igual por referencia
	//pero en forma de char array la cuestion es que en el que recibira la info tiene que definirse el array de manera dinamica
	//poruq eno sabemos la medida del string que enviaremos entonces creo que ya es suficioente con el metodo de enviar strings de arriba esta bien
	//Los datos mas dificiles de enviar eran los int y los float y long etc parece que con referencia quedaron bien
	static void enviarStringPorReferencia() {
		if (retardoWire.seHaCumplido()) {


			Wire.beginTransmission(1);



			String hola = "340,gasLp,90c,30.5litros";      //24 caracteres tamaño de 6bytes

			int medida = hola.length();
			Serial.println("Cantidad de caracteres del String " + String(medida));
			Serial.println("Medida del string en bytes " + String(sizeof(hola)));


			Wire.write((byte*)&hola, 6);





			if (Wire.endTransmission() == 0) {
				Serial.println("La transmision fue exitosa");
			}
			else {
				Serial.println("Revisa la direccion del esclavo la transmision fracaso");

			}





		}
}

	

	/*
	y al finalizar el envio de datos con esos metodos bueno que obvio los peudes escribir directo en el sketch solo los guarde en esos metodos para
	poder guardarlos ordenados y poder verlos cuando los necestie

		if (Wire.endTransmission() == 0) {
			Serial.println("La transmision fue exitosa");
		}
		else {
			Serial.println("Revisa la direccion del esclavo la transmision fracaso");

		}

	aqui terminamos la transmicion el protocolo le avisa que termino de enviar la info para que el esclavo deje de escuchar y sepa que ya terminaste de 
	enviar los datos
	*/


	//ahora como enviar estructuras!!
	//DIOS MIO EL MANEJO DE ESTRUCTURAS ES GENIAL!! todo se esta recibiendo perfectamente solo no la he probado con estructuras que contengan Strings no se si funcionara quizas no por la longitud variable de los String
	//quizas si un charArray fuera elemento de la estructura, el detalle es que tendria que estar definida al tamaño del string podria funcionar
	//asi o enviando los sttring por separado porque igual estamos limitados a 32bytes de transmision por comunicacion entonces si envias en una estructura 
	//un string este no tendria que ser tan largo porque ya con una palabra te estas consumiendo 10bytes y para enviar las demas variables en la estructura no
	//tendriamos espacio!!!
	//puedes enviar dentro de la estructura cualquier dato int floan string etc y al recibirlo se recibe perfectamente
	//LO MISMO APLICA PARA LOS DEMAS DATOS FLOAT INT ETC CON ESTO YA PODEMOS ENVIAR CUALQUIER DATO SIN COMPLICARLA DEMACIADO!!











	//ESCLAVO
	/*en el esclavo claro tambien se incluye la libreria Wire.h el esclavo es otro arduino, con un programa diferente 
	para demostrar que se pueden comunicar ambos, lo mejor es mantener la estructura un solo maestro muchos esclavos
	pero de ser necesario el esclavo puede ser maestro, esto dependera de los metodos que llames en cada uno
	por ejemplo si al maestro le defines una direccion en el wire.begin, y el primero escribe a un esclavo
	el esclavo recibe todo por onReceive, despues en alguna parte el esclavo le escrbie al maestro igual por Wire.write
	entonces en ese momento el esclavo pasa  a ser maerstro, y el maestro esclavo, el que antes era maestro debera tener
	su registro onreceibe donde tratara los datos. o lo mejor seria que solo sea un maestro, el maestro tiene control de 
	cuando solicitar datos o escribir datos al esclavo, si quiere recibir datos del esclavo se llama al requestFrom
	de esta manera solicita datos a un esclavo pero solo cuando el maestro quiere, y el esclavo recibe una interrupcion 
	que debe estar registrada en la funcion onRequest para enviarle los datos que el maestro manejara.
	asi el esclavo no puede comunicarce con el maestro solo cuando el maestro se lo pida, pero si usas los write
	el esclavo puede comunicarce cuando quiera con el maestro porque el maestro tendria un metodo onreceive que basicamente
	lo vuelve esclavo*/

	//entonces para el esclavo para recibir informacion con del maestro
	/*
			void setup() {

		  Wire.begin(1);    //esta es la direccion de este esclavo

		  Wire.onReceive(receiveEventString);       //llamamos a la funcion que se ejecutara cuando recibamos una peticion del master de que nos esta escribiendo o enviando datos, son interrupciones entonces el arduino las estara escuchando siemrpe
													//aunque la funcion tenga parametro el receive string no se ponen los parentecis es como en el attachInterrupt cuando registras interrupciones solo se pone el nombre de la funcion
													//aun no he probado si funcione con objetos o si tendra el mismo problema del attachInterrupt que no le peudes pasar miembros de una clase, sino tienen que ser o estaticos o definidos
													//directamente en el void setup, ir a la clase Atach que hice y al flujometro para entender esto que explico
			Serial.begin(9600);						//lo uso para monitorear los datos
		}


			void loop() {

			//contamos los segundos para que sepamos que no se a trabado el arduino
			  if( millis()> tiempo + 1000){
				segundos++;
				Serial.println(String(segundos));
				tiempo = millis();
			  }


			}



			//y aqui defines tu funcion que va a manejar el onreceive como las que puse abajo
			receiveEventString(int cuanto){bla bla bla bla bla bla}
	
	*/

	/*Darce cuenta que estas funciones quedan registradas en el setup, pero bueno solo una de ellas
	no se si podrias elegir cual recibir en tiempo de ejecucion o dependiendo de que tipo de dato va a entrar
	quizas no, quizas dentro de la funcion que hagas debes de prepararla para recibir los datos que probablemente
	recibirias y no en funciones independientes, aqui lo hago para ordenar y entender como funcionan
	notar tambien que en las 2 primeras funciones uso el available para saber cuantos datos estoy recibiendo

	
	pero en las otras dos le pongo como parametro de la funcion un int, ese parametro al parecer se pasa de manera
	implisita a la funcion cuando la registras y se reciben los datos y ya asi en ese parametro sabemos
	cuantos datos estan entrando. para ya no usar a cada rato el available.

	NOTA: en el ide de arduino no pasaba nada si yo ponia en el onReceive la funcion con el parametro vacio
	como el receiveString que no le declare ningun parametro y en el ide de arduino funcionaba
	en visualStudio ya marca un error y por fuersa la funcion que le pases al onReceive ya debe ser una funcion
	que espera un parametro
	
	
	aunque es bueno usarlo
	por ejemplo para demostrar que los datos se estan jalando correctamente. porque por ejemplo
	los datos al inicio te dice que son 7 bytes, y con forme vas leyendo cada uno se van bajando a 6 o 5 o 4 etc
	hasta que acvabas de leerlos todos en el bocle. Es bueno demostrarlo aqui porque por ejeplo si tu en ningun momento
	llamas al metodo read, esos bytes siempre siguen estando disponibles y el arduino no recibira ningun otro dato 
	que le llege del maestro proque su buffer esta lleno por asi decirlo, entonces con read cuando lees el dato
	lo vacia del available, por eso hay en algunos casi al final una sentencia while que dice trow away any garbage
	es decir tiramos cualquier basura, que es recorrer todos los bytes abailables que pudieran quedar para llamar a la sentencia
	read y que el buffer se vacie de cualquier cosa que pudieramos haber olvidado para volver a jalar nuevos datos*/


	//esta funcion trabaja en el ide de arduino pero en visual studio ya no por fuersa debe tener el parametro como los 2 ultimos
	void receiveEventString() {

			// Función que se ejecuta siempre que se reciben datos del master
			// siempre que en el master se ejecute la sentencia endTransmission
			// recibirá toda la información que hayamos pasado a través de la sentencia Wire.write
		

			String mensaje;

			// Si hay informacion recibida
			//int bytesRecibidos = Wire.available();

			//si por ejemplo se envio un string cada caracter se codifico en 1 byte, dependiendo del numero de caractreres
			//seran los bytes recibidos, cuidad con los caracteres especiales como ° usan 3 bytes y no se reciben bien
			Serial.print("Bytes disponibles: " + String(Wire.available()));
			Serial.print("\t");


			while (Wire.available()) {
				Serial.println("Available bucle: " + String(Wire.available()));

				char c = Wire.read();
				Serial.println(c);
				mensaje += c;

				//aqui recibiremos en cada byte un caracter codificado, lo sacaremos y lo guardaremos en la variable char,
				//que luego la añadiremos al mensaje compelto
				//solo podremos recibir string de 32 caracteres de largo cada comunicacion que hagamos


			}

			Serial.println();
			Serial.print("Mensaje completo " + mensaje);
			Serial.print("\t");
			Serial.print("Available: " + String(Wire.available()));
			Serial.println(" ");



		


	}

	void receiveEventRecibo2OmasBytes() {
		//esta no se porque la hice jaja me hice bolas creo que es la miosma del string

		// Si hay informacion recibida
		//int bytesRecibidos = Wire.available();

		//si por ejemplo se envio un string cada caracter se codifico en 1 byte,
		//dependiendo del numero de caractreres seran los bytes recibidos, cuidad 
		//con los caracteres especiales como ° usan 3 bytes y no se reciben bien
		Serial.print("Bytes disponibles: " + String(Wire.available()));
		Serial.print("\t");


		while (Wire.available()) {
			Serial.println("Available bucle: " + String(Wire.available()));

			byte c = Wire.read();
			Serial.println(String(c));



		}

		Serial.println();
		Serial.print("\t");
		Serial.print("Available: " + String(Wire.available()));
		Serial.println(" ");



	}


	void receiveEventReciboUnInt(int cuantos) {


		// Si hay informacion recibida
		//int bytesRecibidos = Wire.available();

		//si por ejemplo se envio un string cada caracter se codifico en 1 byte, dependiendo del numero de caractreres seran los bytes recibidos, cuidad con los caracteres especiales como ° usan 3 bytes y no se reciben bien
		Serial.print("Bytes disponibles: " + String(cuantos));
		Serial.print("\t");

		//estamos esperando 2 bytes porque partimos el int en 2 bytes

		if (cuantos == 2) {
			int enteroRecibido;

			enteroRecibido = Wire.read();   //leemos el primer byt de los 2 disponibles y por lo tanto solo nos quedara 1
			enteroRecibido <<= 8;
			enteroRecibido |= Wire.read();  //leemos el ultimo byt disponible y se lo añadimos

			Serial.println("El entero recibido fue: " + String(enteroRecibido));
		}



		// throw away any garbage ------------------
		while (Wire.available() > 0)
			Wire.read();



		Serial.println();
		Serial.print("\t");
		Serial.print("Available: " + String(Wire.available()));
		Serial.println(" ");



	}

	
	void receiveEventReciboUnFloat(int cuantos) {


		Serial.print("Bytes disponibles: " + String(cuantos));
		Serial.println("\t");

		//estamos esperando 4 bytes porque partimos porque es un vector de float de 4

		String datosFloat = "";



		while (Wire.available()) {

			char c = Wire.read();
			datosFloat += c;
			Serial.print("Imprimimos c: " + c);
		}


		float temperatura = datosFloat.toFloat();


		Serial.println();
		Serial.print("Mensaje completo " + datosFloat);
		Serial.print("\t");
		Serial.print("Float recibido " + String(temperatura));
		Serial.print("\t");
		Serial.print("Available: " + String(Wire.available()));
		Serial.println(" ");



	}

	//RECIBIMOS EL FLOAT POR REFERENCIA!
	void receiveEventFloat(int bytesEntrantes) {

		float dato;


		//si por ejemplo se envio un string cada caracter se codifico en 1 byte, dependiendo del numero de caractreres seran los bytes recibidos, cuidad con los caracteres especiales como ° usan 3 bytes y no se reciben bien
		Serial.print("Bytes disponibles: " + String(Wire.available()));
		Serial.print("\t");
		delay(100);
		Wire.readBytes((byte*)&dato, 4);



		Serial.println();
		Serial.print("Mensaje completo " + String(dato));
		Serial.print("\t");
		Serial.print("Available: " + String(Wire.available()));
		Serial.println(" ");



	}


	//llamado cuando el master nos esta escribiendo o enviando informacion no funciona
	void receiveEventStringPorReferencia(int bytesEntrantes) {

		String mensaje;


		Serial.print("Bytes disponibles: " + String(Wire.available()));
		//aqui me marcaba los bytes que recibiamos que nos enviaba el maestro
		//pero creo lo raro es el maestro indica que tiene el string una medida en bytes de 6 y en caracteres 24
		//entonces cual usaba? intente con las 2, cuando recibia 6 byts el mensaje venia decodificado mal,
		//y cuando enviaba 24 los arduinos morian jaja creo que el error es la clase String
		//pienso que funcionara de igual manera si combierto el String a string es decir un array de caractreres
		//y lo enviamos por referencia, de esa manera creo que funcionara y asi nos evitamos el tener que hacer un bucle
		//for para unir caracter por caracter al string bueno eso creo yo jaja

		Serial.print("\t");
		delay(100);
		Wire.readBytes((byte*)&mensaje, 6);



		Serial.println();
		Serial.print("Mensaje completo " + mensaje);
		Serial.print("\t");
		Serial.print("Available: " + String(Wire.available()));
		Serial.println(" ");



	}























	//REQUEST
	//ahora aprendo de los request, que es indicarle al esclavo que lo vas a leer o que quieres que te envie info
	//esto es bueno para mantener la estructura 1 maestro muchos esclavos asi en el esclavo usas el write pero solo en la funcion
	//que se activa cuando el maestro le solicita datos, no permitimos que el esclavo nos envie info cuando el quiera
	//asi por ejemplo tienes que estar monitoriando continuamente con el maestro los datos del esclavo
	//porque segun internet es la forma mas comoda de hacerlo y aunque el protocolo i2c soporta multimaestros
	//dicen que es la manera para que es codigo se vuelva limpio. que 1 solo maestro decida cuando pedir informacion
	//aunque tengas que estar monitoriando constantemente a los esclavos. Auqnue quizas en cierto punto seria bueno 
	//que un esclavo le envie informacion cuando el quiera es decir se convierta en maestro. pero ya lo veremos. 
	//SI USAMOS ESTRUCTURAS TODO SE VUELVE MAS FACIL!!!

	/*
	Okay los request son algo complicados porque tienes que saber exactamente la cantidad de bytes que te va a retornar el esclavo
	Muy bien explicacion del requestForm, porque tenemos que definir un numero de bytes que nos devolvera el esclavo?
			Porque no funciona como cuando enviamos info con un endTransmision?
			Como puedo recibir un numero variable de bits?

			El esclavo no tiene control de la linea scl del reloj ese control lo tiene el maestro, entonces el esclavo no puede
			enviar un bit de parada con el endTransmision como lo hace el maestro, entonces para eso por fuersa necesitamos
			saber cuantos bytes de respuesta esperamos por parte del esclavo con el requesFrom, si por ejemplo le decimos que esperamos 10 bytes
			y el esclavo solo nos manda 5bytes, requestFrom retornara 10 bytes incluso si solo llegaron 5, 5 con informacion correcta y los otros 5 con info incorrecta

			Segun la pagina si tun necesitas encviar un nunmero variable de bytes en el primer byte deberias enviar un "legth" para que el maestro
			sepa cuantos byts correctos va a recibir

			dice que aun asi los datos podrian estar comprometidos explica una manera de comprobar los datos es casi el ultimo comentario de la pagina del segundo link


			Entonces o en nuestro esclavo definimos exactamente la cantidad de bytes que enviaremos que creo seria una opcion buena o lo hacemos de forma varaible
			no importa aqui lo hago de forma variable. 


			Quizas para enviar informacion mas libremente seria mejor hacerlo con multi maestros
			asi el esclavo cuando envie info podra enviarla como maestro y manejar los stop bit o las funciones nrmales de beginTransmision y endtransmision
			y asi poder enviar valores variables mas facil o no se

			asi en el maestro solo tendremos que declarar su funcion on receive y ponerle su direccion
			en el esclavo enviariamos la informacion por ejemplo de alguna alarma al maestro,
			con simple beginTransmision y diriquiendonos al que antes era maestro que ahora sera esclavo
			usando los writes y end transmision
			el maestro que ahora es esclavo manejaria la info recibida en su onReceive y quisas con unos id de esclavos
			por ejemplo si el esclavo tal de tal mando este id entonces la infor que recibimos la trataremos asi etc etc

			o manejar los request jaja como eligamos ya entendi todo


	*/


	void hacerPeticionAlEsclavo() {
		if (retardoRequest.seHaCumplido()) {//este puede ser cualquier temporizador o funcion yo lo hice asi con mi clase que cree para que funcione como temporizador y cada cierto tiempo le solicite la info
			//si no se ha cumplido el tiempo para enviar la info, solicitamos la info con el tiempo del flujometro

			byte bufer[10]; //puedes crear un array para meterle ahi los datos recibidos o leerlos directo como sea

			int respuestaBytes = Wire.requestFrom(1, 32);
			//te indica si llegaron los bytes que solicitaste pero solo esos, pero si defino al maximo 32 bytes cualquier cantidad inferior sera verdad entonces deberiamos validar con otra cosa
			//si no se recibio nada de informacion por ejemplo el esclavo no esta disponible retorna 0 de plano. pero si si hay bytes de regreso aunque no sean los 32 los valida el te devuelve 32
			//si el esclavo manda mas de 32 bytes la informacion llega corrupta, y no podemos enviar los 32 exactos porque ya usamos un byte para la medida nos quedarian 31
			//entonces si hay mas bytes que los 32 de regreso la informacion llega corrupta
			//entonces alparecer nos limitamos a 32 bytes reales de ahi usamos 1 byte para indicarle el tamaño a leer entonces son 31 bytes para el mensaje
			//envie del esclavo un mensaje de 32 caracteres, aqui los recibimos pero el ultimo caracter salio corrupto es decir no existe porque el espacio son 32 menos 
			//el byte que ya usamos para el tamaño, si envio el mensaje de  31 bytes se recibe correctamente + el de valor de medida

			Serial.println("Tamano de respuesta " + String(respuestaBytes));

			if (respuestaBytes > 0) {      //si la péticion fue exitosa ubo retorno de datos los que sean pero mayores a 0, o si queremos podemos igualarlos a algo en especifico pero aqui no funcionara de mucho ya que definimos el tamaño maximo de 32 quizas con menores tamaños e enviando bytes fijos del esclavo funcione para otras cosas
				String mensaje = "";
				//tomamos el primer byte para saber cuantos bytes son reales donde viene la medida
				byte medidaRealRecibida = Wire.read();      //al leer el byte de medida en automatico se borra del bufer y ya solo nos quedaran los valores reales
				Serial.println("Exitosa medida Real " + String(medidaRealRecibida));

				//leemos la medida real recibida ya que recibiremos los 32 bytes excepto que si la info recibida es menor los demas bytes seran corruptos
				//por eso leemos con la medida real que nos envio el esclavo
				for (size_t i = 0; i < medidaRealRecibida; i++)
				{
					char c = Wire.read();
					mensaje += c;

				}
				Serial.println("Respuesta del esclavo: " + mensaje);


				/*
				Important! You do not need to test for Wire.available here. The Wire.requestFrom function does not return until either the requested data is fully available, or an error occurred. Building in a wait for Wire.available simply makes it possible for the code to hang forever if the data is not available.

				The correct method is to test whether the number of bytes you wanted was returned from Wire.requestFrom, as in the example above (10 bytes in this case).

				See Reply #10 below. Wire.requestFrom() will either return zero, or the number of bytes requested. If there is doubt as to whether all of the requested bytes were received do a data integrity check on the received bytes.

				You can use I2C_Anything (see below) to receive multiple bytes.
				*/

			}
			else {
				Serial.println("Quizas el esclavo no esta disponible");
			}

		}
	}


	void hacerPeticionesAlEsclavoEstructuras() {
		//https://www.luisllamas.es/estructuras-puerto-serie-arduino/
		//aqui recibiremos los datos que nos enviara el esclavo, en forma de estructura para eso tenemos que tener ya una estructura creada para escribirle los datos

		if (retardoRequest.seHaCumplido()) {


			struct MyStruct
			{
				int revoluciones;
				float temperatura;
				bool alarma;            //tendremos 1 o 0
			};

			MyStruct misDatos;
			Serial.println("Tamao de la estructura sin datos " + String(sizeof(misDatos)));     //aqui nos damos cuenta que aunuqe  no tengan datos la estructura ya separo la memoria necesaria para sus datos


			int respuestaBytes = Wire.requestFrom(1, 7);
			//aqui yo se que la estructura que me esta enviando el esclavo es de 7 bits al yo poner 3, estoy perdiendo los otros 4 byts
			//entonces la erstructura recibe perfectamente la variable revoluciones pero temperatura y alarma se quedan en 0 es decir no entraron datos para el
			//si yo pongo 6 se corto el de alarma! el tamaño de la estructura corresponde con el uso de memoria de cada variable que contiene
			//el int son 2 el float son 4 y el bool es 1 entonces nos dan los 7 byts que nos envia el esclavo!!!

			Serial.println("Tamano de respuesta " + String(respuestaBytes));

			if (respuestaBytes > 0) {      //si la péticion fue exitosa ubo retorno de datos los que sean pero mayores a 0, o si queremos podemos igualarlos a algo en especifico por ejemplo si esperamos la estructura de 7 bytes y quizas hay una de 9 entonces sabemos que aqui manejaremos la de 7 y no la de nueve

				//la estructura que mando el esclavo tiene 7 bits de longitud


				//aqui ocurre la magia aun no entiendo como jaja dejo el link de donde tome el ejemplo, ellos lo hicieron conserial pero aqui funciono
				Wire.readBytes((byte*)&misDatos, respuestaBytes);

				Serial.println("Estructura recibida exitosamente");
				Serial.println("Structure " + String(misDatos.revoluciones));
				Serial.println("Structure " + String(misDatos.temperatura));
				Serial.println("Structure " + String(misDatos.alarma));
				Serial.println("Tamano de la estructura con datos recividos " + String(sizeof(misDatos)));





			}
			else {
				Serial.println("Quizas el esclavo no esta disponible");
			}

		}
	}

	void hacerPericionesAlEsclavoFloatPorReferencia() {
		if (retardoRequest.seHaCumplido()) {


			float datoRecibido;

			Serial.println("Tamao del dato " + String(sizeof(datoRecibido)));     //nos damos cuenta que aunque no tenga datos, ya se separo la memoria

			int respuestaBytes = Wire.requestFrom(1, 4); //el float tiene 4 byts de tamaño

			Serial.println("Tamano de respuesta " + String(respuestaBytes));

			if (respuestaBytes > 0) {



				Wire.readBytes((byte*)&datoRecibido, respuestaBytes);

				Serial.println("Float recibida exitosamente");
				Serial.println("Float recibido " + String(datoRecibido));
				Serial.println("Tamano del float con datos recibidos " + String(sizeof(datoRecibido)));



			}
			else {
				Serial.println("Quizas el esclavo no esta disponible");
			}

		}
	}

	//JUSTO COMO LO IMAGINAMA Y LO EXPUSE ARRIBA NO FUNCIONA LOS ARDUINOS MUEREN BUENO EN EL CASO SOLO EL QUE RECIBE
	void hacerPeticionesAlEsclavoEstructuraConString() {
		if (retardoRequest.seHaCumplido()) {


			struct MyStruct
			{
				int revoluciones;
				float temperatura;
				bool alarma;            //tendremos 1 o 0
				String mensaje;
			};

			MyStruct misDatos;
			Serial.println("Tamao de la estructura sin datos " + String(sizeof(misDatos)));     //aqui nos damos cuenta que aunuqe  no tengan datos la estructura ya separo la memoria necesaria para sus datos


			int respuestaBytes = Wire.requestFrom(1, 13);
			//aqui yo se que la estructura que me esta enviando el esclavo es de 7 bits al yo poner 3, estoy perdiendo los otros 4 byts
			//entonces la erstructura recibe perfectamente la variable revoluciones pero temperatura y alarma se quedan en 0 es decir no entraron datos para el
			//si yo pongo 6 se corto el de alarma! el tamaño de la estructura corresponde con el uso de memoria de cada variable que contiene
			//el int son 2 el float son 4 y el bool es 1 entonces nos dan los 7 byts que nos envia el esclavo!!!

			Serial.println("Tamano de respuesta " + String(respuestaBytes));

			if (respuestaBytes > 0) {      //si la péticion fue exitosa ubo retorno de datos los que sean pero mayores a 0, o si queremos podemos igualarlos a algo en especifico por ejemplo si esperamos la estructura de 7 bytes y quizas hay una de 9 entonces sabemos que aqui manejaremos la de 7 y no la de nueve

				//la estructura que mando el esclavo tiene 7 bits de longitud


				//aqui ocurre la magia aun no entiendo como jaja dejo el link de donde tome el ejemplo, ellos lo hicieron conserial pero aqui funciono
				Wire.readBytes((byte*)&misDatos, respuestaBytes);

				Serial.println("Estructura recibida exitosamente");
				Serial.println("Structure " + String(misDatos.revoluciones));
				Serial.println("Structure " + String(misDatos.temperatura));
				Serial.println("Structure " + String(misDatos.alarma));
				Serial.println("Structure " + String(misDatos.mensaje));
				Serial.println("Tamano de la estructura con datos recividos " + String(sizeof(misDatos)));





			}
			else {
				Serial.println("Quizas el esclavo no esta disponible");
			}

		}
	}

	//tenia que funcionar!
	void hacerPeticionesAlEsclavoEstructuraConCharArray() {
		if (retardoRequest.seHaCumplido()) {


			struct MyStruct
			{
				int revoluciones;       //2 bytes
				float temperatura;      //4 bytes
				bool alarma;            //1 bytetendremos 1 o 0
				char mensaje[];           //que pasara si no le defino su tamaño al char array? se que el esclvo la defino en 10
			};

			MyStruct misDatos;
			Serial.println("Tamao de la estructura sin datos " + String(sizeof(misDatos)));
			//aqui nos damos cuenta que aunuqe  no tengan datos la estructura ya separo la memoria necesaria para sus datos
			//lo chistoso es que al parecer aqui no necesito definir la medida del char array, porque el esclavo que le envia la info ya la tiene definida
			//y de alguna manera toma el valor correctamente, pero aqui solo separa 7 bytes de memoria y si yo le definiera su tamaño 10 separaria 17
			//como el esclavo esa es la informacion que envia 17 bytes. se que de alguna manera terminara separando los 17 bytes al recibir los datos
			//pero de extraña forma no se imprimen en el Serial con sus 17 bytes de longitud. creo de cualquier mejor seria tratar de definir la medida tambien aqui
			//pero si vamos a enviar algun string variable del escalvo quizas sin definirlo seria mejor igual funciono

			int respuestaBytes = Wire.requestFrom(1, 17); //recibimos 17 bytes en la estructura del esclavo


			Serial.println("Tamano de respuesta " + String(respuestaBytes));

			if (respuestaBytes > 0) {      //si la péticion fue exitosa ubo retorno de datos los que sean pero mayores a 0, o si queremos podemos igualarlos a algo en especifico por ejemplo si esperamos la estructura de 7 bytes y quizas hay una de 9 entonces sabemos que aqui manejaremos la de 7 y no la de nueve

				//la estructura que mando el esclavo tiene 17 bytes de longitud


				//aqui ocurre la magia aun no entiendo como jaja dejo el link de donde tome el ejemplo, ellos lo hicieron conserial pero aqui funciono
				Wire.readBytes((byte*)&misDatos, respuestaBytes);

				Serial.println("Estructura recibida exitosamente");
				Serial.println("Structure " + String(misDatos.revoluciones));
				Serial.println("Structure " + String(misDatos.temperatura));
				Serial.println("Structure " + String(misDatos.alarma));
				Serial.println("Structure " + String(misDatos.mensaje));        //y recibimos correctamente el charArray o string con s minuscula :3 dentro de una estructura
				Serial.println("Tamano de la estructura con datos recividos " + String(sizeof(misDatos)));





			}
			else {
				Serial.println("Quizas el esclavo no esta disponible");
			}

		}
	}
	




	//CON ESTRUCTURAS aun me falta probar si enviando primero 1 byte con el tamaño de la estructura o con un "id" para por ejemplo enviar una estructura
	//con la informacion de las configuraciones, otra estructura para los datos actuales o cosas asi me explico? porque estamos limitados a 32bytes 
	//de una sola vez tendriamos de alguna manera que ser
	//capaces de detectar que estructura estamos recibiendo e enviar multiples estructuras diferentes!!!
	//enviar FLOAT por referencia como le hacemos con las estructuras para quitarnos de estarlo convirtiendo



	//esclavo
	/*
	en el setup definimos el handler del onrequest

	void setup() {
	Wire.begin(1);    //esta es la direccion de este esclavo

	Wire.onReceive(receiveEventString);       //llamamos a la funcion que se ejecutara cuando recibamos una peticion del master recibimo datos

	Wire.onRequest(requestEvent);               //registramos el handle para manejar la peticion del master

	Serial.begin(9600);
	}



		void loop() {
			//contamos los segundos para que sepamos que no se a trabado el arduino
			if (millis() > tiempo + 1000) {
				segundos++;
				Serial.println(String(segundos));
				tiempo = millis();
			}
		}


		//llamado por la rutina de interrupcion cuando quiere una respuesta el master
		void requestEvent() {


	String mensaje = "Hola, te envio mi info";
	byte bytesEnviados = mensaje.length();  //le sumamos uno para que se envien completos en la definicion del tamaño

	//convertimos el Sttring a string
	char strinMensaje[32]; //lo definimos al macimo tamaño en bytes que podemos enviar
	mensaje.toCharArray(strinMensaje, bytesEnviados + 1);

	Wire.write(bytesEnviados);  //para informarle al maestro que enviaremos datos variables y que sepa cuantos byts reales leer
	Wire.write(strinMensaje);

	Serial.println("Enviando request con tamano de " + String(bytesEnviados));

	
	Note that requestEvent is called from an Interrupt Service Routine (ISR)!

		You should not:


			Do serial prints
			Use "delay"
			Do anything lengthy
			Do anything that requires interrupts to be active



		You can only do one Wire.write in a requestEvent! You do not do a Wire.beginTransmission or a Wire.endTransmission. There is a limit of 32 bytes that can be returned.

		You can use I2C_Anything (see below) to return multiple bytes.

	
	}

	*/

	void requestEvent() {


		String mensaje = "Hola, te envio mi info";
		byte bytesEnviados = mensaje.length();  //le sumamos uno para que se envien completos en la definicion del tamaño

		//convertimos el Sttring a string
		char strinMensaje[32]; //lo definimos al macimo tamaño en bytes que podemos enviar
		mensaje.toCharArray(strinMensaje, bytesEnviados + 1);

		Wire.write(bytesEnviados);  //para informarle al maestro que enviaremos datos variables y que sepa cuantos byts reales leer
		Wire.write(strinMensaje);

		Serial.println("Enviando request con tamano de " + String(bytesEnviados));

		/*
		Note that requestEvent is called from an Interrupt Service Routine (ISR)!

			You should not:


				Do serial prints
				Use "delay"
				Do anything lengthy
				Do anything that requires interrupts to be active



			You can only do one Wire.write in a requestEvent! You do not do a Wire.beginTransmission or a Wire.endTransmission. There is a limit of 32 bytes that can be returned.

			You can use I2C_Anything (see below) to return multiple bytes.

		*/
	}


	void requestEventStructure() {


		struct MyStruct
		{
			int revoluciones;       //usa 2 bytes
			float temperatura;      //usa 4 bytes
			bool alarma;            //usa 1 byte
		};

		MyStruct misDatos;
		misDatos.revoluciones = 3250;
		misDatos.temperatura = 10.9589;
		misDatos.alarma = false;

		Serial.println("Structura " + String(misDatos.revoluciones));
		Serial.println("Structura " + String(misDatos.temperatura));
		Serial.println("Structura " + String(misDatos.alarma));
		Serial.println("Tamano de la estructura " + String(sizeof(misDatos)));



		Wire.write((byte*)&misDatos, sizeof(misDatos));

		Serial.println("Enviando request con tamano de " + String(sizeof(misDatos)));


	}

	void requestEventFloatPorReferencia() {


		float dato = 19.185;

		byte bytesEnviados = sizeof(dato);  


		Wire.write((byte*)&dato, bytesEnviados);

		Serial.println("Enviando request con tamano de " + String(bytesEnviados));


	}

	//NO FUNCIONA JUSTO COMO LO EXPUSE ARRIBA
	void requestEventStructuraConString() {


		struct MyStruct
		{
			int revoluciones;       //usa 2 bytes
			float temperatura;      //usa 4 bytes
			bool alarma;            //usa 1 byte
			String mensaje;         //usa 6 bytes
		};

		MyStruct misDatos;
		misDatos.revoluciones = 3250;
		misDatos.temperatura = 10.9589;
		misDatos.alarma = false;
		misDatos.mensaje = "Hola que tal";

		Serial.println("Structura " + String(misDatos.revoluciones));
		Serial.println("Structura " + String(misDatos.temperatura));
		Serial.println("Structura " + String(misDatos.alarma));
		Serial.println("Structura " + String(misDatos.mensaje));
		Serial.println("Tamano de la estructura " + String(sizeof(misDatos)));



		Wire.write((byte*)&misDatos, sizeof(misDatos));

		Serial.println("Enviando request con tamano de " + String(sizeof(misDatos)));


	}

	//este si funciona claro debia funcionar :3
	void requestEventStructureConStringCharArray() {


		struct MyStruct
		{
			int revoluciones;       //usa 2 bytes
			float temperatura;      //usa 4 bytes
			bool alarma;            //usa 1 byte
			char mensaje[10];         //usa 10 bytes y si no lo defino en 10? jaja ya probe y murio arduino tiene que ir definido
		};//persona1{3200, 10.5, true, "Hola"}; en la estructura podemos definir el objeto al final de su llave y antes de su punto y coma https://www.youtube.com/watch?v=3-XcUG_6TQc

		MyStruct misDatos;//o podemos definir su objeto 
		misDatos.revoluciones = 3250;
		misDatos.temperatura = 10.9589;
		misDatos.alarma = false;
		String hola = "Que onda";
		hola.toCharArray(misDatos.mensaje, 10); //este hara su trabajo de definirlo a la longitud? jaja no si tu no defines el char array esto muere 



		Serial.println("Structura " + String(misDatos.revoluciones));
		Serial.println("Structura " + String(misDatos.temperatura));
		Serial.println("Structura " + String(misDatos.alarma));
		Serial.println("Structura " + String(misDatos.mensaje));
		Serial.println("Tamano de la estructura " + String(sizeof(misDatos)));



		Wire.write((byte*)&misDatos, sizeof(misDatos));

		Serial.println("Enviando request con tamano de " + String(sizeof(misDatos)));


	}










	//MULTIMAESTROS	 
	/*para esto no pondre explicacion porque ya se hace con todo lo que sabemos manejar hasta ahorita
	los request son solo para un maestro y varios esclavos porque le solicitas informacion al esclavo pero el maestro tiene el control de uando hacerlo
	
	y para multimaestros es decir que el que antes era esclavo actue como maestro y le envie info al que antes era maestro que ahora sera esclavo
	solo se usan los onReceive en ambos arduinos que funcionaran como multimaestros

	aunque bueno tambien podrias usar los request en ambos pero creo que se volveria demaciado complejo de entender y funcionar correctamente
	asi en multimaestros simpelemtne uno le envia informacion al otro cuando quieren y es necesario, 

	y con los request seria solicitarle info al otro arduino pero para que usarlo si cuando el otro se comporte como maestro este recibira los datos
	en su onreceive convencional me explico?

	y si aqui vemos que con multimaestros podria complicarse poco a poco las cosas, 
	las desventajas de usar el Request es que hay que definir la cantidad exacta de valores que nos devolvera o de manera variable pero tambien es algo raro

	bueno meditar las opciones que se pueden usar. Me gustaria mas mantenerme en la postura de 1 solo maestro pero el request no me gusto mucho
	*/




	
	
	
	
	
	
	
	
	
	
	
	
	
	//Manejo de nueva libreria expuesta en el 2do link que se llama I2C_Anything: 
	//http://gammon.com.au/Arduino/I2C_Anything.zip
	/*
	Send/receive any data type


		If you want to send or receive things other than simple bytes (eg. int, float, structures) you can use the I2C_Anything which simplifies this process.

		The I2C_Anything library can be downloaded from:

		http://gammon.com.au/Arduino/I2C_Anything.zip

		Just unzip the downloaded file, and put the resulting folder "I2C_Anything" into your Arduino "libraries" folder. Then restart the IDE and you are ready to use it.






					Example of sending with I2C_Anything:


			#include <I2C_Anything.h>

			...

			  long foo = 42;
			  float bar = 123.456;

			  Wire.beginTransmission (SLAVE_ADDRESS);
			  I2C_writeAnything (foo);
			  I2C_writeAnything (bar);
			  if (Wire.endTransmission () == 0)
				  {
				  // success!
				  }
				else
				  {
				  // failure ... maybe slave wasn't ready or not connected
				  }












				  Example of receiving with I2C_Anything:


					#include <I2C_Anything.h>

					...

					volatile boolean haveData = false;
					volatile long foo;
					volatile float bar;

					// called by interrupt service routine when incoming data arrives
					void receiveEvent (int howMany)
					 {
					 if (howMany >= (sizeof foo) + (sizeof bar))
					   {
					   I2C_readAnything (foo);
					   I2C_readAnything (bar);
					   haveData = true;
					   }  // end if have enough data
					 }  // end of receiveEvent



					Note that variables set within an ISR have to be declared volatile. Note the use of a flag to signal the main loop that we have received something.


	*/





};

