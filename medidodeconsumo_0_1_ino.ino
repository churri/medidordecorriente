/*   RTC info and lib
   https://learn.adafruit.com/adafruit-ds3231-precision-rtc-breakout/wiring-and-test

led1  D8   Power led - (red)
led2  D7   Write led
led3  D6   Error led
SD card   CS - pin 2
  
*/
//Para RTC
#include <Wire.h> 
#include "RTClib.h"
#include <SD.h>

RTC_DS1307 RTC;
const int chipSelect = 2;


void setup() {
  pinMode(8, OUTPUT); // red Led for power
  pinMode(7, OUTPUT); // Write led
  pinMode(6, OUTPUT); // Error led
  digitalWrite(8, HIGH); // Powered on
  Wire.begin(); // Inicia el puerto I2C
  RTC.begin(); // Inicia la comunicación con el RTC
  //RTC.adjust(DateTime(__DATE__, __TIME__)); // Establece la fecha y hora (Comentar una vez establecida la hora)
  analogReference(INTERNAL);
  Serial.begin(9600);
  if (!SD.begin(chipSelect)) {
    blink_led(6, 3);  // if card init fail blink 3 times
  return;
  Serial.print("Y/m/D H M S  Consumo 1 ");
  } 
}

void loop() {
  digitalWrite(7,HIGH);
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  if (dataFile) {
    float Irms1=get_corriente(A0); //Corriente eficaz (A)
    float Irms2=get_corriente(A1); //Corriente eficaz (A)
    float Irms3=get_corriente(A2); //Corriente eficaz (A)
    float Irms4=get_corriente(A3); //Corriente eficaz (A)
    if (writeDate(dataFile)){
      digitalWrite(7,LOW);
      delay(400);
    };  
    dataFile.print(Irms1,3);
    dataFile.print(',');
    dataFile.print(Irms2,3);
    dataFile.print(',');
    dataFile.print(Irms3,3);
    dataFile.print(',');
    dataFile.print(Irms4,3);
       
    
    Serial.print("-->");
    Serial.print(Irms1,3);
    
    Serial.print("-->");
    Serial.print(Irms2,3);

    Serial.print("-->");
    Serial.print(Irms3,3);

    Serial.print("-->");
    Serial.print(Irms4,3);
    
    Serial.println();
    
    dataFile.println();
    
    dataFile.close();
  } 
  else {
    blink_led(6, 2);  // if card init fail blink 3 times
  }  
}

char writeDate(File dataFile){
  DateTime now = RTC.now(); // Obtiene la fecha y hora del RTC
  dataFile.print(now.year(), DEC); // Año
  dataFile.print(',');
  dataFile.print(now.month(), DEC); // Mes
  dataFile.print(',');
  dataFile.print(now.day(), DEC); // Dia
  dataFile.print(',');
  dataFile.print(now.hour(), DEC); // Horas
  dataFile.print(',');
  dataFile.print(now.minute(), DEC); // Minutos
  dataFile.print(',');
  dataFile.print(now.second(), DEC); // Segundos
  dataFile.print(',');
}


float get_corriente(char PORT){
  float voltajeSensor;
  float corriente=0;
  float Sumatoria=0;
  long tiempo=millis();
  int N=0;
  while(millis()-tiempo<500)//Duración 0.5 segundos(Aprox. 30 ciclos de 60Hz)
  { 
    voltajeSensor = analogRead(PORT) * (1.1 / 1023.0);////voltaje del sensor
    corriente=voltajeSensor*50.0; //corriente=VoltajeSensor*(30A/1V)
    Sumatoria=Sumatoria+sq(corriente);//Sumatoria de Cuadrados
    N=N+1;
    delay(1);
  }
  Sumatoria=Sumatoria*2;//Para compensar los cuadrados de los semiciclos negativos.
  corriente=sqrt((Sumatoria)/N); //ecuación del RMS
  if ( corriente == 77.78) {
    Serial.print(corriente,3);
    corriente = 0;
  }
  return(corriente);
}

void blink_led(int pinled,int times){
   Serial.print(pinled);
   digitalWrite(pinled,LOW);
    for(int x=0; x<times; x++){
       digitalWrite(pinled,HIGH);
       delay(400);
       digitalWrite(pinled,LOW);
       delay(400);
    }
}
