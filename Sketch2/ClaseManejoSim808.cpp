#include "ClaseManejoSim808.h"

/*

ClaseManejoSim808::ClaseManejoSim808(uint8_t pinRx, uint8_t pinTx, uint8_t pinDeApagado, uint8_t pinDeReset):SoftwareSerial(pinRx,pinTx)	//le definimos al construtor de la clase padre que tome los datos de este constructor
{
	this->pinDeApagado = pinDeApagado;
    this->pinDeReset = pinDeReset;
	//y a nuestro constructor hijo solo le pedimos que defina el pin de apagado del modulo



    
}

*/
ClaseManejoSim808::ClaseManejoSim808(uint8_t pinDeApagado, uint8_t pinDeReset) {
    this->pinDeApagado = pinDeApagado;
    this->pinDeReset = pinDeReset;
    //y a nuestro constructor hijo solo le pedimos que defina el pin de apagado del modulo



}

void ClaseManejoSim808::begin(long speed)
{
    
        //aplicamos el poliformismo para el metodo begin
        //SoftwareSerial::begin(speed);			//llamamos al metodo begin padre

    Serial3.begin(speed);

        pinMode(pinDeApagado, OUTPUT);			//con nuestro polimormismo añadimos las declaraciones del pin que apagara y encendera el sim
        digitalWrite(pinDeApagado, LOW);

        pinMode(pinDeReset, OUTPUT);			//con nuestro polimormismo añadimos las declaraciones del pin que apagara y encendera el sim
        digitalWrite(pinDeReset, HIGH);       //COMO SE REINICIA EL MODULO ENVIANDO ESTE PIN A TIERRA LO PONEMOS EL ALTO

        this->baudiosConfigurados = speed;
        
    
}






/*
Puedes enviar alguna de estas constantes para calcular en automatico cuanto tiempo el procesador debe esperar por la respuesta
dependiendo del numero de bytes
_MENOR_A_32_BYTES, _MENOR_A_64_BYTES, _MENOR_A_128_BYTES, _MENOR_A_258_BYTES
O puedes enviar un long especificando cuanto tiempo quieres esperar manualmente por ejemplo
1000 equivalente a que el procesador se quedara esperando 1 segundo la respuesta

La RESPUESTA ESPERADA si pj. tu respuesta contiene algo como CONNECT OK y programas un tiempo de 3000ms pero tu respuesta se recibe antes
porque continuar esperando los 3000ms? y no salir antes porque ya tienes la respuesta que te interesa?
Si no quieres usar la respuesta esperada envia nullptr

El tiempo a esperar maximo estara definido si tienes configurado watch dog en mi caso lo tengo configurado a 8s entonces prefiero quedarme como maximo en 5 segundos de espera
para que el wathch dog no se active y reinicie arduino

Al hacer esto dotamos la parte de donde se conecta al GPRS para enviar informacion de mucha mas autonomia por ejemplo cuando se conecta al servidor y este le responde CONNECT OK
lo hace en diferente tiempo a veces es rapido a veces tarda mas entonces lo que hacia era darle por ejemplo 2 segundos, entonces a fuerzas el procesador esperaba los 2
segundos para dejar de leer la info aunque el connect ok llegara antes y ocurria el problema que algunas veces no llegaba en 2 segundos la respuesta
entonces mi solucion era ahora darle mas tiempo 5 segundos, el problema es que detienes 5s el procesador cada que quieres hacer una conexion algo malo, porque lo necesitas para
hacer otras tareas, ahora le defino los mismos 5 segundos pero le envio como parametro el CONNECT OK que quiero recibir, una vez que la respuesta lo tiene, se sale del bucle 
y que cren? aunque yo programe 5segundos la respuesta llega en 700ms 500ms 900ms, si la respuesta no llega en 5 segundos entonces salimos del bucle, pero vez? ahora
el procesador espera el tiempo justo y no espera mas de la cuenta inecesariamente.

Ese era mi problema con las conecciones gprs que tenia que detener en vano mucho tiempo el procesador

Pero cuidado con la respuesta Esperada ya que si por ejemplo tu esperas recibir 

+CGPSINF: 4,1947.718100,N,9952.730700,W,005757.000,A,A

y tu pones que quieres salir en cuanto encuentre la palabra +CGPSINF:, efectivamente saldras del bucle pero! no segiras leyendo los bytes que aun te esta enviando el modulo
con las coordenadas, entponces en tu respuesta solo tendras +CGPSINF: porque las coordenadas se dejaron de leer y bueno se quedaran almacenadas en el buffer serial, pero en el array de respuesta no estaran guardados
Asi solo es bueno usarlo cuando la respuesta que esperas esta al final de toda la informacion o cuando no te interesa la demas informacion mas que tu respeusta esperada

Ahora necesito corregir eso, debido a que necesito usar la parte de la respuesta esperada pero necesito despues que se recibio la respuesta esperada seguir leyendo los datos que siguien entrando 
por el buffer serial, y no que me llege trunca la respuesta asi por ejemplo la respuesta esperada ya no tendra que estar obligatoriamente al final de lo que quieres recibir sino que
podria estar al inicio, o a la mitad del mensaje, y cuando se reciba aun asi leer los datos del buffer despues de la respeusta esperada.
Para hacer esto una vez que se reciba la respuesta esperada, ya no se rompera el bucle sino que activara un temporizador de pj. 5ms
cada que se tengan datos availables en el puerto serie este timer se reiniciara, cuando ya no haya datos disponibles en el buffer
esperara los 5ms y cuando se cumplan ahora si se saldra del bucle, que quiere decir esto? 
que se activa el timer porque se encontraron datos que corresponden con la respuesta esperada, siempre y cuando sigan entrando bytes al buffer serial
no saldremos del bucle, una vez que ya no hay entrada de datos en el buffer serial entonces saldremos del bucle, y asi obtendremos la informacion que esta despues
de la respuesta esperada, y ademas mantendremos la eficiencia al no tener que esperar el tiempo maximo que programammos en el tiempoDeEspera

Ahora solo queda determinar si los 5ms estanb ien en las pruebas a 115200 baudios va perfecto hasta siento que podriamos bajarlo de 5 a 4 o 3


*/
char* ClaseManejoSim808::enviarComandoSinEvaluar(char* comando, long tiempoEspera, char* respuestaEsperada) 
{
    //retornar char arrays 
   //https://es.stackoverflow.com/questions/102651/regresar-un-char-desde-una-funci%C3%B3n
//el tamaño maximo de envio de datos por escritura es de 63 bytes
   /*
   en la mayoria de los comandos la respuesta no supera los 63 bytes entonces podemos dejar que el buffer se llene
   poniendo un retardo despues de la escrutura y asi el bufer recibira todos los bytes llenando su memoria y al final leerlos todos de un jalon
   con un bucle for,
   pero con el comando que solicita coordenadas la cosa cambia la respuesta es mas grande que 63 bytes, exactamente envia 100 caracteres de a un byte por caracter
   entonces lo mejor es
   dejar al arduino en un bucle dando vueltas durante 150ms que es tiempo suficiente para que llegen todos los datos
   y cada que haya un bite disponible que lo meta al array, asi el array tendra toda la respuesta no importa que pase de 63 bytes
   porque con cada byte que leamos del buffer este se elimina y el buffer se vacia
   */


    Serial.println(F("\n-Metodo enviar comando sin evaluar-\n"));
    if (respuestaEsperada != nullptr) {
        Serial.print(F("Buscaremos la respuesta: "));
        Serial.println(respuestaEsperada);
    }
   
    wdt_reset();                                        //cada que se llame a esta funcion reiniciamos el watch dog
    int x = 0;
    bool primeraVez = true;
    memset(respuesta, '\0', sizeof(respuesta));                   // Llenamos y si ya tenia datos almacenados ahora lo limpiamos y ponemos valores vacios
    long millisAntesDeBucle;
    long millisDespuesDeBucle;
    long tiempoLimiteCalculado = calcularTiempoDeEsperaDelProcesador(tiempoEspera);
    


    while (Serial3.available() > 0) Serial3.read();                 // Limpia el buffer de entrada de cualquier dato que pueda haberse quedado en el buffer es decir basura
    Serial3.write(comando);                                 // Envia el comando AT
    Serial3.write("\n");



       


    
    
    retardo1->setRetardo(tiempoLimiteCalculado);             
    retardo1->inicializarMillis();      

    this->retardoNoMasEntradasEnElBuffer->setRetardo(5);
    this->retardoNoMasEntradasEnElBuffer->inicializarMillis();


   
    millisAntesDeBucle = millis();
    Serial.println(String(millisAntesDeBucle));               //imprimimos los millis antes de entrar al bucle y ver si estan funcionando los retardos variables 

    //Serial.println("Entrando en bucle");    
    // Espera una respuesta
    do {
        // si hay datos el buffer de entrada del UART lee y comprueba la respuesta
        if (Serial3.available() != 0)
        {
            //Serial.println(String(x));
            //delay(1);

            if (x < sizeof(respuesta)) {                      //protejemos el array de un posible desbordamiento si la respuesta es mas larga que el tamaño del array, tendremos null pointer exeption y se detendra arduino. Asi solo leeremos 100 caracteres maximos en el array
                respuesta[x] = Serial3.read();
                x++;

                this->retardoNoMasEntradasEnElBuffer->reiniciar();
            }
            
            
        }

        if (respuestaEsperada != nullptr) {


            if (strstr(respuesta, respuestaEsperada) != nullptr) {

                if (primeraVez) {
                    Serial.print(F("Respuesta encontrada activando Timer"));
                    Serial.println(String(millis()));
                    this->retardoNoMasEntradasEnElBuffer->reiniciar();
                    primeraVez = false;
                }
                

                if (retardoNoMasEntradasEnElBuffer->seHaCumplido(false)) {
                    Serial.println(F("Ya no hubo mas entrada de datos el timer se desbordo, salimos del bucle"));
                    break;  //salimos del bucle en cuanto se recibio la respuesta que se esperaba
                }
                
                
            }
        }
        
    }
    
    while (!retardo1->seHaCumplido(false));
    //el retardo esta hecho con millis, retorna true si ha pasado el tiempo definido en este caso 200ms, pero lo invertimos para que cuando aun no se cumpla sea true y el while se siga ejecutando
    //es decir el bucle while se ejecutara durante 200 ms, cuando hayan pasadon los 200ms se obtendra false en el while y se dejara de leer el buffer serial en teoria
    //debieron haber entrado todos los datos correctamente
    //lo ponemos false para que se no se reinicie solo, ya que sera un retardo que se inicializara aqui mismo en la funcion en la parte de arriba


    millisDespuesDeBucle = millis();



    


    Serial.println(String(millisDespuesDeBucle));       //imprimimos los millis al salir del bucle para constatar que el procesador solo se quedo el tiempo que le definimos dependiendo del tamaño de la respuesta
    //Serial.println(String(x));   

    Serial.print(F("El procesador espero en el bucle durante: "));
    Serial.print(String(millisDespuesDeBucle-millisAntesDeBucle));
    Serial.print(F(" De un tiempo limite de: "));
    Serial.print(String(tiempoLimiteCalculado));
    Serial.print(F(" Se activo busqueda de respuesta:"));
    Serial.println((primeraVez) ? "false" : "true");    //invierto los resultados porque primera vez sera false si se activo


    Serial.println(respuesta);



    
    




 


    //hay un error, depronto funciona depronto el array devuelto esta vacio, ya entendi lo que ocurre y no es problema de punteros, lo que pasa
    //es que el array respuesta esta declarado dentro de esta funcion, este ambito, etnocnes el array que declaro solo existe dentro de esta funcion
    //lo llenamos pero una vez que esta funcion termina, retorna un puntero que apunta al array, pero como la funcion ya termino
    //el array se destruye junto con el ambito y las demas variables y el puntero que retornamos, efectivamente tiene laposicion de memoria del array
    //pero aqui ya se destruyo o se lleno de basura, cunado queremos leer el char array en otra funcion este ya esta vacio
    //no se si la mejor opcion seria declarar el array como un campo dentro de esta clase
    //https://foro.elhacker.net/programacion_cc/funcion_que_retorne_char_array-t367266.0.html hasta bajado decia lo de el ambito

    Serial.println(F("\n-Fin Metodo enviar comando sin evaluar-\n"));
    wdt_reset();//si por ejemplo nos mandan a esperar 5s el watch dog esta en 8s reiniciamos aqui para que no se reinicie arduino

    return respuesta; //se retorna un puntero hacia la direccion de memoria donde esta el array
}

/*
Puedes enviar alguna de estas constantes para calcular en automatico cuanto tiempo el procesador debe esperar por la respuesta
dependiendo del numero de bytes
_MENOR_A_32_BYTES 
_MENOR_A_64_BYTES 
_MENOR_A_128_BYTES 
_MENOR_A_258_BYTES 

O puedes enviar un long especificando cuanto tiempo quieres esperar manualmente por ejemplo 
1000 equivalente a que el procesador se quedara esperando 1 segundo la respuesta



Este metodo resuelve mi problema de que queria que el procesador pudiera esperar un tiempo diferente a cada tipo de respuesta esperada
para por ejemplo no esperar 100ms fijos porquie por ejemplo un comando que te retorne 100bytes, y no esperar esos mismos 100ms en un comando que solo responde 8 bytes y que lo hace en 8ms, 

tambien depende de la velocidad a la que configures el puerto serie, entonces este moetodo esta hecho para que dependiendo de la velocidad definida de mas tiempo o menos tiempo
porque claro 57600 tardara mucho menos que 9600 donde tendremos que esperar por mas tiempo las respuestas
*/
long ClaseManejoSim808::calcularTiempoDeEsperaDelProcesador(long cantidaDeBytesEsperadosComoRespuesta)
{
    /*
Definimos nuestro tiempo de retardo dependiendo del tamaño de la respuesta
Este calculo esta hecho a 19200 baudios de velocidad cada byte tarda 50us en enviarce
si se disminuye la velocidad de comunicacion con el modulo tardara mas y es probable que las respuestas no entren
ahora si se duplica la velocidad que creo seria lo mejor tardaria mucho menos y quizas podriamos estos retardos disminuirlos

Mis puruebas con el osciloscopio enviando el comando
AT ay de regreso como   responde como 8 bytes       tiempo a	19200 baudios   8.8ms		    38400 4.97ms	    57600 3.94ms
AT+CGPSSTATUS?          responde como 52bytes       tiempo a    19200		    37.5ms          38400 19.2ms        57600 12.9ms
AT+CGPSINF=2            responde como 90bytes                   19200           59.8ms          38400 30.7ms        57600 20.3ms
AT+CGPSINF=0            responde como 108bytes                  19200 baudios   68.2ms          38400 34.6ms        57600 23.2ms
AT+AT+CGPSINF=128       responde como 62bytes                   19200           44.6ms          38400 22.6ms        57600 15.2ms

RESPUESTA? SI QUEREMOS BAJAR LOS TIEMPOS QUE EL MICRO SE QUEDA DENTRO DEL LOOP ESPERANDO LA RESPUESTA, HAY QUE INCREMENTAR LA VELOCIDAD AL DOBLE

YA SEA QUE USEMOS EL RETARDO VARIABLE DEPENDIENDO DE EL COMANDO QUE ENVIEMOS SE VUELVE UN POQUITO MAS COMPLEJO PORQUE HAY QUE SABER EL TAMAÑO DE LA RESPUESTA
Y SI NO ES CORRECTO PODRIAMOS NO LEER LOS DATOS
AUNQUE TAMBIEN GANAR UN POCO MAS DE INDEPENDENCIA DEL PROCESADOR PARA HACER OTRAS COSA ESTARA PERFECTP
AL INICIO COMENZAMOS DANDOLE UN VALOR DE RETARDO FIJO DE 200MS DESPUES LO BAJAMOS A 100 AJUSTANDONOS A LOS 68MS DE LA RESPUESTA MAS LARGA
Y AHORA PONIENDO LA VELOCIDAD AL DOBLE PODRIAMOS PERFECTAMENTE DEJARLO A SOLO 50MS PARA CUALQUIER RESPUESTA,

LA PREGUNTA ES COMO DECIDIMOS HACERLO JAJA COMO SEA YA TODO EL CODIGO PUEDE FUNCIONAR, SI NOS ELEGIMOS POR UNA O POR OTRA SIMPLEMENTE
CAMBIAMOS LA LINEA DONDE LE PONEMOS SETRETARDO EN ENVIARCOMANDOSIN EVALUAR Y LA DEJAMOS FIJA A 50MS

una solucion en mas simple y otra mas compleja
si por ejemplo definimos 50ms fijos el metodo que consulta la localizacion hace 3 usos del enviarComandoSInEvaluar entonces son 150ms que el procesador se queda en el bucle
ya se antes eran como 600ms lo bajamos a 300 y ahora al aumentar la velocidad lo podemos bajar a 150ms, quizas si usamos las velocidades variables podriamos esperar cuanto?
veamos hace 1 peticion de 64 bytes y 2 de 100 serian 128ms pues no es mucho jaja aunque falta que entremos a la parte de leer los sms o las peticiones web habria que ver
la mayor ventaja es cuando se hace la peticion del comando at, bueno creo esos 50ms ni se notaran para detener la pantalla a menos que hagamos 5 o 6 envios de comandos
antes en un mismo metodo

Ahora que pasara si elevas la velocidad a 56000 ? jaja consultar las velocidades aptas del modulo y no se quizas podria haber mas errores no cres?
ya hice la prueba claro los pulsos son mucho mas veloces pero en el mnitor serie deprotno salian caracteres que no tenian que salir como una k entre las coordenadas
quizas no sea tan comun pero arriesgarnos? 

SI QUIERES SABER CUANTO TIEMPO TARDARA CADA PULSO DE BIT SOLO DIVIDE
1,000,000us   que es igual a 1 segundo
entre los baudios
asi
1000000 / 9600 = 104.166us
1000000 / 19200 = 52.08us
1000000 / 38400 = 26.04us
1000000 / 57600 = 17.36us
1000000 / 115200 = 8.68us


AT+IPR=?
+IPR: (),(0,1200,2400,4800,9600,19200,38400,57600,115200,230400,460800)
el modulo soporta estas velocidades trate de poner 115200 pero creo que ya superamos la capacidad de algo proque no se reconocen los comandos correctamente u.u
nos vamos a quedar en 57600 es una muy buena velocidad!!

Hay que tener cuidad por ejemplo cuando envio los 497 bytes de todos los mensajes a la velocidad de 57600 baudios deberia tardar solo como 50ms o menos
pero hay tiempo que usa el modulo entre consultar 1 mensaje y enviarlo, haciendo que se tarde 242ms, puedes verlo en el osciloscopio




Experciencia:

 trabajamos mucho tiempo con 57600 baudios pero al consultar las coordenadas y recibir cadenas largas por parte del modulo
este metodo de calculo funciono perfecto, empece a tener errores casualmente con el final de la informacion de una cadena larga, crei que este metodo estaba calculando mal el
tiempo pero no era asi, al final baje la velociudad a 38400, y la cadena comenzo a llegar correctamente, creo que a mayor velocidad mayores probabilidades de datos corruptos
AT+CGPSINF=0
+CGPSINF: 0,1947.719600,9952.738400,2567.200000,20200218023938.000,0,10,0.293,4.00O  -- en este ultimo esta el error

-Fin Metodo enviar comando sin evaluar-

0.293,4.00O
4.00O           --era donde viene el angulo, y este se enviaba corrupto al servidor, crei que era mi metodo que limpiaba los datos pero no

no lograba corregirlo y al otro momento llegaba algo mal diferente por ejemplo en lugar de llegar una , en la velocidad enviaba 1.234423.2343
no tenia logica
AT+CGPSINF=0+CGPSINF: 0,1947.720700,9952.737800,2567.000000,20200218023940.000,0,10,0.8301.300

K

baje la veolcidad y los datos llegaron mas limpios
AT+CGPSINF=0
+CGPSINF: 0,1947.727800,9952.732100,2587.000000,20200218030307.000,0,9,1.092680,296.250000

OK


-Fin Metodo enviar comando sin evaluar-

1.092680,296.250000
296.250000
*/


    /*
    PARA CALCULAR EL TIEMPO NECESARIO DEPENDIENDO DE LOS BAUDIOS VOY A USAR LAS PUIREBAS QUE HICE CON EL OSCILOSCOPIO LAS MARCADAS ARRIBA EN EL COMENTARIO
    Sabemos que a 19200 un comando que retorna 8 bytes tardo en entregarse completa 8.8ms redondeemos a 10ms entonces a          9600 tardaria 20ms   9600/1200 = 8 *20ms = 160ms a 1200baudios
    Sabemos que a 19200 un comando que retorna 32 bytes tardo en entregarse completa 36ms redondeemos a 40ms entonces a          9600 tardaria 80ms   9600/1200 = 8 *80ms = 640ms a 1200baudios
    Sabemos que a 19200 un comando que retorna 62 bytes tardo en entregarse completa 44.6ms redondeemos a 50ms entonces a        9600 tardaria 100ms  9600/1200 = 8 *100ms = 800ms a 1200baudios
    Sabemos que a 19200 un comando que retorna 108 bytes tardo en entregarse completa 68.2ms redondeemos a 70ms entonces a       9600 tardaria 140ms  9600/1200 = 8 *140ms = 1120ms a 1200baudios

    como las velocidades todas son multiplos de la velocidad minima de 1200 soportada por el modulo si la respuesta de 62bytes tardaria a 1200 baudios 800ms cuanto tardaria a 38400 y a 57600?
    38400 /1200baudios = 32-> 800ms / 32 = 25ms
    57600 /1200 =        48-> 800ms / 48 = 16.6ms

    o se podria decir que para enviar 62 bytes tardo 800ms lo que es igual a 12.9ms por byte a 1200 redondeemos a 15ms por byte
    para enviar 32 tardo 640ms = 20ms por byte
    para 108 tardo 1120ms = 10.37ms

    Porque no es lineal o acorde 1 porque estoy aprocimando un poco los tiempos y 2 porque entre el envio de la peticion y el envio de la respuesta del modulo hay un tiempo muerto de respuesta
    el cual a veces era 1.7ms donde no hay bits, cuando se envian mas bytes este tiempo muerto solo ocurre una vez y se envian 120 bites que claro es mas rapido que esos 1.7ms de tiempo muerto

    Si nos damos cuenta este metodo nos da la cantidad que nos da en el osciloscopio, vamos a añadir tolerancias para procurar que la respuesta no entre tan justa
    */

    int multiplo = baudiosConfigurados / 1200;

    

    if (cantidaDeBytesEsperadosComoRespuesta == _MENOR_A_32_BYTES) {
        return 700 / multiplo;          //640 + tolerancia

    }else if (cantidaDeBytesEsperadosComoRespuesta == _MENOR_A_64_BYTES) {
        return 1000 / multiplo;          //800 + tolerancia

    }else if (cantidaDeBytesEsperadosComoRespuesta == _MENOR_A_128_BYTES) {
        return 1300 / multiplo;         //1120 + tolerancia

    }else if (cantidaDeBytesEsperadosComoRespuesta == _MENOR_A_258_BYTES) {
        return 2500 / multiplo;         //1120 * 2 = 2240 + tolerancia

    } else {
        //Si es cualquier otra cifra a enviar 
        return cantidaDeBytesEsperadosComoRespuesta;
    }
    
}











/*
Indica si el modulo Sim esta encendido
@RETURN
true   si la respuesta recibida al enviar el comando AT es Ok
falso si no hay nada de bytes disponibles o la respuesta esperada no es OK sino otra indicando que el modulo esta apagado
*/
bool ClaseManejoSim808::getEstatusModuloEstaApagadoEncendido()
{
    //BUSCAMOS EL SUB STRING OK DENTRO DEL CHAR ARRAY DE LA RESPUESTA si no lo encuentra devuelve vacio, en c++ null no existe xD
    //si no hubo bites disponibles obviamente el char que devuelve el metodo enviar comando sin evaluar estara vacio o la respuesta no concordara


    char* info = enviarComandoSinEvaluar(AT_BASICO, _MENOR_A_32_BYTES, "OK"); //BUSCAMOS LA PALABRA OK PARA QUE NO ESPERE TIEMPO EN VANO
    //Serial.println(info);
    
    
        if (strstr(info, "OK") == nullptr) {
            //si no se encontro la respuesta o no tenemos bytes disponibles

            //Serial.println("--ClaseManejoSim808::getEstatusModuloEstaApagadoEncendido--Respuesta no encontrada"); 
            return false;
        }
        else {
            //Serial.println("Respuesta encontrada");             
            return true;
        }
   
  

    
}





/*
@return:
    0 si esta apagado
    1 si esta encendido pero sin señal
    2 si esta encendido con señal 2D
    3 si esta encendido con señal 3D
    255 si no se recibe respuesta correcta o no hay bytes disponibles puede ser que todo el modulo este apagado que no se recibieron bytes

    Llevarlo con las definiciones
    GPS_APAGADO 0
    GPS_ENCENDIDO_SIN_SENAL 1
    GPS_ENCENDIDO_SENAL_MEDIA 2
    GPS_ENCENDIDO_SENAL_PERFECTA 3
    GPS_ERROR_COMANDO 255
*/
byte ClaseManejoSim808::getStatusGps()
{

    char* respuesta = enviarComandoSinEvaluar(AT_GPS_STATUS, _MENOR_A_64_BYTES, "OK");//buscamos el Ok para que no espere tiempo en vano
    //Serial.println(respuesta);
    //aqui podemos tener 4 respuestas posibles
    /*
        el modulo gps esta apagado, si es asi devuelve locationUnknow
        Si el gps esta encendido pero no hay ubicacion devuelve location no fix
        Si el gps esta encendido y tiene ubicacion pero solo 2D devuelve location 2D fix
        Si esta encendido y tiene ubicacion al 100% devuelve location 3d fix
    */

    if (strstr(respuesta, ESPERADA_RESPUESTA_GPS_STATUS_APAGADO) != '\0') {
        //si se encontro la respuesta 

        //Serial.println("Respuesta encontrada Modulo gps apagado");
         
        return GPS_APAGADO;
    }
    else if(strstr(respuesta, ESPERADA_RESPUESTA_GPS_STATUS_ENCENDIDO_SINLOCALIZACION) != '\0'){
        //si se encontro la respuesta 

        //Serial.println("Status gps encendido sin localizacion");            
        return GPS_ENCENDIDO_SIN_SENAL;
    }
    else if (strstr(respuesta, ESPERADA_RESPUESTA_GPS_STATUS_2D) != '\0') {
        //si se encontro la respuesta 

        //Serial.println("Status gps media senal");            
        return GPS_ENCENDIDO_SENAL_MEDIA;
    }
    else if (strstr(respuesta, ESPERADA_RESPUESTA_GPS_STATUS_3D) != '\0') {
        //si se encontro la respuesta 

        //Serial.println("Status gps senal perfecta");            
        return GPS_ENCENDIDO_SENAL_PERFECTA;
    }
    else {
        //si no se encontro ninguna respuesta
        //Serial.println("Status gps Respuesta no encontrada error comado o modulo sim sin conexion");
        return GPS_ERROR_COMANDO;
    }
}
//Return:
    //True si se activo correctamente el modulo
    //False si ocurrio algun error y la respuesta no fue recibida
bool ClaseManejoSim808::activarGps()
{
    if (strstr(enviarComandoSinEvaluar(AT_GPS_ACTIVA, _MENOR_A_32_BYTES, nullptr), ESPERADA_RESPUESTA_GPS_ACTIVA) == '\0') {
        //si no se encontro la respuesta o no tenemos bytes disponibles

        //Serial.println("--ClaseManejoSim808::activarGps--Respuesta no encontrada"); 
        return false;
    }
    else {
        //Serial.println(strstr(resp, "OK")); 
        //Serial.println("Respuesta encontrada");             
        return true;
    }
    
}
//Return:
    //True si se Apago correctamente el modulo
    //False si ocurrio algun error y la respuesta no fue recibida
bool ClaseManejoSim808::apagaGps()
{
    if (strstr(enviarComandoSinEvaluar(AT_GPS_APAGA, _MENOR_A_64_BYTES, nullptr), ESPERADA_RESPUESTA_GPS_ACTIVA) == '\0') {
        //si no se encontro la respuesta o no tenemos bytes disponibles

        //Serial.println("Respuesta no encontrada"); 
        return false;
    }
    else {
        //Serial.println(strstr(resp, "OK")); 
        //Serial.println("Respuesta encontrada");             
        return true;
    }
}






//Consulta la fehca del reloj RC del modulo 
    //la limpia y entrega un string con la fecha y hora dividida en una coma
    //El reloj RTC del modulo no es muy bueno tiene desfaces grandes en tiempos de apagado a menos quizas que la bateria ya este por morir, como sea le modulo que usare no tendra incluida la bateria para el reloj RTC,
    //asi que no lo usare, usare la hora que entregen los satelites gps
    //Return
    //20/01/08,22:21:22-06
String ClaseManejoSim808::consultarFechaHoraDelModulo()
{
    char* respuesta;
    respuesta = enviarComandoSinEvaluar(AT_RTC_FECHA_HORA_CONSULTAR,_MENOR_A_64_BYTES, nullptr);
    //Serial.println(respuesta);
    /*
    AT+CCLK?
    +CCLK: "20/01/08,21:38:31-06"
    
    OK
    */

    //Serial.println(strstr(respuesta, "+CCLK:"));
    
    String respuestaString(strstr(respuesta, "+CCLK:")); //creamos al cosntructor de string que recibe un char array para llenar el String porque la otra es una funcion que itere el array y concatenar el string  
    //ya que se encontro la respuesta el metodo nos devolvera solo: y eso lo guardamos en un String
    //+CCLK: "20/01/08,21:38:31-06"
    //
    //OK

    //creamos un sub String que tenga apartir del espacio hasta el salto de linea, le sumamos 1 para que no se incluya el espacio
    String listo = respuestaString.substring(respuestaString.indexOf(' ') + 1, respuestaString.indexOf('\n') - 1);//le restamos menos 1 en donde encontro el salto de linea para que no incluya ese salto de linea en el substring
    //Serial.println(listo);
    //"20/01/08,22:15:05-06" aqui ya tenemos solo esto
    //ahora vamos a quitar las comillas
    //Serial.println(String(listo.length()));
    String ultimo = listo.substring(1, (listo.length() - 1));
    //Serial.println(ultimo);
    //20/01/08,22:15:05-06
    //largo de 20
    return ultimo;
}


void ClaseManejoSim808::enciendeModuloConDelays()
{
    
        //no la recomiendo poruqe detenemos el proicesador por 2 segundos MEJOR LO BAJO A 1.4 SEGUNDOS A VER SI ES SUFICIENTE
        //eso podria hacer que el watch dog se detone, o si es llamado muchas veces
        //por algun mal codigo el arduino se estara deteniendo mucho y dejara de acer otras tareas
    wdt_reset();//reiniciamos el wathc dog para que no se resetee arduino
            digitalWrite(pinDeApagado, HIGH);
            delay(700);
            digitalWrite(pinDeApagado, LOW);
            delay(700);
        
    
}

void ClaseManejoSim808::resetDelModuloDelays()
{
    digitalWrite(pinDeReset, LOW);
    delay(100);
    wdt_reset();                                //aqnque tenemos 4 segundos aun asi lo reinicio xD
    digitalWrite(pinDeReset, HIGH);
}





/*

Entrega la informacion de ubicacion completa se hacen 3 envios de comando al modulo tomar en cuenta que el procesador se quedara en el bucle entre cada peticion de promedio 50ms
trabajando a 38400 baudios
cadenaDeRetorno
_CADENA_RETORNO_FECHAHORA_LAT_LON_ALT_VEL_DIR_PRECI_SATELL_CALIDAD           10/1/2020 18:33:29,19.795300,-99.878799,2558.7,3.12,264.589996,1.24,11,3D
_CADENA_RETORNO_LAT_LON_VEL_DIR                                              19.795300,-99.878799,3.12,264.589996

869170031697544,10/1/2020 18:33:29,19.795300,-99.878799,2558.7,3.12,264.589996,1.24,11,3D
 imei       ,   fechaHora,    latitud,   longitud,altitud,km/hr,grados,  preci, satelites,Location 3D Fix
Eliminamos el imei del resultado, para que cuando se almacenen los datos en la sd y se manden 10 de una sola vez el imei solo se mande una vez
asi ahorramos datos gprs

En este metodo creamos varias peticiones para juntar la inforacion que necesitamos debido a que diferentes comandos
entregan informaciones diferentes por ejemplo un comando tiene las coordenadas en decimales, y no en grados lo cual nos ayuda a 
no tener que convertirlas pero, no lleva la rotacion por ejemplo

pero entre varios comandos obtengo la informacion que necesito y la uno en un string


*/
String ClaseManejoSim808::getLocalizacion(byte cadenaDeRetorno, int zonaHoraria)
{
    String lat = "";
    String lon = "";
    String satelites = "";
    String precision = "";
    String altitud = "";
    bool peticion1 = false;

    String veloci = "";
    String angulo = "";
    bool peticion2 = false;

    //String imei = "";
    //bool peticion3 = false;


    String senalGps = "";
    bool peticion4 = false;




    // Obtenemos las coordenadas en grados decimales
    char* respuesta = enviarComandoSinEvaluar(AT_GPS_COORDENADAS_2_GPGGA_HORA_LAT_LONG_DECIMAL, _MENOR_A_128_BYTES, nullptr);
    /*
        AL ENVIAR ESTE COMANDO OBTENDREMOS
        AT+CGPSINF=2
        AT+CGPSINF=2
        +CGPSINF: 2,222358.000,0019.7952,N,-0099.8788,W,1,5,1.67,2572.6,M,-9.0,M,,
           
        OK

        Name 		Example Data 				Description

				Sentence	Identifier 				$GPGGA 	Global Positioning System Fix Data
				Time 		222358.000 						00:56:16 Z
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

    if (strstr(respuesta, "+CGPSINF: 2") != nullptr) {
        String informacion(strstr(respuesta, "+CGPSINF: 2"));       //primero uso el que tiene las coordenadas en decimal
        /*
        +CGPSINF: 2,222358.000,0019.7952,N,-0099.8788,W,1,5,1.67,2572.6,M,-9.0,M,,
           
        OK
        */
        
        /*
            creamos un sub String que tenga apartir del espacio hasta el salto de linea, le sumamos 1 para que no se incluya el espacio
            le restamos menos 1 en donde encontro el salto de linea para que no incluya ese salto de linea en el substring
        */
        informacion = informacion.substring(informacion.indexOf(' ') + 1, informacion.indexOf('\n') - 1);
        //Serial.println(informacion);
        /*
        2,222358.000,0019.7952,N,-0099.8788,W,1,5,1.67,2572.6,M,-9.0,M,,
        modo 2
        hora UTC 222358.000
        latitud 0019.7952,N
        longitud -0099.8788,W
        gpsFix 1
        satellites 5
        presision 1.67
        altitud 2572.6M
        */
        //ahora dividiremos por las comas, hay una funcion que es strtok algo similar a split pero hay que convertir los String a charArray

        
        informacion.remove(0, informacion.indexOf(',') + 1);                     //para quitar el modo 2 la coma tambien
        //Serial.println(listo);                                  //222358.000,0019.7952,N,-0099.8788,W,1,5,1.67,2572.6,M,-9.0,M,,

        informacion.remove(0, informacion.indexOf(',') + 1);                        //quitamos la hora no la quiero poruqe usaremos el metodo para obtener la hora
        //Serial.println(listo);                                          //0019.7953,N,-0099.8787,W,1,4,1.79,2563.7,M,-9.0,M,,



        lat = informacion.substring(0, informacion.indexOf(','));
        //Serial.println(lat);
        informacion.remove(0, lat.length() + 1);                              //quitamos la latitud
        //Serial.println(listo);

        informacion.remove(0, informacion.indexOf(',') + 1);                        //quitamos la N junto con su coma
        //Serial.println(listo);                                          //-0099.8789,W,1,4,2.12,2561.6,M,-9.0,M,,




        lon = informacion.substring(0, informacion.indexOf(','));
        //Serial.println(lon);
        informacion.remove(0, lon.length() + 1);                              //quitamos la long
        //Serial.println(listo);

        informacion.remove(0, informacion.indexOf(',') + 1);                        //quitamos la W junto con su coma
        //Serial.println(listo);                                          //1,4,2.12,2561.6,M,-9.0,M,,




        informacion.remove(0, informacion.indexOf(',') + 1);                              //quitamos el estatos si es 0 datos invalidos, 1 gps fix,2 DGPS fix
        //Serial.println(listo);                                                //4,2.12,2561.6,M,-9.0,M,,





                                                                      
        informacion.remove(0, informacion.indexOf(',') + 1);                              //quitamos los satelites
        //Serial.println(listo);                                          //2.12,2561.6,M,-9.0,M,,


        precision = informacion.substring(0, informacion.indexOf(','));
        //Serial.println(precision);
        informacion.remove(0, precision.length() + 1);                              //quitamos la precision
        //Serial.println(listo);                                          //2561.6,M,-9.0,M,,


        altitud = informacion.substring(0, informacion.indexOf(','));
        //Serial.println(altitud);
        informacion.remove(0, altitud.length() + 1);                              //quitamos la precision
        //Serial.println(listo);                                          //M,-9.0,M,,

        //el resto del string ya no nos sirve
        peticion1 = true;


    }












    //ahora haremos otra peticion para obtener el angulo de direccion del gps
    respuesta = enviarComandoSinEvaluar(AT_GPS_COORDENADAS_0_LAT_LONG_GRADOS_FECHA_HORA, _MENOR_A_128_BYTES, nullptr);
    //PUDE HABER USADO ESTE DESDE ARRIBA PERO ME PREOCUPA QUE 1 TENGO QUE CONVERTIR LAS COORDENADAS A DECIMAL, NO ES DIFICIL
    //Y LA MAS IMPORTANTE ES QUE ESTA NO INDICA N O W Y TAMPOCO VIENEN EN -99, ES DECIR SI EL GPS CAMBIA A OTRA UBICACION
    //LEJOS DE AQUI, PODRIA TENER PROBLEMAS CON SABER SI ES NUMERO POSITIVO O NEGATICVO. VER LA DESCRIPCION DE ESE COMANDO AHI EXPRESE TODO
    /*
    AT+CGPSINF=0
    AT+CGPSINF=0
    +CGPSINF: 0,1947.718300,9952.733400,2546.200000,20200110005119.000,0,11,2.500200,278.420013

    OK

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
    if (strstr(respuesta, "+CGPSINF: 0") != nullptr) {

        String informacion(strstr(respuesta, "+CGPSINF: 0"));       //primero uso el que tiene las coordenadas en decimal
        //Serial.println(informacion);
        /*
            +CGPSINF: 0,1947.718300,9952.733400,2546.200000,20200110005119.000,0,11,2.500200,278.420013

            OK
        */

        /*
            creamos un sub String que tenga apartir del espacio hasta el salto de linea, le sumamos 1 para que no se incluya el espacio
            le restamos menos 1 en donde encontro el salto de linea para que no incluya ese salto de linea en el substring
        */
        informacion = informacion.substring(informacion.indexOf(' ') + 1, informacion.indexOf('\n') - 1);
        //Serial.println(informacion);
        
        //ahora dividiremos por las comas, hay una funcion que es strtok algo similar a split pero hay que convertir los String a charArray
        /*
        0,1947.718300,9952.733400,2546.200000,20200110005119.000,0,11,2.500200,278.420013
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

        informacion.remove(0, informacion.indexOf(',') + 1);                     //para quitar el modo 0 la coma tambien
        //Serial.println(informacion);                                      //1947.718300,9952.733400,2546.200000,20200110005119.000,0,11,2.500200,278.420013

        informacion.remove(0, informacion.indexOf(',') + 1);                        //quitamos la latitud
        //Serial.println(informacion);                                          //9952.733400,2546.200000,20200110005119.000,0,11,2.500200,278.420013

        informacion.remove(0, informacion.indexOf(',') + 1);                        //quitamos la longitud
        //Serial.println(informacion);                                          //2546.200000,20200110005119.000,0,11,2.500200,278.420013

        informacion.remove(0, informacion.indexOf(',') + 1);                        //quitamos la Altitud
        //Serial.println(informacion);                                          //20200110005119.000,0,11,2.500200,278.420013

        informacion.remove(0, informacion.indexOf(',') + 1);                        //quitamos la fechay hora
        //Serial.println(informacion);                                          //0,11,2.500200,278.420013

        informacion.remove(0, informacion.indexOf(',') + 1);                        //quitamos tiempo de respuesta
        //Serial.println(informacion);                                          //11,2.500200,278.420013
                                                                        
                                                                        
        satelites = informacion.substring(0, informacion.indexOf(','));
        //Serial.println(satelites);                                                                ////este entrega un numero diferente de satelites que el de arriva no se cual usar aqui entrego 10 arriba 4
        informacion.remove(0, informacion.indexOf(',') + 1);                        //quitamos numero de satelites
        //Serial.println(informacion);                                          //2.500200,278.420013

        veloci = informacion.substring(0, informacion.indexOf(','));
        //Serial.println(veloci);
        informacion.remove(0, veloci.length() + 1);                              //quitamos la velocidad
        //Serial.println(informacion);                                             //278.420013\nOk


        angulo = informacion.substring(0, informacion.indexOf('\n'));                                                     //en el string original ya solo quedo el angulo y el salto de linea
        //Serial.println(angulo);        
        //Serial.println(informacion);                                             //ya no impota si se queda el angulo dentro de ese string



        peticion2 = true;




    }












    
    //respuesta = enviarComandoSinEvaluar(AT_INFO_IMEI, _MENOR_A_128_BYTES, nullptr);
    //if (strstr(respuesta, "OK") != nullptr) {
    //
    //    /*
    //        AT+CGSN
    //        869170031697544
    //
    //        OK
    //    */
    //
    //    String informacion(respuesta);
    //
    //
    //    //Serial.println("info " + informacion);
    //
    //    informacion.remove(0, informacion.indexOf('\n') + 1); //quitamos hasta el salto de linea incluido el salto de linea
    //
    //    //Serial.println(informacion);
    //    /*
    //    869170031697544
    //
    //    OK
    //    */
    //
    //    /*
    //        creamos un sub String desde la posicion 0 hasta el salto de linea pero sin tomarlo en cuenta en el resultado ya no lo quiero
    //    */
    //    imei = informacion.substring(0, informacion.indexOf("\n") - 1); //quitamos el salto de linea tambien
    //    //Serial.println("imei " + imei);
    //    //imei 869170031697544
    //    peticion3 = true;
    //}
    



          

    respuesta = enviarComandoSinEvaluar(AT_GPS_STATUS, _MENOR_A_64_BYTES,nullptr);
    if (strstr(respuesta, "+CGPSSTATUS:") != nullptr) {

        /*
            AT+CGPSSTATUS?
            AT+CGPSSTATUS?
        +CGPSSTATUS: Location 3D Fix

        OK
        */

        String informacion(strstr(respuesta, "+CGPSSTATUS:"));       //primero uso el que tiene las coordenadas en decimal
        /*
        +CGPSSTATUS: Location 3D Fix

        OK
        */

       //Serial.println("info " + informacion);

       senalGps = informacion.substring(informacion.indexOf(' ') + 1, informacion.indexOf('\n') - 1); //quitamos hasta el salto de linea incluido el salto de linea

        //Serial.println(senalGps);
        /*
        Location 3D Fix
        */

       senalGps = senalGps.substring(senalGps.indexOf(" ")+1, senalGps.indexOf(" ") + 3);
       /*
       3D
       */
       //Serial.println(senalGps);

        





        peticion4 = true;




    }



    if (peticion1 && peticion2 && peticion4) {

        //hacemos la ultima peticio que es la de la hora si ambas peticiones no tuverion ningun errror ya podemos retornar los datos

        float velocidakmHr = veloci.toFloat(); //* 1.852;   //antes multiplicaba por 1.82 porque segun la documentacion la velocidad venia en nudos, pero con las pruebas nos tamos cuenta que la velocidad ya viene en km/hr
        //Serial.println(String(velocidakmHr));
        float latitud = lat.toFloat();
        float longitud = lon.toFloat(); //convertimos para que se eliminen los 2 ceros que aparecen enfrente





        //y despues los convertimos a string con una presicion de 6 decimales
        
        if (cadenaDeRetorno == _CADENA_RETORNO_FECHAHORA_LAT_LON_ALT_VEL_DIR_PRECI_SATELL_CALIDAD) {
            return
                //getDateGpsTimeZone_toStringFechaHora(ZONA_HORARIA_MEXICO_ESTANDAR_CST) + "," + String(latitud, 6) + "," + String(longitud, 6) + "," + altitud + "," + velocidakmHr + "," + angulo + "," + precision + "," + satelites + "," + senalGps;
                Utilerias::imprimirFechaHoraDigitos(getDateGpsTimeZone(zonaHoraria)) + "," + String(latitud, 6) + "," + String(longitud, 6) + "," + altitud + "," + velocidakmHr + "," + angulo + "," + precision + "," + satelites + "," + senalGps;


        }

        if (cadenaDeRetorno == _CADENA_RETORNO_LAT_LON_VEL_DIR) {
            return
                String(latitud, 6) + "," + String(longitud, 6) + "," + velocidakmHr + "," + angulo;

        }
       
    }


    return "Fallo getLoocalizacion peticiones (f=0 t=1) p1Co: " + String(peticion1) + "\t p2VR:" + String(peticion2) + "\t p4C:" + String(peticion4);
}
/*
Este metodo debera llamarce antes de leer las variables de localizacion rapida que estaran almacenadas en esta clase
esto nos permite por ejemplo cada 2 segundos llamar a este metodo desde el loop principal del programa 
este metodo actualizara las variables de la clase y luego accederemos a ellas por los metodos correspondientes

-Hciendo esto nos permite por ejemplo: LLAMAR A ESTE METODO QUE CONSULTARA LAS COORDENADAS ACTUALES CON EL MODULO SIM
ESTO CONLLEVA COMUNICACION POR MEDIO DEL PUERTO UART HACIA EL MODULO SIM QUE POR SUPUESTO CONSUME TIEMPO Y ES PROPENSO A ERRORES
ENTONCES AL HACERLO DE ESTA MANERA PUEDES LLAMAR A ACTUALIZAR EN OTRO LADO DEL ESKETCH CADA 2 SEGUNDOS POR EJEMPLO Y SI HAY ALGUN 
METODO QUE ESCRIBA EN LA SD Y USARAS LA POSICION, SOLO USAS LAS VARIABLES DE ACCESO RAPIDO QUE SE ACTUALIZARON POR MUY TARDE
HACE 2 SEGUNDOS CON LA UBICACION REAL, Y LAS USAS EN TU ARCHIVO SD. SERA MAS RAPIDO GUARDARLAS EN LA SD PORQUE YA NO TUVISTE QUE CONSULTAR
Y COMUNICARTE POR UART AL MODULO PORQUE YA SE HISO EN OTRA PARTE DEL PROGRAMA, TAMBIEN ES MENOS PROBABLE QUE OCURRA UN ERROR Y SE CUELGE ARDUINO CON EL ARCHIOVO SD ABIERTO, 
TAMBIEN SI HAY ALGUN ERROR EN LA CONSULTA DE COORDENADAS LAS COORDENADAS QUE IMPRIMIRAS EN LA SD TENDRAN LAS COORDENADAS DE EL ULTIMO
PUNTO ANTES DEL ERROR, DE LO CONTRARIO EN LA SD NO SE IMPRIMIRIAN LAS COORDENADAS SI HICIERAMOS TODO EL PROCESO DEL UART
SI POR EJEMPLO SE ESCRIBEN 2 ARCHIVOS O 3 SEGUIDOS DONDE SE USEN LAS COORDENADAS, COMO ACELEROMETRO FLUJOMETRO, NO TENDREMOS QUE 
PASAR POR EL PORCESO DE COMUNICARNOS POR UART AL MODULO SIM EN CADA 1 DE LAS PARTES DONDE SE USARAN, YA ALS TIENES ACTUALIZADAS
PODEMOS USARLAS CUANTAS VECES QUERAMOS SEGUIDAS ME EXPLICO?

EL METODO QUE HACE TODO COMPLETO SE LLAMA GETLOCALIZACION, ESE SE COMUNICA POR UART, EL METODO ACTUALIZAR QUE ES ESTE DE ABAJO
USA A GETLOCALIZACION.

*/
void ClaseManejoSim808::actualizarLatLongSpeedAccesoRapido()
{

    /*
    En este metodo usaremos el String que nos devuelve nuestro metodo principal que consulta las coordenadas
    y lo dividiremos para convertir los valores que queramos usar en el sketch principal pero ya convertidos en numeros
    que podremos manejar, lo hago asi para utilizar solo un metodo que se encarge de obtener las localizaciones que seran impresas en la sd
    y este metodo para sacar los valores y convertirlos a sus valores numericos
    */

    String respuesta = getLocalizacion(_CADENA_RETORNO_LAT_LON_VEL_DIR, 0);     //en este caso como no me interesa la hora envio que la hora sea la 0 o UTC porque solo usaremos las coordenadas y eso
    String lat;
    String lon;
    String speed;
    String rumbo;


    if (respuesta.indexOf('=') == -1) {
        //si no devolvio el mensaje Fallon en get........ que contiene el =
        //19.795300,-99.878799,0.36,8.51
        //latitud,  longitud,km/hr,grados
        //Serial.println(respuesta);



        lat = respuesta.substring(0, respuesta.indexOf(','));
        //Serial.println(lat);
        respuesta.remove(0, respuesta.indexOf(',') + 1);
        //Serial.println(respuesta);
        //-99.878799,0.36,8.51

        lon = respuesta.substring(0, respuesta.indexOf(','));
        //Serial.println(lon);
        respuesta.remove(0, respuesta.indexOf(',') + 1);
        //Serial.println(respuesta);
        //0.36,8.51
        


        speed = respuesta.substring(0, respuesta.indexOf(','));
        //Serial.println(speed);
        respuesta.remove(0, respuesta.indexOf(',') + 1);
        //Serial.println(respuesta);
        //8.51

  
        rumbo = respuesta.substring(0, respuesta.indexOf(','));
        //Serial.println(rumbo);
        //Serial.println(respuesta);
        

        this->latitud = lat.toFloat();
        this->longitud = lon.toFloat();
        this->velocidad = speed.toInt();
        this->direccion = rumbo.toInt();
        Serial.println(F("getLatLongSpeed coordenadas actualizadas exitosamente"));
    }
    else {
        Serial.println(F("Eror en las peticiones las coordenadas se quedan iguales getLatLongSpeed"));
    }
    



}
/*
Entrega la latitud que previamente se consulto con el metodo actualizarLatLongSpeedRapido
*/
float ClaseManejoSim808::getLatitudRapido()
{
    return this->latitud;
}
/*
Entrega la longuitud que previamente se consulto con el metodo actualizarLatLongSpeedRapido
*/
float ClaseManejoSim808::getLongitudRapido()
{
    return this->longitud;
}
/*
Entrega la velocidad que previamente se consulto con el metodo actualizarLatLongSpeedRapido
*/
int ClaseManejoSim808::getVelocidadRapido()
{
    return this->velocidad;
}
/*
Entrega la direccion o rotacion en grados que previamente se consulto con el metodo actualizarLatLongSpeedRapido
*/
int ClaseManejoSim808::getDireccionRapido()
{
    return this->direccion;
}






String ClaseManejoSim808::getIMEI()
{
    char* respuesta;
    respuesta = enviarComandoSinEvaluar(AT_INFO_IMEI, _MENOR_A_64_BYTES, "OK");
    if (strstr(respuesta, "OK") != nullptr) {

        /*
            AT+CGSN
            AT+CGSN
            869170031697544

            OK
        */

        respuesta = strstr(respuesta, "8");
        String informacion(respuesta);      



        //Serial.println(informacion);
        /*
        869170031697544

        OK
        */


        return informacion.substring(0, informacion.indexOf("\n") - 1); //quitamos el salto de linea tambien
        //Serial.println("imei " + imei);
        //imei 869170031697544







    }
}

bool ClaseManejoSim808::getStatusPinSim()
{
    char* respuesta = enviarComandoSinEvaluar(AT_INFO_PIN_STATUS, _MENOR_A_64_BYTES, "OK");
    if (strstr(respuesta, "OK") != nullptr) {

        /*
            AT+CPIN?
            AT+CPIN?
            +CPIN: READY

            OK
        */

        respuesta = strstr(respuesta, "+CPIN: ");
        String informacion(respuesta);
        //Serial.println(informacion);
        /*
        +CPIN: READY

         OK
        */


        informacion = informacion.substring(informacion.indexOf(' ') + 1, informacion.indexOf('\n')-1);
        //Serial.println(informacion);

        if (informacion.equals("READY")) {
            Serial.println(F("Pin del fin esta definido exitosamente deberia conectarse a una red sin problemas\n"));
            return true;
        }
        else {
            Serial.println(F("No hay pin de sim definido, quizas no pueda conectarse a una red\n"));
            return false;
        }

      





    }
    else {
        Serial.println(F("Se respondio error, puede que no pueda conectarce a una red\n"));
        return false;
    }
}

bool ClaseManejoSim808::setPinSim(String pin)
{
    //configuramos el APN
    String comando(AT_INFO_PIN_CONFIG);
    /*
    "AT+CPIN=\"XXXX\""
    */
    comando.replace("XXXX", String(pin));


    char* com;
    com = new char[comando.length()];
    comando.toCharArray(com, comando.length() + 1);
    Serial.println(com);
    /*
    AT+CPIN="0000"   
    */




    char* info = enviarComandoSinEvaluar(com, _MENOR_A_64_BYTES, nullptr);
    /*
    Si el apn se configuro bien
    AT+CSTT="internet.itelcel.com","webgprs","webgprs2003"
    AT+CSTT="internet.itelcel.com","webgprs","webgprs2003"
    OK\r\n
    */
    String auxiliar(info);
    //Serial.print(auxiliar);
    auxiliar = auxiliar.substring(auxiliar.length() - 4, auxiliar.length() - 2);
    //Serial.print(auxiliar);
}
/*
En ASU  Para redes 2G 
17-31       Excelente, mas cerca de la torre -79 -> -51 dbm
12.5 - 17   Buena                           -88 -> -79 dbm
9 - 12.5    Regular                         -95 -> -88 dbm
4.5 - 9     Mala                            -104 -> -95 dbm
0 - 4.5     Debil                           -113 -> -104 dbm
0           Sin señal

Usaremos este metodo para conocer la fuerza de señal en formato ASU
y si la señal esta por debajo del margen "Malo" vamos a cancelar el envio de puntos gps y que se guarden en la memoria SD
https://norfipc.com/redes/intensidad-nivel-senal-redes-moviles-2g-3g-4g.php
*/
byte ClaseManejoSim808::getFuerzaSenal()
{
    char* respuesta = enviarComandoSinEvaluar(AT_INFO_RED_CALIDAD_DE_RED, _MENOR_A_32_BYTES, "OK");
    if (strstr(respuesta, "OK") != nullptr) {

        /*
            AT+CSQ
            AT+CSQ
            +CSQ: 16,0

            OK
        */

        respuesta = strstr(respuesta, "+CSQ: ");
        String informacion(respuesta);
        //Serial.println("info " + informacion);
        /*
        +CSQ: 16,0

         OK
        */

        
        informacion = informacion.substring(informacion.indexOf(' ')+1, informacion.indexOf(','));
        //Serial.println("Calidad:" + informacion);

        return (byte) informacion.toInt();





    }
}











//Estos metodos son para el modulo sim de arduino el primero que compre que incluye el cargador de bateria
//ahora compre modulos mas economicos que no lo incluyen entonces usare una bateria externa con su propio gestor de bateria
//o BMS pero dejo estos metodos porque los hice y funcionan bien aunque ya no se usaran

/*
True    -> Gestor esta activado
False   -> Gestor esta desactivado
El modulo Sim tiene incluido un gestor de carga de bateria, pero este gestor de carga esta desactivado por default
es decir cuando el modulo se reinicie este gestor estara apagado, a menos que se guarde en las configuraciones ATW del modulo
pero para pronto comprobaremos si el gestor esta activado o desactivado
*/
bool ClaseManejoSim808::getGestorBateriaActivado()
{
    char* respuesta = enviarComandoSinEvaluar(AT_CONFIG_BATERIA_ESTADO_CARGA___ATW, _MENOR_A_32_BYTES, "OK");
    if (strstr(respuesta, "OK") != nullptr) {

        /*
           27bytes
            AT+ECHARGE?
            AT+ECHARGE?
            +ECHARGE: 1

            OK
        */

        respuesta = strstr(respuesta, "+ECHARGE:");

        String informacion(respuesta);


        //Serial.println(informacion);


        //Serial.println(informacion);
        /*
        +ECHARGE: 1

        OK
        */


        informacion = informacion.substring(informacion.indexOf(' ') + 1, informacion.indexOf('\n')-1);
        //Serial.println("Estado:" + informacion);

        if (informacion.equals("1")) {
            return true;
        }
        else {
            return false;
        }
    }
    else {
        //si se recibe error 
        return false;
    }

}
/*
@param tipo
        True activar gestor
        False desactivar Gestor
@return
        True si se activo o desactivo correctamente el gestor
        False si hubo error al enviar el comando o el modulo sim no respondio
Activaremos el gestor de bateria si quieres que el modulo sim se encarge de cargar la bateria de litio
esto hay que hacerlo cada que se inicialice el modulo sim si no se tiene guardado en las configuraciones 
ATW
*/
bool ClaseManejoSim808::setActivarDesactivarGestorBateria(bool tipo)
{


    char* respuesta;

    if (tipo) {

        respuesta = enviarComandoSinEvaluar(AT_CONFIG_BATERIA_HABILITAR_CARGA___ATW, _MENOR_A_32_BYTES, "OK");
        if (strstr(respuesta, "OK") != nullptr) {

            /*
               29Bytes
                AT+ECHARGE=1
                AT+ECHARGE=1
                OK
            */    
            Serial.println(F("Gestor de carga activado"));
            return true;
        }
        else {
            Serial.println(F("Error al enviar el comando o respuesta recibida"));
            return false;
        }



    }
    else {
        respuesta = enviarComandoSinEvaluar(AT_CONFIG_BATERIA_DESABILITAR_CARGA___ATW, _MENOR_A_32_BYTES, "OK");
        if (strstr(respuesta, "OK") != nullptr) {
            /*
               17Bytes
                AT+ECHARGE=0
                OK
            */
            Serial.println(F("Gestor de carga desactivado"));
            return true;

        }
        else {
            Serial.println(F("Error al enviar el comando o respuesta recibida"));
            return false;
        }

    }

}
/*
@return
True -> se consultaron los datos con exito
False -> Ocurrio un error al consultar los datos
Actualizamos las variables privadas de nuestra clase donde contiene la informacion de la bateria
este metodo se tendra que llamar cada que queramos actualizar los valores de las variables
con nuestro valor real, meterlo en un bucle de cada cierto tiempo por ejemplo cada 5 segundos
leeremos el voltaje de la bateria y actualizaremos nuestras variables privadas
*/
bool ClaseManejoSim808::actualizarInformacionBateria()
{
    String porcentaje;
    String voltaje;


    char* respuesta = enviarComandoSinEvaluar(AT_INFO_BATERIA, _MENOR_A_32_BYTES, "OK");
    if (strstr(respuesta, "OK") != nullptr) {

        /*
           33Bytes
            AT+CBC
            AT+CBC
            +CBC: 0,93,4147

            OK
            -----------------
            el primer 0 no se que sea, solo parece siempre asi, quizas es la bateria 1 o si tuviera para dos baterias, crei que indicaba si se estaba cargando o descargando pero tampoco
        */

        respuesta = strstr(respuesta, "+CBC:");
        String informacion(respuesta);
        //Serial.println("info " + informacion);
        /*
        +CBC: 0,93,4147

         OK
        */


        

        informacion.remove(0, informacion.indexOf(',') + 1);
        //Serial.println(informacion);
        /*
        93,4147

        OK
        */

        porcentaje = informacion.substring(0, informacion.indexOf(','));
        Serial.println("Porcentaje bateria:" + porcentaje);

        informacion.remove(0, informacion.indexOf(',') + 1);
        //Serial.println(informacion);
        /*
        4147

        OK
        */



        voltaje = informacion.substring(0, informacion.indexOf('\n') - 1);
        Serial.println("Voltaje bateria:" + voltaje);



        this->porcentajeBateria = (byte)porcentaje.toInt();
        this->voltajeBateria = voltaje.toInt();

        Serial.println(F("Informacion de bateria consultada y actualizada exitosamente"));

        

        return true;

    }
    else {
        //si se recibe error 
        Serial.println(F("Error al consultar la informacion de la bateria las variables temporales se quedan iguales"));

        return false;
    }
   
}
/*
Entrega el porcentaje de la bateria que previamente tuvo que ser actualizado con actualizarInformacionBateria
*/
byte ClaseManejoSim808::getPorcentajeBateria()
{
    return this->porcentajeBateria;
}
/*
Entrega el voltaje de la bateria que previamente tuvo que ser actualizado con actualizarInformacionBateria
*/
int ClaseManejoSim808::getVoltajeBateria()
{
    return this->voltajeBateria;
}






time_t ClaseManejoSim808::fechaHoraGpsUTC()
{
    char* respuesta = enviarComandoSinEvaluar(AT_GPS_COORDENADAS_128_GPZDA_HORA_FECHA, _MENOR_A_64_BYTES, nullptr);
    //Serial.println(respuesta);
    /*
    IMPORTANTE SI RECIEN SE INICIA EL MODULO Y SE ENVIA EL COMANDO RETORNARA COMO RESPUESTA ERROR
    -SI EL MODULO GPS ESTA APAGADO ES DECIR LOCATION UNKNOW EL MODULO RETORNA LA FECHA PERO DE +CGPSINF: 128,235955.000,05,01,1980,00,00
    -EN CUANTO EL MODULO SE ENCIENDE, NO IMPORTA SI NO TIENE SEÑAL GPS AL PARECER YA RETORNA LA FECHA CORRECTA
    AT+CGPSINF=128
    AT+CGPSINF=128
	+CGPSINF: 128,023648.000,10,01,2020,00,00

	OK
    */
    //protejemos por si se envia antes que el gps este activo y retorna error entonces se moriria arduino en la parte donde convierte la hora a int
    if (strstr(respuesta, "+CGPSINF:") != nullptr) {
        //Serial.println("Respuesta encontrada ");

        String informacion(strstr(respuesta, "+CGPSINF:"));
        //Serial.println(informacion);


        /*
        +CGPSINF: 128,023648.000,10,01,2020,00,00

        OK
        */

        /*
            creamos un sub String que tenga apartir del espacio hasta el salto de linea, le sumamos 1 para que no se incluya el espacio
            le restamos menos 1 en donde encontro el salto de linea para que no incluya ese salto de linea en el substring
        */
        informacion = informacion.substring(informacion.indexOf(' ') + 1, informacion.indexOf('\n') - 1);
        //Serial.println(informacion);
        /*
        128,023648.000,10,01,2020,00,00
        modo 128
        hora UTC 023648.000
        dia UTC 10
        mes UTC 01
        Anio 2020
        */
        //ahora dividiremos por las comas, hay una funcion que es strtok algo similar a split pero hay que convertir los String a charArray

        String modo = informacion.substring(0, informacion.indexOf(','));
        //Serial.println(modo);                                   //128
        informacion.remove(0, modo.length() + 1);                     //para quitar la coma tambien
        //Serial.println(listo);                                  //023648.000,10,01,2020,00,00

        String date = informacion.substring(0, informacion.indexOf(','));
        //Serial.println(date);                                   //023648.000
        informacion.remove(0, date.length() + 1);
        //Serial.println(informacion);                                  //10,01,2020,00,00

        String dia = informacion.substring(0, informacion.indexOf(','));
        //Serial.println(dia);                                   //10
        informacion.remove(0, dia.length() + 1);
        //Serial.println(informacion);                                  //01,2020,00,00

        String mes = informacion.substring(0, informacion.indexOf(','));
        //Serial.println(mes);                                   //01
        informacion.remove(0, mes.length() + 1);
        //Serial.println(informacion);                                  //2020,00,00

        String anio = informacion.substring(0, informacion.indexOf(','));
        //Serial.println(anio);                                   //2020
        informacion.remove(0, anio.length() + 1);
        //Serial.println(informacion);                                  //00,00 esto lo desechamos no se que sera


        //dividimos la hora en 2, el gps siempre enviara de 222 la hora
        String hora = date.substring(0, 2);
        //Serial.println(hora);                           //02
        String minuto = date.substring(2, 4);
        //Serial.println(minuto);                         //36
        String segundo = date.substring(4, 6);
        //Serial.println(segundo);                        //48


        tmElements_t Fecha;
        Fecha.Second = segundo.toInt();
        Fecha.Minute = minuto.toInt();
        Fecha.Hour = hora.toInt();
        Fecha.Day = dia.toInt();
        Fecha.Month = mes.toInt();
        Fecha.Year = anio.toInt() - 1970;

        time_t tiempo = makeTime(Fecha);

        


        return tiempo;
    }
    else {
        Serial.println("Respuesta no encontrada ");
        return 0;  //debido a que la variable tiempo no es mas que un unsigned Long, retornamos un 0
    }
}

time_t ClaseManejoSim808::getDateGpsTimeZone(int zonaHoraria)
{
    time_t timeUTC = fechaHoraGpsUTC();
    
    time_t horaZonaHoraria;
    
    
    long tiempoAjusteZonaHoraria = zonaHoraria * 3600; //si en la zona horaria es -6 e. resultado sera -21600, si es +3 el resultado sera positivo y se añadira
    //para restarle el tiempo segun la libreria time 1 hora equivale a 3600s como queremos obtrener un timezone-6
    //es decir menos 6 horas -6*3600 = -21600
    //de esa forma en nuestro puerto serie tendremos
    //10/1/2020 4:19:20             //esta seria la hora y fecha utc dada por el gps
    //9 / 1 / 2020 22:19 : 20       //y esta la hora y fecha actual de la zona -6
    horaZonaHoraria = timeUTC + tiempoAjusteZonaHoraria;


    

    /*
    Serial.print(day(timeUTC));
    Serial.print(+"/");
    Serial.print(month(timeUTC));
    Serial.print(+"/");
    Serial.print(year(timeUTC));
    Serial.print(" ");
    Serial.print(hour(timeUTC));
    Serial.print(+":");
    Serial.print(minute(timeUTC));
    Serial.print(":");
    Serial.println(second(timeUTC));
    delay(1000);//Esperamos 1 segundo


    Serial.print(day(horaZonaHoraria));
    Serial.print(+"/");
    Serial.print(month(horaZonaHoraria));
    Serial.print(+"/");
    Serial.print(year(horaZonaHoraria));
    Serial.print(" ");
    Serial.print(hour(horaZonaHoraria));
    Serial.print(+":");
    Serial.print(minute(horaZonaHoraria));
    Serial.print(":");
    Serial.println(second(horaZonaHoraria));
    delay(1000);//Esperamos 1 segundo


 */

    return horaZonaHoraria;
}

String ClaseManejoSim808::getDateGpsTimeZone_toStringFechaHora(int zonaHoraria)
{
    //dd/MM/aaaa hh/mm/ss
    time_t tiempo = getDateGpsTimeZone(zonaHoraria);

    return String(day(tiempo)) + "/" + String(month(tiempo)) + "/" + String(year(tiempo)) + "," + String(hour(tiempo)) + ":" + String(minute(tiempo)) + ":" + String(second(tiempo));
     
}

String ClaseManejoSim808::getDateGpsTimeZone_toStringFecha(int zonaHoraria)
{
    time_t tiempo = getDateGpsTimeZone(zonaHoraria);
    //dd/MM/aaaa
    return String(day(tiempo)) + "/" + String(month(tiempo)) + "/" + String(year(tiempo));
}

String ClaseManejoSim808::getDateGpsTimeZone_toStringHora(int zonaHoraria)
{
    time_t tiempo = getDateGpsTimeZone(zonaHoraria);
    //hh:mm:ss
    return String(hour(tiempo)) + ":" + String(minute(tiempo)) + ":" + String(second(tiempo));
}




bool ClaseManejoSim808::enviarSMS(String zonaMundial, String numeroTelefono, String contenido)
{

    char* info;
    info = enviarComandoSinEvaluar(AT_SMS_CONFIG_CAMBIAR_MODO_TEXTO___ATW, _MENOR_A_128_BYTES, nullptr);

    if (strstr(info, "ERROR") != nullptr) return false;
    
    

    /*
    Sustituimos en el comando enviar sms las YY y XX por los numeros
    */
    String comando(AT_SMS_ENVIAR_SMS);
    comando.replace("YY",zonaMundial);
    comando.replace("XXXXXXXXXX",numeroTelefono);
    char* com;                                          //creamos el array en tiempo de ejecucion para no pelear con el tamaño del comando
    com = new char[comando.length()];
    comando.toCharArray(com, comando.length()+1);
    //Serial.println(com);    //AT+CMGS="+527121157686"

    info = enviarComandoSinEvaluar(com, _MENOR_A_128_BYTES, nullptr);
    delete[] com;           //liberamos la memoria del puntero
    //Serial.println(info);
    /*
    AT+CMGS="+527121157686"
     >                          //al devolvernos esto indica que pide el cuerpo del mensaje
    */
    
        if (strstr(info, ">") != nullptr) {

            //enviamos el cuerpo del mensaje
            Serial3.print(contenido);


            info = enviarComandoSinEvaluar(AT_SMS_FINALIZAR_ENVIO_SMS_1, 5000, "OK");
            
            //DESPUES DE ENVIAR EL COMANDO \u001A si el gps logro enviar el mensaje respondera con: para eso tarda como 3 segundos en enviar la respuesta, por eso detengo el procesador 5 segundos 
            /*

            Hola luisda 900033


            +CMGS: 23							//el gps responde que lo envio y en que posicion de la memoria se guardo el mensaje enviado

	        OK
            */

    
            //Serial.println(info);
    
            if (strstr(info,"OK") != nullptr) {

                //Serial.println("Mensaje enviado con exito");
                return true;
            }
            

        }

        

    
    return false;
}


















/*
    Pregunta por el estado del GPRS
    retorna:
    True o 1 si esta activado
    False o 0 si esta apagado
    2 si hay algun error
*/
byte ClaseManejoSim808::gprsComprobarEstado()
{
    


    char* info = enviarComandoSinEvaluar(AT_GPRS_INFO_ESTADO_ACTUAL, _MENOR_A_32_BYTES, "OK");
    /*
        AT+CGATT?
        AT+CGATT?
        +CGATT: 0

        OK  
    */

    if (strstr(info, "+CGATT:")!= nullptr) {
        info = strstr(info, "+CGATT:");
        /*
        +CGATT: 0

        OK  
        */
        //Serial.println(info);
        String dato(info);
        
        dato = dato.substring(dato.indexOf(' ') + 1, dato.indexOf('\n') - 1);
        //Serial.println("Dato " + String(dato));

        if (dato.equals("1")) {
            //Serial.println("GPRS encendido");
            return true;
        }
        else {
            //Serial.println("GPRS Apagado");
            return false;
        }

    }
    else {
        //si se retorna cualquier cosa menos la respuesta correcta retornamos 2 indicando que esta apagado
        return 2;
    }
    
    
    
}
/*
    Activa o desactiva el GPRS
    envia True para activarlo, False para apagarlo
    Retorna True si la accion se realizo con exito
    False si ocurrio un error, o el modulo esta apagado
    */
bool ClaseManejoSim808::gprsActivar_desactivar(bool estado)
{
    char* info;
    if (estado) {
        info = enviarComandoSinEvaluar(AT_GPRS_ACTIVAR, _MENOR_A_32_BYTES,"OK");
    }
    else {
        info = enviarComandoSinEvaluar(AT_GPRS_DESACTIVAR, _MENOR_A_32_BYTES, "OK");
    }

    
    
    if (strstr(info, "OK") != nullptr) {       
        return true;
    }
   

    return false;
}
/*
Pregunta el estado de la conexion gprs esto nos permite saber si:
Return:
0 o false ->IP INITIAL Si no se ha configurado ningun APN, no hay ningon modo gprs activado aun
1 o true ->IP START Si ya se definio el APN por lo tanto la configuracion esta a la mitad, en este momento no se puede cambiar el APN a menos que se cierre con AT+CIPSHUT
2        ->IP GPRSACT Si se conecto correctamente a la red GPRS el led esta parpadeando rapidamente y ya se puede establecer una conexion con un servidor
3        -> IP STATUS significa que esta parpadeando rapidamente el led y que ya emos obtenido la direccion ip con AT+CIFSR 
4        -> COONECT OK significa que esta parpadeando rapidamente el led y que hay una conexion establecida con un servidor!
5        -> TCP CONNECTING el servidor no ha retornado la respuesta para CONNECT OK, es decir esta en proceso, aqui no se pueden enviar datos aun
6        -> TCP CLOSE significa que esta parpadeando rapidamente el led y pero que se cerro la conexion que se tenia con el servidor anterior
7        ->ERROR Si hay cualquier error al preguntar el status
*/
byte ClaseManejoSim808::gprsStatusDelContextoCIP()

{
    Serial.println(F("\n-Metodo StatusDelContextoCip-\n"));


    char* info = enviarComandoSinEvaluar(AT_GPRS_INFO_STATUS_DEL_CONTEXTO, _MENOR_A_64_BYTES, nullptr);//NO HYA FORMA DE BUSCAR PALABRA CLAVE PARA SALIR ANTES YA QUE EL OK ESTA INTERMEDIO Y EL FINAL DE LA RESPUESTA ES VARIABLE
    /*
    OKAY PRIMERO PREGUNTAREMOS EL ESTADO DEL CIP

    AT+CIPSTATUS
    OK

    STATE: IP START


    AT+CIPSTATUS
    OK

    STATE: IP INITIAL

    AT+CIPSTATUS
    OK

    STATE: IP GPRSACT                  //esto lo responde cuando ya se envio el comando AT+CIIR y el led esta parpadeando rapido

    */
    if (strstr(info, "STATE:") != nullptr) {

        String statusCip(strstr(info, "STATE:"));
        //Serial.println(statusCip);
        /*
        STATE: IP INITIAL
        */
        statusCip = statusCip.substring(statusCip.indexOf(' ') + 1, statusCip.indexOf('\n') - 1);
        //Serial.println(statusCip);
        /*
        IP INITIAL
        */






        if (statusCip.equals("IP INITIAL")) {
            //si es igual significa que no se ha iniciado un contexto
            //y se tiene que configurar APN antes

            return 0;



        }
        else if (statusCip.equals("IP START")) {
            //si es IP START significa que ya se definio un APN
            //pero que no se a enviado el comando AT+CIICR que hace que el modulo entre en modo gprs y parpadee rapidamente

            return 1;

        }
        else if (statusCip.equals("IP GPRSACT")) {

            //Significa que el modulo ya entro en gprs mode
            //el led esta parpadeando rapidamente
            //ahora se puede crear una conexion nueva
            return 2;
        }
        else if (statusCip.equals("IP STATUS")) {

            //Significa que el modulo ya entro en gprs mode
            //el led esta parpadeando rapidamente
            //y que ya hemos solicitado y obtenido la ip con AT+CIFSR
            return 3;
        }
        else if (statusCip.equals("CONNECT OK")) {

            //Significa que el modulo ya entro en gprs mode
            //el led esta parpadeando rapidamente
            //y que hay una coneccion activa con un servidor
            return 4;
        }
        else if (statusCip.equals("TCP CONNECTING")) {

            //Significa que el modulo ya entro en gprs mode
            //el led esta parpadeando rapidamente
            //Se enviop la peticion para conectarce con el servidor pero el servidor no ha respondido con CONNECT OK
            //no hay nignuna conexion establecida
            return 5;
        }

   
        else if (statusCip.equals("TCP CLOSED")) {

            //Significa que el modulo sigue en modo gprs
            //el led esta parpadeando rapidamente
            //pero que se ha cerrado una conexion con un servidor
            return 6;
        }
        else {
            //por si nos llega alguna nueva palabra que no tengamos reconocida por ejemplo un momento nos dejaron de enviar datos
            //y al preguintar por el error era //STATE: PDP DEACT ?? what tha fuukkkk what is this??
            //y entonces al reiniciar el modulo esto se corrigio
            return 7;

        }


        

    }
    else {

        //SI responde algo como error, que solo lo hace cuando el modulo no esta compeltamente iniciado de un reset
        //o algun otro errro desconociudo
        return 7;
    }

    //por si no hay ninguna otra palabra de arriba que co
    return 7;
}






bool ClaseManejoSim808::gprsAbrirContextoGPRS(String apnNombreRed, String apnUsuario, String apnPassword)
{

    /*
    No entendia bien como funcionaba la definicion del apn, yo crei que al enviar el comando AT+CSTT que es el que define el APN
    en ese momento el modulo lo guardaba en su memoria y podias usarlo cuantas veces quicieras sin volverlo a definir
    asi por ejemplo cuando preguntaba que APN tenia el modulo este respondia con el apn que defini, pero en ese momento
    haces que el modulo entre en un estado de definicion de gprs, despues tienes que enviar AT+CIICR el modulo comienza a parpadear rapidamente

    Paso 1 para crear conexion
    1 activar el gprs
    2 al preguntar por el estatus del cip AT+CIPSTATUS responde  STATE: IP INITIAL, es decir que se debe configurar el APN
    3 enviar el APN         Este se tiene que enviar por fuerza el modulo no lo guarda en ningun lugar, cada que establescas una conexion se tiene que enviar
    4 al preguntar el estatus del CIP responde STATE: IP START Significa que ya se definio el apn y se entro aun contexto de gprs si quieres cambiar el apn aqui ya no podras, tendras que enviar AT+CIPSHUT donde se vuelve a definir STATE: IP INITIAL y ahora si puedes cambiar el APN y volvemos a empezar
    5 Entrar en modo gprs con el comando AT+CIICR en este momento el led comienza a parpadear rapido, y significa que ya estamos en modo gprs oficialmente y podemos iniciar una conexcion
    6 para asegurarnos que tenemos conexion enviamos el comando AT+CIFSR que nos devuelve la IP que le asigno la red
    7 si preguntamos por el estado del CIP nos respoinde STATE: IP GPRSACT  quiere decir que estamos en modo gprs y podemos iniciar una conexio a un servidor
    8 si se quiere cambiar el apn en cualquiera de este estado hay que enviar AT+CIPSHUT y volver a comenzar todo
    9 Iniciamos una conexion a un servidor con el comando AT+CIPSTART definiendo el timpo de conexion si es TCP o UDP debe responer connect ok
    10 si p´reguntamos por el estado del cip debe marcar STATE: CONNECT OK
    10 Enviamos infomacion al servidor
    11 Si el servidor no nos cierra la conexion o queremos conectarnos a otro sin salir del modo GPRS enviamos AT+CIPCLOSE que nos cierra la conexion con ese servidor y podemos iniciar desde AT+CIPSTART
    11 si preguntas por el estado del cip sera STATE: TCP CLOSED
    12 Mientras nos quedemos en modo gprs podremos iniciar una nueva conexion apartir del paso 9
    13 si queremos salir de todo el contexto gprs enviamos AT+CIPSHUT el led deja de parpadear rapidamente, y tendriamos que volver a iniciar desde el paso 3 el envio del APN

    Entonces lo que haremos es que cuando se configure el APN por medio de un mensaje ese apn ARDUINO lo guardara en su EEPROM (ya qeu al modulo le vale m**)
    y cada que se inicie un contexto gprs arduino leera el apn de su EEPROM y se lo mandara al modulo
    */

    char* info;

    //para llamar a este metodo asumo que ya previamente se activo el gprs

    //cerramos cualquier contexto gprs auqnue no tengamos jaja de esta manera nos aseguramos que el estado sea IP INITIAL
    info = enviarComandoSinEvaluar(AT_GPRS_CIERRA_CONTEXTO, 1000, nullptr);
    /*
    AT+CIPSHUT
    SHUT OK
    EL SUT OK LLEGA DESPUES DE UN MOMENTO
    */

    if (strstr(info, "SHUT OK") != nullptr) {

        Serial.println("Contexto cerrado con SHUT OK exitosamente");
        //configuramos el APN
        String comando(AT_GPRS_APN_DEFINIR_APN);
        /*
        "AT+CSTT=\"name\",\"usuario\",\"password\""
        */
        comando.replace("name", apnNombreRed);
        comando.replace("usuario", apnUsuario);
        comando.replace("password", apnPassword);

        char* com;
        com = new char[comando.length()];
        comando.toCharArray(com, comando.length() + 1);
        //Serial.println(com);
        /*
        AT+CSTT="internet.itelcel.com","webgprs","webgprs2003"
        */




        info = enviarComandoSinEvaluar(com, _MENOR_A_128_BYTES, nullptr);
        delete[] com;                       //liberamos la memoria del puntero
        /*
        Si el apn se configuro bien 
        AT+CSTT="internet.itelcel.com","webgprs","webgprs2003"
        AT+CSTT="internet.itelcel.com","webgprs","webgprs2003"
        OK\r\n
        */
        String auxiliar(info);
        //Serial.print(auxiliar);
        auxiliar = auxiliar.substring(auxiliar.length() - 4, auxiliar.length()-2);
        //Serial.print(auxiliar);
        /*
        OK
        */        

        /*
        No uso la funcion strstr porque ella no es buena para buscar palabras concretas por ejemplo podria vernir la O y la K en otra plabra pero juntas
        o en el nombre de dominio y asi la daria como valida al final siempre nos manda un \r\n, retorno de carro y salto de linea los incluimos en el equals para que funcione bien
        */
        if (auxiliar.equals("OK")) {
            //si se definio correctamente el APN
            Serial.println("APN definido exitosamente");

            //iniciamos el contexto GPRS haciendo parpadera el led Rapidamente, NOTE QUE AQUI DEPRONTO TARDA UN POCO MAS EN ESTABLECERLO lo dejo 2 segundos esperando respuesta
            info = enviarComandoSinEvaluar(AT_GPRS_APN_CONECTAR_CON_APN, 2000, "OK");//s se encuentra antes el ok no llegaran los saltos de linea a tener en cuenta para limpiear la respuesta
            /*
            AT+CIICR
            AT+CIICR
            OK\r\n                          <- ALGUNAS VECES llegaran los saltos de linea si se cumplen los 2 segundos, pero si se ecnuetnra antes el OK recordar que se deja de recibir la informacion y no llegaran ni el retorno de carro ni el salto de linea
            */

            
            if (strstr(info,"OK")!= nullptr) {
                Serial.println("Se entro en contexto gprs exitosamente, obteniendo direccion ip...");
                info = enviarComandoSinEvaluar(AT_GPRS_APN_IP_OBTENER, _MENOR_A_128_BYTES, nullptr);
                /*
                AT+CIFSR
                AT+CIFSR
                10.138.184.79
                */

                if (strstr(info, "ERROR") == nullptr) {
                    //si no se encuentra la palabra error entonces quiere decir que se obtubo bien la ip
                    auxiliar = String(info); 
                    auxiliar.replace("AT+CIFSR\r\n", "");
                    /*
                    10.138.184.79
                    */
                   
                    auxiliar.replace("\r\n", "");
                    Serial.println("direccion ip obtenida correctamente: " + auxiliar);



                }
                else {
                    //si ha habido un error al conceguir la ip
                    Serial.println("Error al obeter una direccion ip");
                }


                
            }
            else {
                Serial.println("Error al entrar en contexto gprs, al enviar comando AT+CIICR");
            }
           




        }
        else {
            Serial.println("Error al definir el APN quizas debes cerrar el contexto con AT+SHUT");
        }
        


    }
 else {
       Serial.println("Error al cerrar el contexto AT+SHUT");
    }











    return false;
}
/*
Metodo de conexion HTTP puede ser TCP o UDP
ip_dominio puede ser www.kaliopeadmin.com.mx  o  162.241.93.228  
PUERTO  de conexion con el servidor por lo regular todos son puerto 80, a menos que en el servidor se abra otro puerto especifico para sierta aplicacion

RETURN
True si se establecio correctamente la conexion con el servidor COONECT OK
True si al volver a llamar a este metodo indica ALREADY CONNECT
False       Si no se pudo establecer coneccion CONNECT FAIL
False si en 5segundos de espera el modulo no respondio CONNECT OK
False       Si ocurre un error con el envio del comando y o se recibe una respuesta no esperada responde ERROR



*/
bool ClaseManejoSim808::gprsAbrirConexionConServidor(String TCP_UDP, String ip_DominioServidorPrincipal, String puerto, byte& gestorDeErrores)
{

    Serial.println(F("\n-METODO gprsAbrirConexionConServidor-\n"));
    //para este metodo asumimos que ya estamos en contexto gprs solo enviaremos la peticio para conectarnos a un servidor
    String comando(AT_GPRS_INFO_INICIAR_CONEXION);
    /*
    AT+CIPSTART=\"PROTOCOLO\",\"IP-DOMINIO\",\"PORT\"
    */
    comando.replace("PROTOCOLO", TCP_UDP);
    comando.replace("IP-DOMINIO", ip_DominioServidorPrincipal);
    comando.replace("PORT", puerto);

    char* com;
    com = new char[comando.length()];
    comando.toCharArray(com, comando.length() + 1);
    //Serial.println(com);
    /*
    AT+CIPSTART="TCP","162.241.93.228","80"
    */
    /*
    //esperamos 4 segundos porque responde inmediatamente OK pero despues de un ratito responde CONNECT OK significa que ya se conecto al servidor exitosamente
    Tenemos dos opciones si no queremos esperar los 4 segundos aqui no obtendremos la respuesta que queremos de connect ok
    y saldremos del metodo, cuando el programa principal revice la conexion al servidor, entonces volvera a entrar aqui
    donde volvera a enviar la solicitud de conexion al hacerlo si el modulo ya se habia conectado con exito respondera 
    AT+CIPSTART="TCP","162.241.93.228","80"
    ERROR

    ALREADY CONNECT
    Atencion aqui ya que tenemos configurado al modulo por default como una sola coneccion a diferente servidor cada vez, el modulo admite 3 conecciones diferentes, deje el link en el de los comandos
    de la documentacion oficial del sim808
    si la unica conexion que estamos usando esta ya conectada con servidor y queremos colocar otra direccion ip o dominio diferente respondera ALREADY CONNECT
    significa entonces que la conexion esta ocupada ya con otro servidor y que para cambiar de serviodor hay que enviar primero CIPCLOSE



    entonces significa que ya estaba conectado exitosamente y saltariamos todo el codigo, la desventaja es que hay que esperar hasta que el programa pricnipal vuelva a preguntar
    por el estado de la conexion al servidor
    la desventaja de la otra es que hay que esperar forsozamente 4 segundos y corre el riesgo de que quizas por algo tarde mas en conectarse y no obtengamos la respuesta que queremos aun asi
    e probado y tarda como 1 segundo o menos pero no se si cuando no tenga señal gprs pueda tardar mas pufff
    si hay alguna falla con el servidor y no nos conectamos responde
    CONNECT FAIL

    podriamos buscar la respuesta ALREADY COONNECT asi ya sabriamos que el modulo anteriormente se conecto correctamente al servidor
    o las 2 opciones esperamos no se 2 segundos y si no hay respuesta salimos del metodo, al volver a tratar de conectarce en otro ciclo loop
    si esta con alreadyCONNECT entonces retornamos true y que ya se puede enviar informacion

    */
    char* info = enviarComandoSinEvaluar(com, 5000, "CONNECT OK");    
    delete[] com;
    Serial.println(info);
    /*
    3 o opciones de respuesta

    AT+CIPSTART="TCP","162.241.93.228","80"
    OK

    CONNECT OK


    AT+CIPSTART="TCP","162.241.93.2289","80"
    OK

    STATE: IP STATUS

    CONNECT FAIL


    AT+CIPSTART="TCP","162.241.93.228","80"
    ERROR

    ALREADY CONNECT


    */
    
    if (strstr(info, "CONNECT OK") != nullptr) {
        //si la respuesta contiene 
        Serial.println("Nos conectamos exitosamente al servidor " + ip_DominioServidorPrincipal + " puerto " + puerto + " por protocolo: " + TCP_UDP);

        Serial.println(F("\n-Fin METODO gprsAbrirConexionConServidor-\n"));
        return true;
    }else if (strstr(info, "ALREADY CONNECT") != nullptr) {
        //si la respuesta contiene 
        Serial.println(F("Ya hay una conexion abierta con un servidor, cierrela antes de iniciar una nueva"));

        Serial.println(F("\n-Fin METODO gprsAbrirConexionConServidor-\n"));
        return true;
    }
    else if (strstr(info, "CONNECT FAIL") != nullptr) {
        //si la respuesta contiene 
        Serial.println("Fracaso la conexion con el servidor: " + ip_DominioServidorPrincipal + " puerto " + puerto + " por protocolo: " + TCP_UDP);

        Serial.println(F("\n-Fin METODO gprsAbrirConexionConServidor-\n"));
        return false;
    }
    if (strstr(info, "ERROR") != nullptr) {
        //si la respuesta contiene 
        Serial.println(F("Hay un error al intentar enviar el comando AT+CIPSTART"));
        gestorDeErrores++;

        Serial.println(F("\n-Fin METODO gprsAbrirConexionConServidor-\n"));
        return false;
    }


    

    Serial.println(F("No alcanzo a llegar la respuesta del modulo CONNECT OK o cualquier otra en los 5 segundos y se obtubo una respuesta vacia"));

    Serial.println(F("\n-Fin METODO gprsAbrirConexionConServidor-\n"));
    return false;       //por cualquier cosa que no responda niguna de las anteriores POR EJEMPLO SI NO ALCANZA A RESPONDER CONNECT OK EN 5 SEGUNDOS RETORNARIA ESTE FALSE
    
    


}







/*
Con este metodo enviamos informacion por el protocolo HTTP y POST, creamos la estructura necesaria para la conexion
y enviamos la informacion
Return
True ->Si se recibio el codigo 200 ok por parte del servidor
False ->Si se recibio algun otro codigo, es decir la informacion no se envio correctamente
El dominio secundario lo denomine asi porque recordar que el metodo que establece la conexion al servidor es por ejemplo www.kaliopeadmin.com.mx
ese dominio se convierte a una direccion IP que tu puedes consultar con el metodo AT+CDNSGIP=www.kaliopeadmin.com.mx, esa direccion ip es la del servidor
pero comunmente con esa misma IP el servidor maneja muchos otros nombres de dominios muchas otras paguinas web de diferentes propietarios,
entonces estableciste la conexion con el servidor de esa ip exitosamente
pero ahora el servidor debe saber a que subdominio va a enviar esa informacion, en este caso el subdominio seria kaliopeadmin.com.mx, ese se pone en la peticion HTTP
y entonces ahora si el servidor sabe a donde va a enviar la informacion o de que dominio la va a sacar
*/
bool ClaseManejoSim808::gprsEnviarInformacionPOST(String dominioSecundario, String archivoDeDestino, String informacion1, String informacion2, String informacion3, String informacion4, String informacion5)
{

    // la longuitud del campo informacion se tiene que pasar incluido el nombre del campo con el que se va a pasar
    //datos=holaComoEstas        Esta seria un tamaño de 19
    String campo1 = "d1=";
    String campo2 = "&d2=";
    String campo3 = "&d3=";
    String campo4 = "&d4=";
    String campo5 = "&d5=";

    int tamanoTotal = campo1.length() + informacion1.length() + campo2.length() + informacion2.length() + campo3.length() + informacion3.length() + campo4.length() + informacion4.length() + campo5.length() + informacion5.length();


    Serial.println(F("\n\n-METODO gprsEnviarInformacionPOST-\n\n"));
    Serial.println(String(informacion4.length()));
    //leer sobre las peticiones http te daras cuenta que llevan una estructura con cabeceras y saltos de linea para que el servidor la interprete correctamente
    String peticionHTTP = "POST /" + archivoDeDestino + " HTTP/1.1\r\n";                    
    peticionHTTP += "HOST: " + dominioSecundario + "\r\n";                                                      //encabezado obligatorio 
    peticionHTTP += "Content-Type: application/x-www-form-urlencoded\r\n";                                      //Header para definir que le vamos a enviar informacion de tipo de un "form" para en el body poder incluir el nombre del campo
    peticionHTTP += "Content-Length: "+ String(tamanoTotal) + "\r\n";                  //Header define cuandos bytes vamos a enviar, es necesario si mandas menos o mas la ifnroamcion se cortara
    //peticionHTTP += "Keep-Alive: timeout=5, max=100\r\n";                                                      //Trato de pedir que el servidor no cierre la conexion hasta que pasen 5s o 100 conexiones, pero en la documentacion de HTTP indica que no todos los servidores soportan esto
    //peticionHTTP += "Connection: Keep-Alive\r\n";                                                               //Este debe venir afuerzas junto con el KEEP alive de arriba, o este podria ser CLose indicando al servidor que cierre la conexion en cuanto termine esta peticion
    peticionHTTP += "Connection: Close\r\n";                                                               //Este debe venir afuerzas junto con el KEEP alive de arriba, o este podria ser CLose indicando al servidor que cierre la conexion en cuanto termine esta peticion
    peticionHTTP += "\r\n";                                                                                     //Siguiento la estructura de HTTP despues de definir los encabezados necesarios se envia el body con la informacion separada con un salto de linea, de los headers
    peticionHTTP += campo1 + informacion1 + campo2 + informacion2 + campo3 + informacion3 + campo4 + informacion4 + campo5 + informacion5 + "\r\n";                                                               //Este seria el body

    //y esta es la peticion de envio de datos por Post correcta terminada
    Serial.print(peticionHTTP);
    /*
    POST /gps_luisda.php HTTP/1.1
    HOST: 162.241.93.228
    Content-Type: application/x-www-form-urlencoded
    Content-Length: 46
    Keep-Alive: timeout=5, max=100
    Connection: Keep-Alive

    datos=Esta es la informacion que quiero enviar

    //modifique para que se puedan mandar campos diferentes como datos2 datos 3 etc
    d1=869170031697544,21/1/2020 18:40:3,19.795200,-99.878799,2569.9,2.64,352.049988,1.46,11,Location 3D Fix&d2=Hola2&datos3=&datos4=
    */








    /*
    Ya que creamos nuestra peticion con la informacion que vamos a enviar, enviamos al modulo el comando 
    AT+CIPSEND, el modulo nos respondera ">" indicando que podemos introducir la informacion que queremos enviar
    
    */

    char* info = enviarComandoSinEvaluar(AT_GPRS_ENVIAR_INFORMACION, _MENOR_A_128_BYTES, nullptr);
    if (strstr(info, ">") != nullptr) {
        //podemos escribir nuestra peticion HTTP con la informacion
        Serial3.print(peticionHTTP);            //no le enviamos otro salto de linea con serialPrinLn porque la peticion HTTP ya lo tiene al final
        //Serial3.Serial3.write(AT_SMS_FINALIZAR_ENVIO_SMS);  //enviamos el comando que indica que hemos finalizado el ingreso de datos y que puede enviarlos

        //mejor lo envio con mi metodo para poder leer la respuesta del modulo y evaluarla
        info = enviarComandoSinEvaluar(AT_SMS_FINALIZAR_ENVIO_SMS_2, 100, nullptr);
        /*
        CUando se manda el comando de finalizacion de linea lo que hace el modulo es que nos devuelve como respuesta
        la informacion que nosotros le enviamos para acegurar que la recibio, esa la leeremos aqui, porque en ella no vendra 
        la respuesta que nos interesa solo repetira lo que le enviamos, e incluso es probable que se desborde nuestro array donde guardamos las respeustas
        porque lo tenemos como 500 caracteres, y si por ejemplo envio 20 o 30 posiciones que esten guardadas en la memoria seran como 3mil bytes
        */
        Serial.println(F("Respuesta que retorna el sim confirmando el envio de los datos, son exactos a los que le enviamos"));
        Serial.println(info);
        /*
        Aqui se imprime lo mismo que le enviamos poruqe es la respuesta del modulo para dar por entendido que la informacion se recibio
        cuidado con el puerto serial a una velocidad de baudios muy alta la informacion que se imprime de las respuestas viene muy revuelta y aveces incompleta
        pero el arduino no tiene problemas es decir no la lee mal, solo no se visualiza bien en el serial. Cuando es mucha informacion de lectura, y si bajas
        a 9600 baudios se corrige aunuqe tambien depronto salen los errores
        POST /gps_luisda.php HTTP/1.1
        HOST: 162.241.93.228
        Content-Type: application/x-www-form-urlencoded
        Content-Length: 46
        Keep-Alive: timeout=5, max=100
        Connection: Keep-Alive

        datos=Esta es la informacion que quiero enviar
        */
        

        //usamos el mismo metodo pero le enviamos algo vacio, porque lo unico que queremos es usar la parte que lee la respuesta
        //lo dejamos por maximo 5 segundos esperando la respuesta, y usamos el campo Date: para que en cuanto llege esa palabra deje de leer
        //ya que lo demas no me interesa asi no tendremos que esperar los 5sy ahora si tendriamos la respuesta que nos interesa
        //haciendo mis pruebas en unas tardo 700ms en responder el servidor, en otras 1 s en otras 3 segundos y en otras casi los 4s
        info = enviarComandoSinEvaluar("", 5000, "Date:");
        Serial.println(F("Respuesta recibida de la conexion: "));
        Serial.println(info);
        /*
        SEND OK                                                                         -> esta es la informacion que nos indica que si se envio el mensaje y que lo recibio el servidor porque le envia bytes de confirmacion al modulo y en cuanto pasa eso el modulo nos responde SEND OK
        HTTP/1.1 200 OK                                                                 -> Este es el encabezado que nos devuelve el Servidor aqui indica que lo recibio correctamente pero podria venir otros codigos de error!! como 404 archivo no encontrado etc,400 badRequest, etc buscar los codigos http ahi vienen todos
        Date: Fri, 21 Feb 2020 23:12:25 GMT
        Server: Apache
        Connection: close
        Transfer-Encoding: chunked
        Content-Type: text/html; charset=UTF-8

        f
        STATUSKALIOPEOK                                                                     ->Este es un echo es el body de la http, este nos lo envia el archivo php con un echo de nuestro servidor
        0






        SEND OK                                        
        HTTP/1.1 200 OK                                 
        Date: Mon, 20 Jan 2020 20:03:41 GMT
        Server: Apache                                                      
        Content-Length: 0
        Connection: close
        Content-Type: tx/tl hre=T-              //por ejemplo este es un error de la reprecentacion en el puerto serial pero no inidica que el arduino lo leyera mas, sio bajas los baudios mostrara la info correcta

        Porque usamos Date: como mi palabra a detener cuando se ecnuetre?
        SEND OK
        HTTP/1.1 200 OK
        Date:

        porque asi recibimos los 2 codigos que nos interesan la respuesta del modulo y el encabezado del servidor
        porque si uso 200 OK habra okaciones donde la respuesta no sea 200 ok sino 401 file not found etc
        o si uso http entonces no tendre el codigo ya que solo tendria esto
        SEND OK
        HTTP
        pero ahora con la nueva modificacion del metodo que permite seguir recibiendo las respuests del buffer aunque ya se encontrara palabra clave

        */

        if (strstr(info, "SEND OK") != nullptr) {
            Serial.println(F("Informacio enviada correctamente"));

            String respuestaDelServer = strstr(info,"HTTP/1.1");
            Serial.println("Respuesta recibida del Server: \n" + respuestaDelServer);
            /*
            SEND OK                                                                  
            HTTP/1.1 200 OK                                                             
            Date: Fri, 21 Feb 2020 23:12:25 GMT
            Server: Apache
            Connection: close
            Transfer-Encoding: chunked
            Content-Type: text/html; charset=UTF-8

            f
            STATUSKALIOPEOK              -->Body                                                     
            0
            */

            respuestaDelServer = respuestaDelServer.substring(0, respuestaDelServer.indexOf('\n') - 1);           

            Serial.println("Codigo de respuesta del server: " + respuestaDelServer);
            /*
            HTTP/1.1 200 OK
            */

            if (respuestaDelServer.equals("HTTP/1.1 200 OK")) {
                Serial.print(F("EXITO!!Informacion recibida por el servidor exitosamente obtuvimos el codigo de respuesta "));
                Serial.println(respuestaDelServer);


                this->bodyDeRespuestaUltimaConexion = strstr(info, "STATUSKALIOPEOK");  //nos devolvera un string apartir de donde se encuentre esta palabra clave, esta palabra nos la devolera si o si nuestro archivo php para ubicar facilmente el body

                Serial.print(F("Body obtenido: "));
                Serial.println(this->bodyDeRespuestaUltimaConexion);



                Serial.println(F("\n\n-Fin METODO gprsEnviarInformacionPOST-\n\n"));

                return true;
            }
            else {
                Serial.print(F("Hubo exito al enviar la informacion al servidor pero se genero un error en la peticion HTTP el servidor retorno error "));
                Serial.println(respuestaDelServer);

                Serial.println(F("\n\n-Fin METODO gprsEnviarInformacionPOST-\n\n"));
                return false;
            }

        }
        else {
            Serial.print(F("El modulo sim no pudo enviar la informacion, revice la calidad de senal o el saldo del sim"));


            Serial.println(F("\n\n-Fin METODO gprsEnviarInformacionPOST-\n\n"));
            return false;
        }
        
    }
    else {
        //error el modulo gps no nos envia el simbolo para enviar la informacion

        Serial.println(F("El modulo no nos envio \">\" indicando que podemos enviarle datos "));

        Serial.println(F("\n\n-Fin METODO gprsEnviarInformacionPOST-\n\n"));
        return false;
    }


    Serial.println(F("\n\nSalimos por el retun de seguridad"));

    Serial.println(F("\n\n-Fin METODO gprsEnviarInformacionPOST-\n\n"));
    return false;
}









//Estos metodos son usados para cuando queremos enviar informacion de 1000bytes por vez
//ya que enviarInformacionPost es util solo para conectarce y enviar datos brevez y cierra su coneccion
//dividi ese metodo en 2 en abrirEnvioDeInformacionPost y cerrarEnvioDeInformacionPost para asi poder enviar 1000bytes almacenados en la SD
//por coneccion y no usar esos 1000bytes de la memoria SRAM del arduino

/*
Retorna 
true      ->      si el modulo esta en introduccion de datos es decir respondio ">" y podemos enviarle el cuerpo del mensaje
false    ->        si el modulo no respondio y no podemos enviar datos

Este metodo solo se encargara de enviar la peticion HTTP y despues esperara que se reciba el inductor >
indicando que podemos enviar informacion, es decir pondra al modulo en modo de recepcion de informacion

Despues con otro metodo nos encargaremos de finalizar y enviar la informacion devolviendo al modulo a su estado
original, 

Para que se hace esto, bueno el modulo sim tiene un buffer Serial de 1512bytes, es decir todos los datos que se le envien por serial
los guardara en este buffer, eso es una ventaja porque por ejemplo si quiero enviar de 1 sola vez 1000 bytes de informacion que estan guardados en la SD con el metodo de arriba
primero tengo que crear una variable en arduino que contendra la informacion total que saque de la memoria SD, esto significa que arduino tendra que separar en tiempo de ejecucion
1000bytes, para una memoria ram de 8mil bytes es una cantidad conciderable de memoria la que tiene que quedar dispónible para el arduino

con este metodo se anula esa debilidad, debido a que cuando el modulo nos responda ">" indicando que podemos enviar datos, lo dejaremos asi entonces
crearemos un bucle que por ejemplo leera la informaciuon guardada en la SD, byte a byte, y arduino lo que hara es actuar como un puente que solo tomara ese byte leido de la sd
lo enviara al modulo SIM808 el modulo lo guardara en su buffer de 1512bytes y le respondera al arduino el mismo byte que el le mando, arduino lo comprobara con el byte que envio
y entonces sabremos que lo recibio exitosamente, despues arduino volvera a leer otro byte de la SD y se lo enviara al modulo, y asi hasta que el modulo sim contenga los 1000bytes
de informacion guardada en su buffer, entonces arduino le enviara el comando de que ya tiene que enviar la informacion (char(26)) el modulo la enviara y respondera Send Ok

Segun mis pruebas el buffer fue de 1512 bytes porque cuando hice pruebas pude enviar 1480 bytes de una vez, si enviaba 1600 el modulo responde ERROR

ASi pude enviar 13 registros gps de 108bytes cada uno, dentro de este buffer se guarda el encabezado HTTP que se envia al inicio entonces
por eso es que creo es mejor solo enviar 1000bytes de tu informacion para guardar otros poquitos para la solicitud HTTP y que queden otros bytes libres
para no correr riesgos


Muy bien ahora aqui hay un problema importante, si yo le digo que le enviare un cuerpo de 1003 bytes, debo de enviar forzosamente esos 1003 bytes
si envio por ejemplo 963 solo responde Send Ok pero el servidor no responde 200 OK y no procesa la informacion,
le envie 984 y ese si lo proceso
pero bueno entiendo que tiene un pequeño margue o quizas fue un error, pero lo que si esta claro es que si defino
que le enviare 200 bytes son los bytes que tienen que venir en el cuerpo forzosamente, si no tendras muchos problemas jaja como yo!


Porque el metodo de envio de informacion de arriba funciona bien su debilidad es que si tienes una cantidad enorme de datos como las del acelerometro
o envias uno por uno lo cual es muy lento porque en cada dato hay que crear una conexion, o creas una variable que te contenga todos los datos que quieres enviar, quitandole memoria al arduino


*/
bool ClaseManejoSim808::gprsAbrirEnvioDeInformacionPOST(String dominioSecundario, String archivoDeDestino, long tamanoDelCuerpoDelMensaje, String info2)
{
    Serial.println(F("\n\n-METODO gprsAbrirEnvioDeInformacionPOST-\n\n"));




    int contadorDebytes = 0;
    bool errorEnEnvioDeFragmento = false;


    // la longuitud del campo informacion se tiene que pasar incluido el nombre del campo con el que se va a pasar
   //datos=holaComoEstas        Esta seria un tamaño de 19
    
    String campo2 = "d2=";
    String campo1 = "&d1=";


    

    //leer sobre las peticiones http te daras cuenta que llevan una estructura con cabeceras y saltos de linea para que el servidor la interprete correctamente
    String peticionHTTP = "POST /" + archivoDeDestino + " HTTP/1.1\r\n";
    peticionHTTP += "HOST: " + dominioSecundario + "\r\n";                                                      //encabezado obligatorio 
    peticionHTTP += "Content-Type: application/x-www-form-urlencoded\r\n";                                      //Header para definir que le vamos a enviar informacion de tipo de un "form" para en el body poder incluir el nombre del campo
    peticionHTTP += "Content-Length: " + String(campo1.length() + tamanoDelCuerpoDelMensaje + campo2.length() + info2.length()) + "\r\n";                  //Header define cuandos bytes vamos a enviar, es necesario si mandas menos o mas la ifnroamcion se cortara
    //peticionHTTP += "Keep-Alive: timeout=5, max=100\r\n";                                                      //Trato de pedir que el servidor no cierre la conexion hasta que pasen 5s o 100 conexiones, pero en la documentacion de HTTP indica que no todos los servidores soportan esto
    //peticionHTTP += "Connection: Keep-Alive\r\n";                                                               //Este debe venir afuerzas junto con el KEEP alive de arriba, o este podria ser CLose indicando al servidor que cierre la conexion en cuanto termine esta peticion
    peticionHTTP += "Connection: Close\r\n";                                                               //Este debe venir afuerzas junto con el KEEP alive de arriba, o este podria ser CLose indicando al servidor que cierre la conexion en cuanto termine esta peticion
    peticionHTTP += "\r\n";                                                                                     //Siguiento la estructura de HTTP despues de definir los encabezados necesarios se envia el body con la informacion separada con un salto de linea, de los headers
    
    
    
    
    
    peticionHTTP += "d2=" + info2 + "&d1=";                                                               //Este seria el body no enviamos saltos de linea ya que hasta la parte de abajo se va a llenar el body

    //y esta es la peticion de envio de datos por Post correcta terminada
    Serial.print(peticionHTTP);
    /*
    POST /gps_luisda.php HTTP/1.1
    HOST: 162.241.93.228
    Content-Type: application/x-www-form-urlencoded
    Content-Length: 1003
    Connection: Close
                                                                    ------ hasta aqui
    d2=888874141414444&d1=
    (Esto es lo unico que se enviara al principipo esto ya se guardara en el buffer del sim
    del puro encabezado son 138 bytes vamos a enviar en el campo 2 el imei esto lo añadimos

    tenemos que definir por fuerza el tamaño antes de enviar el cuerpo, porque claro como aun no sabemos cuanta informacion vamos a enviar
    tengo que dejar una cantidad fija, tengo duda que pasara si digo que mandare 1003 bytes y solo mando 1000, que pasara, que usara telcel para saber que has consumido datos?
    yo creo que no importaria el contara los bytes enviados despues del comando Send > porque por ejemplo aquneu el chip no tenga saldo telcel me deja hacer las conexiones al servidor
    y me responde conecct ok, pero lo que no sale es la informacion despues del > que nos responde el sim, etnonces me imagino que eso es lo que cobra, de igual manera el encabezado http
    ya se esta cobrando porque se manda despues del mensaje


A menos que para rellenar esos 1000 bytes use espacios en blanco pufff ahi si me precuparia

    entonces de la peticion http son 138bytes y del cuerpo seran 1003 bytes contando el identificador del campo "d1=" apartir de ese identificador enviaremos solo 1000bytes de cuerpo
    entonces en total estamos enviando como 1141 bytes y usando esos de los 1512 bytes del buffer creo que ahi estamos bien

    
    )
    */








    /*
    Ya que creamos nuestra peticion con la informacion que vamos a enviar, enviamos al modulo el comando
    AT+CIPSEND, el modulo nos respondera ">" indicando que podemos introducir la informacion que queremos enviar

    */

    char* info = enviarComandoSinEvaluar(AT_GPRS_ENVIAR_INFORMACION, _MENOR_A_128_BYTES, nullptr);
    if (strstr(info, ">") != nullptr) {

        Serial3.print(peticionHTTP);                    //enviamos nuetra peticion HTTP

        info = enviarComandoSinEvaluar("", _MENOR_A_258_BYTES, nullptr);
        //como el sim nos responde con la misma peticion que le enviamos indicando que lo recibio, 
        //el buffer serial del arduino estara lleno entonces usamos nuestro enviador de comandos solo para leer la respuesta que recibimos
        //para de igual manera vaciar todo el buffer serial del arduino

        Serial.println(F("\n\nHemos recibido el inductor \">\" indicando que apartir de ahora todo lo que se le envie al modulo sera cuerpo de la informacion"));

        

        Serial.println(F("\n\n-Fin METODO gprsAbrirEnvioDeInformacionPOST-\n\n"));
        return true;


    }
    else {
        Serial.println(F("\n\nEl Sim no contesto el inductor >"));
        Serial.println(F("\n\n-Fin METODO gprsAbrirEnvioDeInformacionPOST-\n\n"));
        return false;


    }




    
}
/*
True        ->La informacion fue enviada y recibida correctamente por el servidor Se recibio Send ok y ademas el servidor respondio 200 ok 
False       ->Hubo exito al enviar la informacion al servidor Se recibio Send Ok pero se genero un error en la peticion HTTP el servidor retorno un codigo diferente de 200 ok
False       -> El modulo sim no respondio Send Ok es decir no salieron los datos y el servidor no los recibio, Problema de saldo o de señal 
*/
bool ClaseManejoSim808::gprsCerrarEnvioDeInformacionPOST()
{
    //Este metodo es una copia exacta de la mitad del metodo enviarInformacionPost
    Serial.println(F("\n\n-METODO gprsCerrarEnvioDeInformacionPOST-\n\n"));
    char* info;

    Serial.println(F("Enviando el caracter 26 para indicarle al modulo que debe enviar los datos"));
    info = enviarComandoSinEvaluar(AT_SMS_FINALIZAR_ENVIO_SMS_2, 100, nullptr);
    /*
    CUando se manda el comando de finalizacion de linea lo que hace el modulo es que nos devuelve como respuesta
    la informacion que nosotros le enviamos para acegurar que la recibio, esa la leeremos aqui, porque en ella no vendra
    la respuesta que nos interesa solo repetira lo que le enviamos, e incluso es probable que se desborde nuestro array donde guardamos las respeustas
    porque lo tenemos como 500 caracteres, y si por ejemplo envio 20 o 30 posiciones que esten guardadas en la memoria seran como 3mil bytes
    */
    Serial.println(F("Respuesta que retorna el sim confirmando los datos, son exactos a los que le enviamos"));
    Serial.println(info);
    /*
    Aqui se imprime lo mismo que le enviamos poruqe es la respuesta del modulo para dar por entendido que la informacion se recibio
    cuidado con el puerto serial a una velocidad de baudios muy alta la informacion que se imprime de las respuestas viene muy revuelta y aveces incompleta
    pero el arduino no tiene problemas es decir no la lee mal, solo no se visualiza bien en el serial. Cuando es mucha informacion de lectura, y si bajas
    a 9600 baudios se corrige aunuqe tambien depronto salen los errores
    POST /gps_luisda.php HTTP/1.1
    HOST: 162.241.93.228
    Content-Type: application/x-www-form-urlencoded
    Content-Length: 46
    Keep-Alive: timeout=5, max=100
    Connection: Keep-Alive

    datos=Esta es la informacion que quiero enviar
    */


    //usamos el mismo metodo pero le enviamos algo vacio, porque lo unico que queremos es usar la parte que lee la respuesta
    //lo dejamos por maximo 5 segundos esperando la respuesta, y usamos el campo Date: para que en cuanto llege esa palabra deje de leer
    //ya que lo demas no me interesa asi no tendremos que esperar los 5sy ahora si tendriamos la respuesta que nos interesa
    //haciendo mis pruebas en unas tardo 700ms en responder el servidor, en otras 1 s en otras 3 segundos y en otras casi los 4s
    info = enviarComandoSinEvaluar("", 5000, "Date:");
    Serial.println(F("Respuesta recibida de la conexion"));
    Serial.println(info);
    /*
    SEND OK                                         -> esta es la informacion que nos indica que si se envio el mensaje y que lo recibio el servidor porque le envia bytes de confirmacion al modulo y en cuanto pasa eso el modulo nos responde SEND OK
    HTTP/1.1 200 OK                                 -> Este es el encabezado que nos devuelve el Servidor aqui indica que lo recibio correctamente pero podria venir otros codigos de error!! como 404 archivo no encontrado etc,400 badRequest, etc buscar los codigos http ahi vienen todos
    Date: Mon, 20 Jan 2020 20:03:41 GMT
    Server: Apache                                                      --usando el campo date solo tendremos en la respuesta hasta el date de arriba
    Content-Length: 0
    Connection: close
    Content-Type: tx/tl hre=T-              //por ejemplo este es un error de la reprecentacion en el puerto serial pero no inidica que el arduino lo leyera mas, sio bajas los baudios mostrara la info correcta

    Porque usamos Date: como mi palabra a detener cuando se ecnuetre?
    SEND OK
    HTTP/1.1 200 OK
    Date:

    porque asi recibimos los 2 codigos que nos interesan la respuesta del modulo y el encabezado del servidor
    porque si uso 200 OK habra okaciones donde la respuesta no sea 200 ok sino 401 file not found etc
    o si uso http entonces no tendre el codigo ya que solo tendria esto
    SEND OK
    HTTP


    */

    if (strstr(info, "SEND OK") != nullptr) {
        Serial.println(F("Informacio enviada correctamente"));

        String respuestaDelServer = strstr(info, "HTTP/1.1");
        Serial.println("Respuesta recibida del server \n" + respuestaDelServer);
        /*
        HTTP/1.1 200 OK
        Date: Mon, 20 Jan 2020 20:21:16 GMT
        Server: Apache
        Content-Length: 0
        Connection: close
        Content-Type: ethm;castUF8
        */

        respuestaDelServer = respuestaDelServer.substring(0, respuestaDelServer.indexOf('\n') - 1);

        Serial.println("Codigo de respuesta del Server: " + respuestaDelServer);
        /*
        HTTP/1.1 200 OK
        */

        if (respuestaDelServer.equals("HTTP/1.1 200 OK")) {
            Serial.print(F("EXITO!!Informacion recibida por el servidor exitosamente obtuvimos el codigo de respuesta "));
            Serial.println(respuestaDelServer);

            this->bodyDeRespuestaUltimaConexion = strstr(info, "STATUSKALIOPEOK");
            Serial.print(F("Body obtenido: "));
            Serial.println(this->bodyDeRespuestaUltimaConexion);




            Serial.println(F("\n\n-Fin METODO gprsEnviarInformacionPOST-\n\n"));
            return true;
        }
        else {
            Serial.print(F("Hubo exito al enviar la informacion al servidor pero se genero un error en la peticion HTTP el servidor retorno error. Sobreescribimos el metodo retornamos true en lugar de false para version SOFT 1.24 asi corregimos los datos repetidos en el servidor "));
            Serial.println(respuestaDelServer);


            Serial.println(F("\n\n-Fin METODO gprsEnviarInformacionPOST-\n\n"));
            return true;    //aqui retornabamos false


            /*
            El problema era que por algun motivo el servidor no respondia el 200 ok
            pero nuestro modulo si enviaba los datos, al responder false
            el otro metodo que usa a este, indicaba que no se habian enviado los archivos aunque
            claramente si lo habia hecho, porque el modulo sim808 respondio SEND OK
            entonces quizas el problema era que el servidor no alcanzaba a responder dentro de los 5 segundos que espera el procesador
            y eso ocacionaba el envio de datos dobles porque el otro metodo no actualizaba la posicion actual
            y seguia enviando y enviando los mismos datos hasta que se respndia 200ok
            */
        }

    }
    else {
        Serial.print(F("El modulo sim no pudo enviar la informacion, revice la calidad de senal o el saldo del sim"));



        Serial.println(F("\n\n-Fin METODO gprsCerrarEnvioDeInformacionPOST-\n\n"));
        return false;
    }



}
/*
@Return
String del ultimo body obtenido por la ultima conexion de cualqueira de los 2 metodos
tanto gprsEnviarInformacionPOST y gprsCerrarEnvioDeInformacionPOST
Si ese metodo retorno es nu conexion true buscara el body con la palabra clave STATUSKALIOPEOK 
y se guardara en nuestro campo bodyDeRespuestaUltimaConexion

esos metodos al recibir la respuesta del servidor separan el body y actualizan la variable 
que podremos obtener con este metodo
*/
String ClaseManejoSim808::gprsGetBodyUltimo()
{
    return this->bodyDeRespuestaUltimaConexion;
}




