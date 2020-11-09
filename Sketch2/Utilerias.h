#pragma once
#include <Arduino.h>
#include <Time.h>

//---Modificado desde arducam


class Utilerias {
public:
	static time_t convertirHoraUTC(time_t timeUTC,int zonaHoraria) {
     

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

    static String horaEnZonaHoraria(time_t timeUTC, int zonaHoraria) {
        //dd/MM/aaaa hh/mm/ss
        time_t tiempo = convertirHoraUTC(timeUTC, zonaHoraria);

        return String(day(tiempo)) + "/" + String(month(tiempo)) + "/" + String(year(tiempo)) + "," + String(hour(tiempo)) + ":" + String(minute(tiempo)) + ":" + String(second(tiempo));
    }




    /*
    En este metodo devolvemos un string con el formato 
    27-01-2020 08:59:01
    
    Si la fecha devuelta no es mayor a 10 entonces añadimos un cero para mantener bien el formato
    */
    static String imprimirFechaHoraDigitos(time_t time) {
        return imprimirFechaDigitos(time)+ "," + imprimirHoraDigitos(time);

    }

   static String imprimirHoraDigitos(time_t time) {

        int min = minute(time);
        int hor = hour(time);
        int sec = second(time);
        String resultadoHora = "";
        if (hor < 10) {
            resultadoHora += "0" + String(hor);
        }
        else {
            resultadoHora += String(hor);
        }

        if (min < 10) {
            resultadoHora += ":0" + String(min);
        }
        else {
            resultadoHora += ":" + String(min);
        }

        if (sec < 10) {
            resultadoHora += ":0" + String(sec);
        }
        else {
            resultadoHora += ":" + String(sec);
        }

        return resultadoHora;

    }

   static String imprimirFechaDigitos(time_t time) {
       int dia = day(time);
       int mes = month(time);
       int ano = year(time);
       String resultadoFecha = "";

       if (dia < 10) {
           resultadoFecha += "0" + String(dia);
       }
       else {
           resultadoFecha += String(dia);
       }

       if (mes < 10) {
           resultadoFecha += "-0" + String(mes);
       }
       else {
           resultadoFecha += "-" + String(mes);
       }

       
           resultadoFecha += "-" + String(ano);
       

       return resultadoFecha;
   }







//comprobaremos que cualquier fecha pasada a este metodo
//sea vigente, cuando el gps y o el modulo rtc no estan atiempo retornan 
//0-0-1970 0:0:0
//lo que haremos es verificar que la fecha sea menor a 2015 este numero puede ser cualquiera jaja
//asi si la fecha que nos paso el modulo RTC o el modulo gps por ejemplo es menor al 2015 sabemos que esa fecha no esta actualizada yu que debemos configurarla
//o en el caso del modulo gps es que aun no ha adquirido la señal correctmaente
//En la practiva algunas veces me di cuenta que ciertos modulos devolvian el año 2176
  static bool tiempoMayor2015(time_t fechaAcomparar) {

       Serial.println(F("\n-Metodo tiempoMayoral2015ymenora2050-\n"));


       //el tiempo que se indique sera apartir de 1970 entonces si yo quiero indicar que sera 2016 mandare 46 años o restar al año 1970
       tmElements_t Fecha;
       Fecha.Second = 0;
       Fecha.Minute = 0;
       Fecha.Hour = 0;
       Fecha.Day = 1;
       Fecha.Month = 1;
       Fecha.Year = 2015 - 1970;
       //01-01-2015 00:00:00   

       time_t comparacion = makeTime(Fecha);


       tmElements_t FechaLimite;
       FechaLimite.Second = 0;
       FechaLimite.Minute = 0;
       FechaLimite.Hour = 0;
       FechaLimite.Day = 1;
       FechaLimite.Month = 1;
       FechaLimite.Year = 2050 - 1970;
       //01-01-2050 00:00:00 
       time_t limite = makeTime(FechaLimite);


       Serial.println(String(fechaAcomparar));
       Serial.println(String(day(fechaAcomparar)) + "-" + String(month(fechaAcomparar)) + "-" + String(year(fechaAcomparar)) + " " + String(hour(fechaAcomparar)) + ":" + String(minute(fechaAcomparar)) + ":" + String(second(fechaAcomparar)));

       Serial.println(String(comparacion));
       Serial.println(String(day(comparacion)) + "-" + String(month(comparacion)) + "-" + String(year(comparacion)) + " " + String(hour(comparacion)) + ":" + String(minute(comparacion)) + ":" + String(second(comparacion)));

       Serial.println(String(limite));
       Serial.println(String(day(limite)) + "-" + String(month(limite)) + "-" + String(year(limite)) + " " + String(hour(limite)) + ":" + String(minute(limite)) + ":" + String(second(limite)));






       if (fechaAcomparar > comparacion && fechaAcomparar < limite) {
           Serial.println(F("La fecha a evaluar es vigente, es mayor que 2015 y menor al 2050"));
           Serial.println(F("\n-Fin Metodo tiempoMayoral2015ymenora2050-\n"));
           return true;
       }
       else {
           Serial.println(F("La fecha a evaluar no es vigente es menor que 2015 o mayor al 2050"));
           Serial.println(F("\n-Fin Metodo tiempoMayoral2015ymenora2050-\n"));
           return false;
       }



   }

  /*
  True si las horas pasadas tienen una diferencia de mas de 10 segundos
  False si las horas pasadas estan dentro de esos 10 segundos

  Esto se usa para comparar las horas del modulo RTC y del modulo GPS
  para que si el modulo RTC ha sufrido un desface con el tiempo
  al detectar que es mayor a 10segundos se vuelva a configuirar el tiempo
  con el del modulo gps
  */
  static bool desfaseDeTiempo10Segundos(time_t fecha1, time_t fecha2) {
       Serial.println(F("\n-desfaseDeTiempo10Segundos-\n"));
       Serial.println(String(fecha1) + "-" + String(fecha2));
       long temporal = fecha1 - fecha2;

       if (temporal > 10 || temporal < -10) {
           Serial.print(F("Las horas son diferentes por:"));
           Serial.println(String(temporal));
           Serial.println(F("\n-Fin desfaseDeTiempo10Segundos-\n"));
           return true;
       }
       else {
           Serial.print(F("Las horas estan dentro de la tolerancia "));
           Serial.println(String(temporal));
           Serial.println(F("\n-Fin desfaseDeTiempo10Segundos-\n"));
           return false;
       }

   }
};