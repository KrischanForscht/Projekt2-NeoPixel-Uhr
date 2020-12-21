// Nachdem mit dem ESP Probleme m it Big Sur auftraten, erst mal mit Arduino UNO alles zum, Laufen bringen.
//#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <FastLED.h>
#include <FastLED_NeoMatrix.h>
#include <Adafruit_BMP085.h>
#include <Adafruit_CCS811.h>
#include "virtuabotixRTC.h"    

virtuabotixRTC myRTC(16,27,14); //Uhr ESP32  (CLK,DAT,RST)
//virtuabotixRTC myRTC(5,4,7); //Uno
Adafruit_CCS811 ccs;//Co2 Sensor
Adafruit_BMP085 bmp;


#ifndef ESP32
#define delay FastLED.delay
#endif

#define PIN 12 
#define BM32
#define BRIGHTNESS 10

#define mw 32
#define mh 8
#define NUMMATRIX (mw*mh)
CRGB leds[NUMMATRIX];
FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(leds, 8, 8, 4, 1, 
NEO_MATRIX_BOTTOM     + NEO_MATRIX_RIGHT +
NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG + 
NEO_TILE_TOP + NEO_TILE_RIGHT +  NEO_TILE_PROGRESSIVE);

void displayText(char text[6], int r, int g, int b) {
 // Serial.print("Das ist bei DisplayText angekommen:");
  //Serial.println(text);
  
  matrix->setTextSize(1);
  matrix->clear();
  matrix->setCursor(0, 0);
  matrix->setTextColor(matrix->Color(r,g,b));
  matrix->print(text);
//matrix->print('x');
  matrix->show();
}

void displayFloat(float Zahl,int nachkomma, int r, int g, int b) {
  char str[5];
  dtostrf(Zahl, 5, nachkomma, str);
  /*
  Serial.print("Das ist bei DisplayFloat angekommen:");
  Serial.println(Zahl);
  Serial.print("und das wurde daraus:");
  Serial.println(str);
  */
  matrix->setTextSize(1);
  matrix->clear();
  matrix->setCursor(0, 0);
  matrix->setTextColor(matrix->Color(r,g,b));
  matrix->print(str);
//matrix->print('x');
  matrix->show();
}
    
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Los gehts mit LED");

  FastLED.addLeds<NEOPIXEL,PIN>(  leds, NUMMATRIX  ).setCorrection(TypicalLEDStrip);
  matrix->begin();
  matrix->setTextWrap(false);
  matrix->setBrightness(BRIGHTNESS);

  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP sensor, check wiring!");
  while (1) {}
  }
  
  if(!ccs.begin()){
    Serial.println("Failed to start sensor! Please check your wiring.");
    while(1);
  }
  while(!ccs.available());
  for(int i=0;i<255;i++){ displayText("Moin!",i,255-i,i);delay(10);}
//Set the current date, and time in the following format:
//seconds, minutes, hours, day of the week, day of the month, month, year
//myRTC.setDS1302Time(00, 40, 23, 4, 26, 11, 2020);
}

void loop() {

  int pause=2000;
  
  //Zeit und Datum
  myRTC.updateTime(); 
  displayText("Datum",255,255,0);
  delay(pause);
  char text[6]="";
  char day[3];
  char month[3];
  dtostrf(myRTC.dayofmonth,2,0,day);
  dtostrf(myRTC.month,2,0,month);
  strcat(text,day);
  strcat(text,"/");
  strcat(text,month);
  Serial.println(text);
  displayText(text,255,255,00);
  delay(pause);
  char text2[6]="";
  char hours[3];
  char minutes[3];
    
  dtostrf(myRTC.hours,2,0,hours);
  dtostrf(myRTC.minutes,2,0,minutes);
  strcat(text2,hours);
  strcat(text2,":");
  strcat(text2,minutes);
  displayText("Zeit",255,255,255);
  delay(pause);
  Serial.println(text2);
  displayText(text2,255 ,255 ,255);
  delay(pause);

//Temperatur und Druck BMP180
  Serial.print("Temperature = ");
  Serial.print(bmp.readTemperature());
  Serial.println(" *C");
  
  displayText("Temp.",255,0,0);
  delay(pause);  
  displayFloat(bmp.readTemperature(),1,255,0,0);
  delay(pause);  
  
  Serial.print("Pressure = ");
  Serial.print(bmp.readPressure());
  Serial.println(" Pa");
  
  displayText("Druck",0,255,0);
  delay(pause);
  displayFloat(bmp.readPressure()/100.0,0,0,255,0);
  delay(pause);

  //Gas
  if(ccs.available()){
    if(!ccs.readData()){
      displayText(" CO2",0,0,255);
      delay(pause);
      displayFloat(ccs.geteCO2(),2,0,0,255);
      delay(pause);
      displayText("TVOC ",0,255,255);
      delay(pause);
      displayFloat(ccs.getTVOC(),1,0,255,255);
      delay(pause);
    }
    else{
      Serial.println("ERROR!");
      }
    }

  //Bodenfeuchte
  double summe = 0;
  for (int i=0; i<1000;i++){
    int sensorValue = analogRead(39);
    summe=summe+ sensorValue;
    delay(1);
  }
  summe=summe/1000.0;
  summe=summe/40.95;
  summe=100-summe;
  Serial.print("Palme: ");
  Serial.println(summe);

  displayText("Palme",0,255,0);
  delay(pause);
  displayFloat(summe,1,0,255,0);
  delay(pause);  
  
}
