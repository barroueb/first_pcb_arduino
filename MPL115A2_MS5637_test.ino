#include <Wire.h>
#include <Adafruit_MPL115A2.h>
Adafruit_MPL115A2 mpl115a2;
#include <MS5637.h>
MS5637 ms5637;

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &=~_BV(bit))
#endif


void setup() {
  

// Initialise I2C communication as MASTER
Wire.begin();
#if defined(_AVR_ATmega168_) || defined(_AVR_ATmega8_) || defined(_AVR_ATmega328P_)
cbi(PORTC, 4);
cbi(PORTC, 5);
#else
cbi(PORTD, 0);
cbi(PORTD, 1);
#endif

Serial.begin(9600);
// Initialise Serial Communication, set baud rate = 9600
//read calibration data from MS5637
 ms5637.initMS5637();
//inits MPL115A2
 mpl115a2.begin();


  Serial.println("init");
   delay(300);
}

void loop() { 


  //MS5637 vars
  float pressure;
  float ctemp;
  float fTemp;
 
  
  ms5637.getMS5637data(&pressure , &ctemp, &fTemp);
Serial.println("-----MS5637");
  ms5637.displayMS5637data(pressure, ctemp, fTemp);

 
  Serial.println("-----MPL115a2-----");
  delay(300);
  //MPL115A2 vars 
 float pressureKPA = 0, temperatureC = 0;
  mpl115a2.getPT(&pressureKPA,&temperatureC);
  
 Serial.print("Pressure (kPa): "); Serial.print(pressureKPA, 4); Serial.print(" kPa  ");
 Serial.print("Temp (*C): "); Serial.print(temperatureC, 1); Serial.println(" *C both measured together");

}

