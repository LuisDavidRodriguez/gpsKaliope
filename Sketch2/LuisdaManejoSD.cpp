#include "LuisdaManejoSD.h"


File LuisdaManejoSD::myFileL;

bool LuisdaManejoSD::setUltimaPosicionEnviadaLocalizacion(long number)
{
	

	bool control = false;

	if (SD.exists(_LUISDA_SD_NOMBRE_LOCALIZACION_ULTIMA_POSICION)) {
		//si existe entonces primero lo eliminamos para que solo se guarde un solo valor en todo el archivo

		if (!SD.remove(_LUISDA_SD_NOMBRE_LOCALIZACION_ULTIMA_POSICION)) {
			//Error no se ha podido eliminar el archivo
			return false;

		}
	}

	myFileL = SD.open(_LUISDA_SD_NOMBRE_LOCALIZACION_ULTIMA_POSICION, FILE_WRITE);

	if (myFileL) {

		myFileL.print(number);
		myFileL.print("\n");

		
	}
	else {
		//error no se a podido crear y o abrir el archivo
		return false;
	}


	myFileL.close();
	return true;

}

long LuisdaManejoSD::getUltimaPosicionEnviadaLocalizacion()
{
	long dato;

	if (SD.exists(_LUISDA_SD_NOMBRE_LOCALIZACION_ULTIMA_POSICION)) {

		myFileL = SD.open(_LUISDA_SD_NOMBRE_LOCALIZACION_ULTIMA_POSICION);

		if (myFileL) {
			
			dato = myFileL.readStringUntil('\n').toInt();
			
		}
		else {
			dato = 0;
		}
	}
	else {
		dato =  0;
	}

	myFileL.close();
	return dato;
	
	
}



bool LuisdaManejoSD::setUltimaPosicionEnviada(String nombreDelArchivoUltimaPosicion, long number)
{
    bool control = false;

    if (SD.exists(nombreDelArchivoUltimaPosicion)) {
        //si existe entonces primero lo eliminamos para que solo se guarde un solo valor en todo el archivo

        if (!SD.remove(nombreDelArchivoUltimaPosicion)) {
            //Error no se ha podido eliminar el archivo
            return false;

        }
    }

    myFileL = SD.open(nombreDelArchivoUltimaPosicion, FILE_WRITE);

    if (myFileL) {

        myFileL.print(number);
        myFileL.print("\n");


    }
    else {
        //error no se a podido crear y o abrir el archivo
        return false;
    }


    myFileL.close();
    return true;
}
/*
Abre el archivo que almacena la ultima posicion enviada
*/
long LuisdaManejoSD::getUltimaPosicionEnviada(String nombreDelArchivoUltimaPosicion)
{
    long dato;

    if (SD.exists(nombreDelArchivoUltimaPosicion)) {

        myFileL = SD.open(nombreDelArchivoUltimaPosicion);

        if (myFileL) {

            dato = myFileL.readStringUntil('\n').toInt();

        }
        else {
            dato = 0;
        }
    }
    else {
        dato = 0;
    }

    myFileL.close();
    return dato;
}


/*
Abre un archivo deseado, y escrube en el la informacion, esta debe venir sin salto de linea
ya que aqui el sistema la añade automaticamente

importante los nombres de los archivos no pueden pasar de 8 caracteres y 3 para las extecnioens
https://eldesvandejose.com/2016/05/12/la-libreria-sd/
*/
bool LuisdaManejoSD::escribirArchivo(String nombre, String informacion) {
    myFileL = SD.open(nombre, FILE_WRITE);

    if (myFileL) {
        myFileL.print(informacion);
        myFileL.print("\n");



    }
    else {
        Serial.println(F("Error al abrir el archivo"));
        return false;
    }
    myFileL.close();
    return true;

}


bool LuisdaManejoSD::escribirArchivo(String nombre, String informacion, String informacion2) {
    myFileL = SD.open(nombre, FILE_WRITE);

    if (myFileL) {
        myFileL.print(informacion);
        myFileL.print(informacion2);
        myFileL.print("\n");



    }
    else {
        Serial.println(F("Error al abrir el archivo"));
        return false;
    }
    myFileL.close();
    return true;

}



/*
Leeremos desde una posicion inicial hasta un salto de linea un String, esto porque en todos nuestros archivos el salto de linea es el separador que
inidica que el siguiente es un nuevo dato de informacion

los 2 punteros que creo, contendran la informacion aparit de cual byte del archivo comenzo a leer la informacion y en que posicion envio el ultimo dato
asi si la respuesta es enviada satisfactoriamente, la siguiente ves que vuelva a leer la informacion el servidor le dira apartir de que posicion del archivo ya recibio
y este metodo podra saber apartir de donde leer para enviar los datos

*/
String LuisdaManejoSD::leerArchivoResponderStringHastaLineaNueva(String nombreArchivo, long& posicionInicial, long& posicionFinal, long& totalSize) {

	Serial.println(F("\n-Metodo leerArchivoResponderStringHastaLineaNueva-\n"));
	myFileL = SD.open(nombreArchivo);
	String informacion = "";
	



	if (myFileL) {
		Serial.println(nombreArchivo +  " abierto correctamente");
		totalSize = myFileL.available();													//cuando no has leido nada de informacion del sd esto nos retorna el tamaño total del archivo

		myFileL.seek(posicionInicial);													//nos colocamos en la parte inicial de esta forma el archivo comenzara a leer apartir de la posicion que le enviemos
		Serial.println("posicion inicial " + String(myFileL.position()));

		while (myFileL.available()) {
			Serial.println("posicion en progreso " + String(myFileL.position()));

			informacion = myFileL.readStringUntil('\n');
		}

		Serial.println("posicion Final " + String(myFileL.position()));
		posicionFinal = myFileL.position();
		myFileL.close(); //cerramos el archivo

	}
	else {
		Serial.println(F("Error al abrir el archivo"));
	}


	return informacion;
}





/*
Param:
Posicion inicial    -> Definira el puntero en la SD para apartir de ahi leer la informacion
Posicion Final      -> Devolvera aqui la ultima posicion hasta donde puedes leer informaicon cooerente dentro de los 1000bytes
Total bytes         -> Devolvera aqui cuantos bytes de informacion cooerente tienes antes de llegar a los 1000Bytes
TotalSize           -> Devolvera aqui el tamaño total del archivo si se leyo exitosamente

-1 Posicion Finaly TotalBytes   -> seran -1 si el archivo no se pudo leer en la SD, o si envio una posicionInicial mayor al tamaño total del archivo
*/
long LuisdaManejoSD::getTotalSizeOf(String nombreArchivo)
{
    myFileL = SD.open(nombreArchivo);

    if (myFileL) {

        long size = myFileL.size();

        myFileL.close();

        return size;
    }
    else {
        return 0;
    }
}
void LuisdaManejoSD::evaluarBytesDisponiblesParaEnvioHastaSaltoDeLinea(String nombreArchivoSD,long posicionInicial, long& posicionFinal, long& totalBytes, long& totalSize) {



    /*
    Tenemos que saber cuanta informacion "coerente" tenemos para enviar, es decir si nuestro limite de envio es de 1000 bytes
    pero nuestra informacion coerente de coordenadas es por ejemplo eso, donde cada punto de informacion esta separado por un \n saldo de linea
    28/1/2020 13:50:11,19.791899,-99.863899,359.5,1.82,280.200012,2.24,14
    28/1/2020 13:50:11,19.791899,-99.863899,359.5,1.82,280.200012,2.24,14
    28/1/2020 13:50:11,19.791899,-99.863899,359.5,1.82,280.200012,2.24,14
    28/1/2020 13:50:11,19.791899,-99.863899,359.5,1.82,280.200012,2.24,14

    No sirve de nada si yo me agarro a enviar solo bytes a lo menso hasta acompletar los 1000 porque al tener un largo fijo al llegar a mi byt mil podra enviar un punto a la mitad por ejemplo
    28/1/2020 13:50:11,19.791899,-99.863899,359.5,1.82,280.200012,2.24,14
    28/1/2020 13:50:11,19.791899,-99.  --aqui se acompleto el byte 1000 es solo ejemplo

    entonces esa linea se envia intermedia al sevidor y el servidor no sabra que hacer con ella!!

    entonces lo que haremos es enviar no byte a byte sino Strings hasta su salto de linea, contaremos cuantos bytes tenemos
    hasta alcanzar el numero mas cercano a 1000, sin pasarnos, asi devolveremos un long con el numero maximo de bytes de informacion cooerente que podemos leer esto lo haremos apartir
    de una posicion inicial dada,
    */



    Serial.println(F("\n--evaluarBytesDisponiblesParaEnvioHastaSaltoDeLinea--\n"));

    int contadorDeBytes = 0;

    long pI = 0;
    long pF = 0;



    myFileL = SD.open(nombreArchivoSD);

    if (myFileL) {
        totalSize = myFileL.size();

        if (posicionInicial > totalSize) {
            posicionFinal = -1;
            totalBytes = -1;
            return;     //detenemos la ejecucion del NHMGSGÑLcodigo hasta aqui porque la posicion inicial pasada violara el tamaño total del archivo es decir es incorrecta
        }

        myFileL.seek(posicionInicial);

        while (myFileL.available())
        {
            pI = myFileL.position();
            String datos = myFileL.readStringUntil('\n');
            pF = myFileL.position();

            contadorDeBytes += datos.length() + 1;                              //contamos los bytes del renglon leido y lo almacenamos
            /*
            Añadimos un +1 porque el readStringUntil solo te devolvera un string con los datos antes del salto de linea pero no vendra incluido el salto de linea en tu string
            pj. 
            123456789\n
            en el string resultante tendras solo 
            123456789   -> con un lenght de solo 9 caracteres cuando deberias tener 10 caracteres!! incluido el salto de linea
            si yo no sumo el salto de linea nos devolvera el numero de bytes de informacion que tenemos pero sin los saltos de linea
            como tambien queremos enviar esos caracteres lo que pasara es que al final un renglon tendra informacion inclompleta robada por los saltos de linea
            que no estan contados.

            Si quieres entender mas el porque ve el metodo LuisdaManejoSD::leerUnRenglon() en el apartado ******descubrimiento quien es el problema**************
            ahi describo cual fue el problema y arriba de ello pongo lo que pasaba cuiando no le sumaba 1


            Jajaja que menso ahora que lo veo si quieres quitarte de problemas y de andarle sumandole un mas 1
            solo resta la posicion final que te entrega este metodo y quitale la posicion inicial y te dara el total de bytes que tiene el archivo
            obviamente en entre la posicion inicial y la final, jaja como sea ambas funcionan, 
            ya esta resuelto!, como sea tengo que usar el contador de bytes para saber hasta donde paso de 1000 bytes, entonces la opcion del contador tambien es buena
            por ejemplo antes un ejemplo

            si leia de la posicion inicial 0 mi metodo me decia que podia leer hasta la posicion 954
            pero el contador de bytes al no contar los saltos de linea me decia que solo enviariamos 933 bytes ignoraba 21 \n
            si tu pones esa informacion en sublime text te dice que son 933 caracteres porque no se XD me imagino que no cuenta los saltos de linea
            pero entonces ahi ocurria el error de que el ultimo renglon llegaba a medias,
            ahora si tu sumas +1 en el contador te dice que son 954 bytes en lugar de 933 que es lo mismo que restarle a la pF 954 la pI 0 = 954 xD
            */

            Serial.println(String(pI));
            Serial.println(datos);
            Serial.println(String(pF));
            //preguntamos si el contador de bytes ya es mayor a 1000

            if (contadorDeBytes > 1000) {

                posicionFinal = pI;
                /*
                Cuando el contador de bytes sea mayor a 1000 ya no guardaremos el valor en contador definitivo
                para que asi contador definitivo conserve el ultimo valor que estuvo antes de 1000

                salimos del while
                */
                break;
            }
            else
            {
                totalBytes = contadorDeBytes;
                posicionFinal = pF;
                //posicionFinal = file.position();
                /*
                De esta manera mientras el contador de bytes no sea mayor que 1000 en el contador definitivo tendremos el valor correcto

                lo mismo como obtenemos la posicion final si se lee un string y nos damos cuenta que no es mayor a 1000 actualizamos la posicion final es decir en donde esta ahorita el puntero del lector sd
                al leerlo si se leyo un string de 100 caracteres tendremos la posicion 101

                seguimos leyendo hasta que estamos en la posicion 920 al leer el string nos damos cuenta que ya estamos arriba de 1000 bytes,
                entonces ya se entra a donde se rompe el bucle y en posicion final tenemos la ultima posicion valida que esta dentro de los 100bytes
                */
            }
        }

        Serial.print(F("Bytes con informacion hasta salto de linea menores a 1000: "));
        Serial.print(String(totalBytes));
        Serial.print(F("\tapartir de la posicion: "));
        Serial.println(String(posicionInicial));
        Serial.print(F("Podrias leer desde la posicion: "));
        Serial.print(String(posicionInicial));
        Serial.print(F("\tHasta la posicion: "));
        Serial.print(String(posicionFinal));
        Serial.print(F(" -1 es decir , hasta la pos: "));
        Serial.print(String(posicionFinal-1));
        Serial.print(F("\t contiene informacion, la posicion "));
        Serial.print(String(posicionFinal));
        Serial.println(F("Es la posicion inicial de la siguiente informacion "));
        Serial.print(F("Tamano total del archivo: "));
        Serial.println(String(totalSize));

        /*
        Por ejemplo si tu lees esto
        123456789
        123456789   123456789

        la posicion 0 es antes del primer 1
              1     2   3   4   5   6   7   8   9   \r \n       1     2   3   4   5   6   7   8   9   \n
             p0|   p1 | p2 |p3 |p4 |p5 |p6 |p7| p8| p9|p10   |p11| p12 |p13|p14|p15|p16|p17|p18|p19|p20|  p22     
        */
       

        Serial.println(F("\n--Fin evaluarBytesDisponiblesParaEnvioHastaSaltoDeLinea--\n"));
        myFileL.close();
       



    }
    else {
        Serial.println(F("\nNo se pudo abrir el archivo\n"));
        Serial.println(F("\n--Fin evaluarBytesDisponiblesParaEnvioHastaSaltoDeLinea--\n"));

        posicionFinal = -1;
        totalBytes = -1;

        
    }

}

/*
Return
True        -> Si se leyo exitosamente el archivo SD y envio datos al servidor con exito
False       -> EL archivo no pudo ser abierto por el metodo que analiza los datos cooerentes a enviar, 1.-El arvhivo ya no existe 2.- no esta incertada la SD, 3.- se trato de leer una posicion inicial mayor al tamaño del archivo
False       -> el archivo no tenia datos nuevos que enviar apartir de la posicion inicial dada
False       -> No se ha podido abrir el archivo SD a leer
False       -> No se enviaron los datos debido a que el modulo no respondio el ">"
False       -> Los datos no se enviaron el modulo sim no respondio SEND OK o el servidor no respondio con 200 OK
*/
bool LuisdaManejoSD::enviar1000bytesEnSDporSim808(ClaseManejoSim808* sim808, String dominioSecundario, String archivoDeDestinoEnServidor, String nombreDelArchivoSD, String nombreDelArchivoUltimaPosicion, long posicionInicial, String info2)
{
    Serial.println(F("\n-Metodo enviar1000bytesEnSDporSim808-\n"));
    //En este metodo estamos entendiendo que ya se tuvo que haber abierto una conexion hacia el servidor


    long posicionFinal = 0;
    long totalBytesCoerentesMenoresA1000 = 0;
    long totalSizeDelArchivo = 0;
    LuisdaManejoSD::evaluarBytesDisponiblesParaEnvioHastaSaltoDeLinea(nombreDelArchivoSD,posicionInicial, posicionFinal, totalBytesCoerentesMenoresA1000, totalSizeDelArchivo);             //obtenemos la posicion inicial de nuestro archivo

    char* info;








    if (totalBytesCoerentesMenoresA1000 > 0) {
        //si es diferente significa que el archivo sd si fue encontrado y tiene datos nuevos apartir de la posicion inicial que enviaremos
        //si el arvhivo SD no pudo ser abierto devolvera -1, entonces por eso especificamos que sea mayor a 0
        //si es 0 significa que ya no hay nueva informacion que leer apartir de la posicion inicial dada
        




            myFileL = SD.open(nombreDelArchivoSD);  

            if (myFileL) {
                //si no podemos abrir el archivo SD es decir quisas no hay memoria SD insertada entonces ni siquiera enviamos la peticion HTTP
                //y tampoco hacemos que entre en modo de envio de datos para ahorrar pasos y posibles errores

                if (sim808->gprsAbrirEnvioDeInformacionPOST(dominioSecundario, archivoDeDestinoEnServidor, totalBytesCoerentesMenoresA1000, info2)) {
                    //para poder entrar aqui significa que se el metodo ya envio una peticion HTTP con el largo de datos que vamos a enviar y el modulo respondio >
                    //indicando que todo lo que se le envie aqui entrara como cuerpo del mensaje a enviar
                    //abrimos el archivo de la memoria SD que queremos enviar
                    //en este momento creamos el bucle que nos enviara 1000 bytes
                    //crearemos nuestro bucle que leera de un archivo SD, desde una posicion inicial dada hasta un salto de linea


                    
                    
                        myFileL.seek(posicionInicial);

                        while (myFileL.position() < posicionFinal) {
                            //leemos y enviamos nuestra informacion al sim 808
                            String envio = myFileL.readStringUntil('\n');
                            envio = LuisdaManejoSD::quitarCaracteresNoAscii(envio);                            
                            Serial3.print(envio);                           //se supone que hasta aqui enviamos al modulo sim la informacion que esta antes del salto de linea pero sin incluirlo



                            /*
                            lo usamos para leer la respuesta que nos envia el sim808 que debe ser una copia de la informaicon que le enviamos
                            Al usar el metodo no enviamos ningun comando porque solo quiero usar la parte que lee la respuesta, 
                            al usar este metodo estamos enviando en automatico un salto de linea que viene incluido en el
                            si no leemos la respuesta del modulo deberemos enviar el salto de linea por nuestra cuenta
                            */
                            info = sim808->enviarComandoSinEvaluar("", _MENOR_A_128_BYTES, nullptr);            
                            String respuesta(info);

                            Serial.println(F("Informacion enviada hacia el modulo*****"));
                            Serial.println(envio);
                            Serial.println(respuesta);
                            Serial.println(F("Informacion recibida del modulo*****"));
                            //aqui vemos que el modulo responde el salto de linea que se le envio con el enviarCOmandoSInevaluar
                            /*
                            

                            Informacion enviada hacia el modulo*****
                            27869170031697544,29/1/2020 20:54:14,19.795300,-99.878700,2564.8,9.19,283.99,1.05,11,3D             ->aqui no se envio el salto de linea mas que el del serial println
                            27869170031697544,29/1/2020 20:54:14,19.795300,-99.878700,2564.8,9.19,283.99,1.05,11,3D             -> este salto de linea es el normal del serial println
                                                                                                                               -> este salto de linea es el que envia el metodo enviar comando sin evaluar cuando leemos la respuesta
                            Informacion recibida del modulo*****
                            

                            

                            por ejemplo hice esta parte que verificara que las respuestas fueran correctas que devuelve el modulo

                              if (respuesta.equals(envio+"\n")) {
                             //el modulo respondio exactamente lo mismo que le enviamos quiere decir que ya lo tiene en su buffer
                              Serial.println(F("El modulo sim recibio correctamente el fragmento enviado"));

                                //si se recibio exitosamente entonces continuamos al siguiente while

                            }
                            else {
                                //si no rsponde exactamente lo mismo vamos a cancelar el envio para que el modulo salga del estado de envio de datos porque la informacion fue erronea
                                todosLosFragmentosEnviadosCorrectamente = false;
                                sim808.enviarComandoSinEvaluar(AT_SMS_CANCELAR_ENVIO_SMS_1, _MENOR_A_32_BYTES, nullptr);
                                Serial.println(F("La inforacion enviada y la que recibio el modulo y respondio son diferentes"));
                                break;
                            }

                            funciona pero solo si la informacion no tiene retornos de carro por ejemplo si yo escribo info en un editor de texto de la compu el automaticamente añade \r\n
                            y al hacer esto solo responde 
                            Informacion enviada hacia el modulo*****
                            123456789
                                                                                    --> el modulo sim no responde la misma info que le enviamos no entiendo porque qquiza por el retorno de carro

                            Informacion recibida del modulo*****


                            entonces deja de funcionar el codigo que comprueba

                            pero si la info esta guardada directo en la sd con el arduino solo añadimos \n
                            entonces 

                            Informacion enviada hacia el modulo*****
                            1869170031697544,29/1/202020:51:42,19.795300,-99.878601,2570.7,2.16,4.10,1.23,11,3D
                            1869170031697544,29/1/202020:51:42,19.795300,-99.878601,2570.7,2.16,4.10,1.23,11,3D

                            Informacion recibida del modulo*****

                            y asi si funciona pero hay que añadirle un +"\n" para que compruebe bien porque el modulo responde un salto de linea mas

                            

                            Listo?!! entnces mejor no uso ese codigo evaluador, seria bueno para comprobar el envio de los datos al sim, pero mejor no lo uso para evitar
                            los problemas con la info o los retornos de carro
                            */


                        }         
                        /*
                        Le segiremos enviando informacion al sim808 en este bucle hasta que la posicion del puntero de la sd
                        sea mayor a la posicion final que nos dijo el metodo que analiza hasta donde esta la informacion coerente
                        ese metodo sabe que en una posicion que es menor a 1000 bytes hasta ahi hay informacion completa y asi evitamos
                        mandar informacion a la mitad
                        */
                    











                        /*
                        Ahora que el modulo sim tiene en su buffer los 1000 bytes o menos que queremos enviar al servidor 
                        tenemos que finalizar el envio de la informacion enviando el caracter char(26) que el modulo reconoce
                        y sabe que ahora hemos terminado de enviarle el contenido del mensaje, el modulo respondera SEND OK
                        indicando que los datos se enviaron y recibio una respuesta del servidor, despues se esperara la otra respuesta
                        del servidor que si la peticion http se hiso correctamente y ademas le enviamos los bytes que le dijimos le enviariamos en la peticion http
                        entonces respondera 200 Ok indicando que la informacion se manejo satisfactoriamente
                        */
                        if (sim808->gprsCerrarEnvioDeInformacionPOST()) {

                            Serial.print(F("datos enviados satisfactoriamente desde la posicion de archivo"));
                            Serial.print(String(posicionInicial));
                            Serial.print(F("\ta la final "));
                            Serial.print(String(posicionFinal));
                            Serial.print(F("\tde un tamano total de: "));
                            Serial.print(String(totalSizeDelArchivo));
                            Serial.print(F("\ten esta sola conexion enviamos: "));
                            Serial.print(String(totalBytesCoerentesMenoresA1000) + " bytes");


                            LuisdaManejoSD::setUltimaPosicionEnviada(nombreDelArchivoUltimaPosicion, posicionFinal);
                            myFileL.close();                                                        //cerramos el archivo 


                            Serial.print(F("Actualizando la posicion final del archivo "));
                            Serial.println(nombreDelArchivoUltimaPosicion);
                            Serial.print(F("Posicion final actualizada "));
                            Serial.println(String(posicionFinal));

                            

                            
                            LuisdaManejoSD::setUltimaPosicionEnviada(nombreDelArchivoUltimaPosicion, posicionFinal);


                            if (posicionFinal == totalSizeDelArchivo) {
                                //si la posicion final ya fue igual a la posicion del archivo eliminamos todo el archivo porque ya fuie enviado
                                if (SD.remove(nombreDelArchivoSD)) {
                                    Serial.print(nombreDelArchivoSD);
                                    Serial.println(F(" eliminado exitosamente porque ya fue enviado totalmente"));
                                    Serial.print(F("Reiniciamos a 0 la ultima posicion enviada del archivo en "));
                                    Serial.println(nombreDelArchivoUltimaPosicion);
                                    

                                    LuisdaManejoSD::setUltimaPosicionEnviada(nombreDelArchivoUltimaPosicion, 0);//reiniciamos a 0 la ultima posicion enviada

                                }
                            }

                            return true;

                        }
                        else {
                            Serial.println("Los datos no se enviaron el modulo sim no respondio SEND OK o el servidor no respondio con 200 OK");
                            myFileL.close();            //cerramos el archivo 
                            return false;
                        }



                    






                }
                else {

                    Serial.println(F("No se enviaron los datos debido a que el modulo no respondio el \">\""));
                    myFileL.close();                                                                            //cerramos el archivo 

                    return false;
                }






            }
            else {
                Serial.print(F("No se ha podido abrir el archivo SD "));
                Serial.println(nombreDelArchivoSD);
                return false;

            }







        




    }
    else if (totalBytesCoerentesMenoresA1000 == 0) {

    /*
    AL entrar aqui significa que el metodo que analiza primero la informacion que vamos a enviar que esta sea coeerente apartir de la posicion inicial que le indicamos
    encontro el archivo y lo abrio pero ya no encontro informacion nueva o que ya no hay informacion nueva que enviar apartir de la posicion inicial,

    otra opcion por entender es que por ejemplo, si ya hemos enviado hasta la posicion 1200 y el archivo sd mide 1200 significa que ya recorrimos todo el archivo
    por lo tanto metodo que sabe la informacion coerente nos devolvera 0, porque ya no hay nuevos datos que enviar apartir de la posicion 1200
    en ese momento eliminaremos el archivo SD


    */
        Serial.println(F("el archivo no tenia datos nuevos que enviar apartir de la posicion inicial dada"));
        Serial.print(F("Posicion inicial: "));
        Serial.print(String(posicionInicial));
        Serial.print(F("\tTamano Total: "));
        Serial.println(String(totalSizeDelArchivo));
        

        
            if (SD.remove(nombreDelArchivoSD)) {
                Serial.println(nombreDelArchivoUltimaPosicion);
                
                Serial.print(F(" eliminado exitosamente porque ya fue enviado totalmente"));
                Serial.print(F("Reiniciamos a 0 la ultima posicion enviada del archivo "));
                Serial.println(nombreDelArchivoUltimaPosicion);

                LuisdaManejoSD::setUltimaPosicionEnviada(nombreDelArchivoUltimaPosicion, 0);//reiniciamos a 0 la ultima posicion enviada

            }
        
        



            Serial.println(F("\n-Fin enviar1000bytesEnSDporSim808-\n"));


        return false;
    }
    else if (totalBytesCoerentesMenoresA1000 == -1) {

    //si es igual a -1 significa que el que evalua los datos cooerentes no pudo abrir el archivo o no existe
    //o se envio una posicion inicial de lectura mayor al tamaño del archivo lo cual produce un error
    //en ese caso solo retornaremos false
    Serial.print(nombreDelArchivoSD);
    Serial.println(F(" no pudo ser abierto por el metodo que analiza los datos cooerentes a enviar, 1.-El arvhivo ya no existe 2.- no esta incertada la SD, 3.- se trato de leer una posicion inicial mayor al tamaño del archivo"));
    Serial.print(F("Posicion inicial del archivo: "));
    Serial.println(String(posicionInicial));
    Serial.print(F("tamano total del archivo "));
    Serial.println(String(totalSizeDelArchivo));
    
    Serial.println(F("\n-Fin enviar1000bytesEnSDporSim808-\n"));


    return false;
    }



    
}


/*
Quita caracteres que no podamos enviar al modulo sim 
por ejemplo se han generado errores no entiendo porque pero en el archivo acelerometro
o en coordenadas se hahn llegado a guardar caracteres extraños ya sea el char26 o caracteres
fuera de ascii imprimibles por ejemplo el 170

 se han impreso esos caracteres en la memoria SD y eso al enviarlos al modulo sim
 para envio al servidor el moudlo sim se traba y no envia la informacion
 entonces lo que haremos sera revizar la cadena que leemos de la memoria y si tiene algun caracter que 
 no sea salto de linea o que este fuera del ascii normal lo remplazaremos por un vacio o espacio

 por ejemplo el modulo sim usa el caracter 26 SUB para saber que ya le terminamos de enviar la informacion
 entonces si hay algun caracter 26 en la memoria SD el modulo sim entendera que ya terminamos de enviar el mensaje
 eso solo por nombrar un error que podria ocurrir si vemos la tabla ascii tenemos que del 00 al 31 son comandos e control
 entre ellos esta el 10 que es el salto de linea y el 13 que es retorno de carro el 127 es DELETE
 y caracteres imprimibles tenemos desde el 32 hasta el 126
 apartir de ahi sigue el ascii extendido que no se usara https://elcodigoascii.com.ar/
*/
String LuisdaManejoSD::quitarCaracteresNoAscii(String lineaPorRevizar)
{
    char* charRevizar;
    int size = lineaPorRevizar.length();
    charRevizar = new char[size];

    lineaPorRevizar.toCharArray(charRevizar, size + 1);
    //Serial.println("Size " + String(size));
    //Serial.println("Size " + String(charRevizar));
    //Serial.println("Info " + lineaPorRevizar);
    //Serial.println(charRevizar);

    for (size_t i = 0; i < size; i++)
    {
        byte ascii = (byte)charRevizar[i];
        //Serial.println("Ascii " + String(ascii));

        if (ascii > 127) {
            charRevizar[i] = char(64);          //si el caracter pasa de 127 lo remplazamos con un "arroba" para saber en el servidor que los datos se estan corrigiendo por el metodo
        }

        if (ascii <31 && (ascii != 10 || ascii != 13)) {
            //si el caracter esta dentro de los caracteres de control pero de ellos no son salto de linea o retorno de carro entonces lo remplazamos por "espacio"
            charRevizar[i] = char(64);
        }

        
    }

    String temporal = String(charRevizar);
    delete[]charRevizar;                //liberamos la memoria del puntero
    return temporal;

}


void LuisdaManejoSD::leerUnRenglon()
{
    myFileL = SD.open(_LUISDA_SD_NOMBRE_LOCALIZACION);

    if (myFileL) {
        
        if (myFileL.available()) {
            Serial.println(String(myFileL.position()));
            myFileL.seek(36);
           Serial.println(String(myFileL.position()));
            
           // byte caracter = myFileL.read();            
            String lectura = myFileL.readStringUntil('\n');
            Serial.println(lectura.length());
            Serial.print(lectura);
            //Serial.print(String(caracter));
            //Serial.println("-AA--");
            Serial.println(String(myFileL.position()));



            /*
        Por ejemplo hice un archivo asi:
        123456789
        223456789
        323456789
        423456789
        523456789
        623456789
        723456789
        823456789
        923456789
        102345678
        102345678
        1869170031697544,29/1/202020:51:42,19.795300,-99.878601,2570.7,2.16,4.10,1.23,11,3D
        2869170031697544,29/1/202020:53:11,19.795300,-99.878799,2569.3,6.96,,1.04,11,3D

        
              1     2   3   4   5   6   7   8   9  \r  \n       2     2   3   4   5   6   7   8   9  \r  \n
             p0|   p1 | p2 |p3 |p4 |p5 |p6 |p7| p8| p9| p10   |p11| p12 |p13|p14|p15|p16|p17|p18|p19|p20|p21


        Importante ese archivo los renglones que tienen 123456789 los edite en el editor de trexto de la computadora,
        No entendia porque se inserto el retorno de carro o quien lo inserta ese no deberia de estar ahi, pero el editor de texto lo inserta en cuanto tu das enter,
        cuando yo creo un nuevo renglon en el archivo sd
        entiendo que lo que ocurre es esto si tu tienes la informacion y solo se pusiera el salto de linea que quieres la siguiente informacion se escribiria
        123456789
                 123456789

        Pero los ultimos datos de coordenadas los inserto el arduino a la memoria sd y en ellos no se inserta un retorno de carro porque n ellos solo programe el \n
        envio al final de la informacion un salto de linea, \n. Puedes ver byte a byte con este metodo para darme cuenta me fui colocando en la posicion que queria ver
        con la funcion seek y leyendo byte, mas no un caracter entonces por ejemplo me coloque en la posicion 0 y me devolvio el byte 49 que en la tabla ascii corresponde al 1
        asi fui viendo hasta que me coloque en el caracter 9 donde se supone que deberia de estar el salto de linea y lo que me encuentro es que hay el byte 13 que equivale al retorno de carro \r
        porque ese fue quien lo ingreso el editor de texto



        Ahora el problema que me estaba ocurriendo, es que calculamos cuantos bytes de informacion cooerente vvamos a enviar
        pero de alguna manera llegaba al servidor la ultima linea a medias porque?       




        Me explico mi metodo me decia 
        1869170031697544,29/1/202020:51:42,19.795300,-99.878601,2570.7,2.16,4.10,1.23,11,3D
        2869170031697544,29/1/202020:53:11,19.795300,-99.878799,2569.3,6.96,,1.04,11,3D
        3869170031697544,29/1/2020 20:54:14,19.795300,-99.878700,2564.8,9.19,283.99,1.05,11,3D
        4869170031697544,29/1/2020 20:55:16,19.795300,-99.878700,2565.2,8.06,274.0599
        5869170031697544,29/1/202020:51:42,19.795300,-99.878601,2570.7,2.16,4.10,1.23,11,3D
        6869170031697544,29/1/202020:53:11,19.795300,-99.878799,2569.3,6.96,,1.04,11,3D
        7869170031697544,29/1/2020 20:54:14,19.795300,-99.878700,2564.8,9.19,283.99,1.05,11,3D
        8869170031697544,29/1/2020 20:55:16,19.795300,-99.878700,2565.2,8.06,274.0599
        9869170031697544,29/1/202020:51:42,19.795300,-99.878601,2570.7,2.16,4.10,1.23,11,3D
        10869170031697544,29/1/202020:53:11,19.795300,-99.878799,2569.3,6.96,,1.04,11,3D
        11869170031697544,29/1/2020 20:54:14,19.795300,-99.878700,2564.8,9.19,283.99,1.05,11,3D
        12869170031697544,29/1/2020 20:55:16,19.795300,-99.878700,2565.2,8.06,274.0599

        aqui son 990 bytes, usamos el metodo readStringUntil('\n'); y al string le preguntabamos cuanto media y nos respondia un numero que ibamos sumando linea a linea leida
        hasta totalizar los 990 bytes, a esos 990 le sumabamos 3 que es el identificador que necesitamos para lapeticion http en este caso era d1=
        entonces en la peticion http le decimos, vas a recibir en el body 993 bytes que seria el equivalente a esto

        d1=1869170031697544,29/1/202020:51:42,19.795300,-99.878601,2570.7,2.16,4.10,1.23,11,3D
        2869170031697544,29/1/202020:53:11,19.795300,-99.878799,2569.3,6.96,,1.04,11,3D
        3869170031697544,29/1/2020 20:54:14,19.795300,-99.878700,2564.8,9.19,283.99,1.05,11,3D
        4869170031697544,29/1/2020 20:55:16,19.795300,-99.878700,2565.2,8.06,274.0599
        5869170031697544,29/1/202020:51:42,19.795300,-99.878601,2570.7,2.16,4.10,1.23,11,3D
        6869170031697544,29/1/202020:53:11,19.795300,-99.878799,2569.3,6.96,,1.04,11,3D
        7869170031697544,29/1/2020 20:54:14,19.795300,-99.878700,2564.8,9.19,283.99,1.05,11,3D
        8869170031697544,29/1/2020 20:55:16,19.795300,-99.878700,2565.2,8.06,274.0599
        9869170031697544,29/1/202020:51:42,19.795300,-99.878601,2570.7,2.16,4.10,1.23,11,3D
        10869170031697544,29/1/202020:53:11,19.795300,-99.878799,2569.3,6.96,,1.04,11,3D
        11869170031697544,29/1/2020 20:54:14,19.795300,-99.878700,2564.8,9.19,283.99,1.05,11,3D
        12869170031697544,29/1/2020 20:55:16,19.795300,-99.878700,2565.2,8.06,274.0599

        Si tu metes eso en donde te cuente los bytes te dice que son 993 bytes correctamente pero entonces al servidor solo llegaba asi


        ------------------------------
                                                                                                --> este salto de linea tambien llega y equivale al salto que envia el metodo enviarcomandosinevaluar cuando envia la cabecera http
        1869170031697544,29/1/202020:51:42,19.795300,-99.878601,2570.7,2.16,4.10,1.23,11,3D
        2869170031697544,29/1/202020:53:11,19.795300,-99.878799,2569.3,6.96,,1.04,11,3D
        3869170031697544,29/1/2020 20:54:14,19.795300,-99.878700,2564.8,9.19,283.99,1.05,11,3D
        4869170031697544,29/1/2020 20:55:16,19.795300,-99.878700,2565.2,8.06,274.0599
        5869170031697544,29/1/202020:51:42,19.795300,-99.878601,2570.7,2.16,4.10,1.23,11,3D
        6869170031697544,29/1/202020:53:11,19.795300,-99.878799,2569.3,6.96,,1.04,11,3D
        7869170031697544,29/1/2020 20:54:14,19.795300,-99.878700,2564.8,9.19,283.99,1.05,11,3D
        8869170031697544,29/1/2020 20:55:16,19.795300,-99.878700,2565.2,8.06,274.0599
        9869170031697544,29/1/202020:51:42,19.795300,-99.878601,2570.7,2.16,4.10,1.23,11,3D
        10869170031697544,29/1/202020:53:11,19.795300,-99.878799,2569.3,6.96,,1.04,11,3D
        11869170031697544,29/1/2020 20:54:14,19.795300,-99.878700,2564.8,9.19,283.99,1.05,11,3D
        12869170031697544,29/1/2020 20:55:16,19.795300,-99.878700,2565.2,8.
        -----------------------------------



        Si contamos son 978 bytes la ultima linea quedo incompleta, obio ya no va el identificador d1= porque este el servidor lo retira en la instruccion Request de php 
        y solo sacas los datos despues del d1=

        entonces si analizamos esa parte al ultimo renglon le faltan estos datos        .06,274.0599
        los cuales son 12 caracteres, es decir nos pasamos por 12 caracteres del numero de bytes que le dijimos que enviariamos al servidor en este caso 993bytes 
        de alguna manera al enviar la informacion al sim y este enviarlo al servidor envio 1005 bytes, el servidor solo toma los 993 bytes que le dijimos en la peticion http
        y los demas los desecha por eso el ultimo renglon esta afectado, significaba que los bytes intrusos van en las lineas superiores, ahora
        nos damos cuenta que el servidor recibio 12 renglones, pero al inicio recibio el salto de linea que envia el metodo enviar comando sin evaluar cuando envia la cabecera http       
        entonces de los primeros 11 renglones (1-11) hay un byte que no estamos contando, 
        entonces el otro intruso es el salto de linea que de alguna manera se nos estaba quedando al inicio

        por lo tanto concuerda con mi teoria y lo que creo que esta pasando, el metodo readStringUntil('n') nos devuelve un string hasta el salto de linea pero sin incluir el salto
        porque por ejemplo si yo cuento 
        Leo con readStringuntil('\n') aunque venga el retorono de carro o lo que sea que haya antes del salto de linea
        123456789\r\n

        el metodo readStrungUntil nos devolvera solo= 123456789\r
        el salto de linea no lo incluye en el string devuelto
        por lo tanto nos deberia indicar el leght de 10




        ******descubrimiento quien es el problema**************


        Ya encontre quien es quien pone el retorono de carro, es el editor de texto de la compu porque el primer renglo lo modifique en la laptop y los renglones de abajo los
        imprimio el gps, en el primer renglon si viene un retorno de carro y un salto de linea, en y apartir de ahi solo son saltos de linea es decir el gps hiso bien su trabajo
        
        ------------------------
        102345678                                                                               --> este lo agregamos en el editor de texto
        Modulo gprs esta apagado                                                                                --> estos los agrego arduino hacia abajo

        869170031697544,31/1/2020 13:23:19,19.795300,-99.878799,2571.8,0.10,0.70,0.94,14,3Derror 5 segundos
        869170031697544,31/1/2020 14:33:22,19.795300,-99.878799,2559.2,0.38,,0.98,14,3D

        -----------------------------------



        ahora, analizare el metodo leght e intentare comprobar que readStringUntil no esta incluyendo el salto de linea en el string devuelto

        colocare el puntero en laposicion 0
        leera con StringUntil hasta el \n salto de linea entonces deberiamos de tener
        102345678\r\n
        una longitud de 11 pero pues no ocurre asi solo nos enterga 10
        y por ejemplo en la parte de abajo si yo imrpimo serial print sin ponerle "ln"
        y justo abajo imprimo algo nuevo

       se supone que el primer serial prin imprimiria en mi monitor la informacion 
       102345678\r\n
       y el segundo serial prin lo demas que en mi caso es -AA-
       se supone que se -AA- deberia de salir en automatico en el nuevo renglon de abajo porque el string que se lee lleva incluido el salto de linea
       Serial.print(lectura);
       Serial.println("-AA--");
       102345678
       -AA-

       pero no ocurre eso, lo que pasa es que imprime esto
       102345678-AA-
       lo cual me esta dando a entender que el metodo readStringUntil, lee hasta el salto de linea pero no lo incluye en la devolucion del string
       por lo tanto solo tenemos 
       102345678\r
       que concuerda con los 10 de longitud que nos indica su leght
       y concuerda con la impresion que genera.

       ahora leere el segundo renglon que es modulo gprs esta apagado
       se supone que aqui ya no viene incluido el retorno de carro pues el arduino esta programado solo para insertar un \n cada nueva linea de datos ingresada a la sd

       modulo gprs esta apagado\n
       
       si esta tomando en cuenta el salto de linea deberia de ser un largo de 25 pero solo retorna 24
       es decir el readStringUntil solo retorna 

       modulo gprs esta apagado

       entonces este es el erro que yo estoy teniendo, estoy contando solo los datos sin el salto de linea
       si me dice que son 990 son los datos en bruto todo lo que haya antes del saldo de linea se cuentan pero no se cuenta el salto de linea
       pero lo que no estamos contando son los saltos de linea!!! entonces esos 990 hay que sumarle los saltos de linea que se encontraron pufff

       y entonces de alguna manera cuando yo 

       1    0   2   3   4   5   6   7   8   \r  \n
       p0| p1 | p2 |p3 |p4 |p5 |p6 |p7| p8| p9| p10   
       m    o   d   u   l   o   ||  g   p   r   s   ||  e   s   t   a   ||  a    p   a   g   a   d   o  \n
      |p11|p12|p13|p14|p15|p16|p17|p18|p19|p20|p21|p22|p23|p24|p25|p26|p27|p28|p29|p30|p31|p32|p33|p34|p35|
       \n
      |p36|


      Ahora un punto importante leemos el String colocamos el puntero en la posicion 11 leemos hasta el salto de linea el metodo nos devuelve
      modulo gprs esta apagado              -> con una longuitud de 24 porque no esta tomando en cuenta el salto de linea de la posicion p35

      pero ahora lo cuirios es de que cuando imprimes la posicion actual del puntero despues de leer ese string te dise que la siguiente posicion es la 36
      lo que significa que lee hasta el salto de linea, te entrega un string pero te lo entrega sin el salto de linea, y al colocarce en la siguiente posicion como realmente
      leyo de la posicion 11 a la 35, el te dice que la siguiente posicion sera la 36. Eso es el problema jaja el lee hasta el salto de linea si no te lo entrega en el string resultante
      es otro asunto, entonces al parecer lo mas facil sera sumarle un + 1 a la longitud de cada string resultante para saber los bytes completos

      O la otra opcion es no usar el read string en el metodo que calcula los bytes totales a enviar menores a 1000 sino mas bien recorrer byte a byte
      y analizar cada byte si un byte corresponde al salto de linea guardaremos esa posicion para saber que hasta ahi es informacion "cooerente"

      seria mas claro en dado caso de no entender el porque sumaremos el + 1 al string.leght, pero un poco mas complejo y modificariamos el codigo que ya tenemos creado  
      

      Ahora importantre en el metodo que envia los datos al sim808 se usa el mismo metodo readString until, quiere decir que los string que le enviemos al modulo
      van a ir sin el salto de linea, 

      Entonces hay que añadir un salto de linea, pero en el caso de mi metopdo estamos usando la funcion envioar comando sin evaluar en el cual ya programammos
      un salto de linea que se envia en automatico, entonces ese es quien esta enviando el salto de linea

        */
        }
        
        myFileL.close();
    }
}