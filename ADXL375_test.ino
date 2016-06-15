#include <Wire.h>
#include <ADXL375.h>

ADXL375 adxl375;

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &=~_BV(bit))
#endif
void setup() {
  // put your setup code here, to run once:
Wire.begin();
#if defined(_AVR_ATmega168_) || defined(_AVR_ATmega8_) || defined(_AVR_ATmega328P_)
cbi(PORTC, 4);
cbi(PORTC, 5);
#else
cbi(PORTD, 0);
cbi(PORTD, 1);
#endif
Serial.begin(9600);

// init ADXL375
 adxl375.configADXL375();
pinMode(2, INPUT);
pinMode(3, INPUT);
  Serial.println("init");
   delay(300);
}

void loop() {
  adxl375.getInterruption(); 
  adxl375.getAcceleration(); 
Serial.println("loop");
   delay(3000);
}


