/* Library for ADXL375  captor
Created by BARROUE Baptiste, 15/06/2016
*/

#include "Arduino.h"
#include "ADXL375.h"

ADXL375::ADXL375()
{
  byte buff[6];
  int j = 0;
  for(j=0;j<6;j++)
  {
  buff[j]=0;
  }
}


//reads num bytes starting from address register on device in to buff array
void ADXL375::readFrom(byte address, int num) {
  Wire.beginTransmission(ADXL375_address); //start transmission to device
  Wire.write(address);        //sends address to read from
  Wire.endTransmission(); //end transmission

  Wire.beginTransmission(ADXL375_address); //start transmission to device
  Wire.requestFrom(ADXL375_address, num);    // request 6 bytes from device

  int i = 0;
  while(Wire.available())    //device may send less than requested (abnormal)
  {
    buff[i] = Wire.read(); // receive a byte
    i++;
  }
  Wire.endTransmission(); //end transmission
}

//Writes val to address register on device
void ADXL375::writeTo(byte address, byte val) {
   Wire.beginTransmission(ADXL375_address); //start transmission to device
   Wire.write(address);        // send register address
   Wire.write(val);        // send value to write
   Wire.endTransmission(); //end transmission
     Serial.println("write ok");
}

  void ADXL375::getInterruption()
  {

  int int0 = LOW;
  int int1 = LOW;

  int0=digitalRead(2);
  int1=digitalRead(3);

  if(int0==HIGH){
   Serial.print("interruption 2!  :  INT_SOURCE = ");
   readFrom(0X30, 1);
   Serial.println(buff[0]);
   }
  if(int1==HIGH) {
   Serial.print("interruption 1!  :  INT_SOURCE = ");
   readFrom(0X30, 1);
   Serial.println(buff[0]);
   }
  }


  void ADXL375::getAcceleration()
  {

short int x;
short int y;
short int z;

float a,b,c;

   readFrom(0X32, 6);
   x = buff[0] | (buff[1] << 8);
   y = buff[2] | (buff[3] << 8);
   z = buff[4] | (buff[5] << 8);

   a = x/20.5;
   b = y/20.5;
   c = z/20.5;

    Serial.print("x = ");
     Serial.print(a);
      Serial.print("g");
    Serial.println();
     Serial.print("y = ");
      Serial.print(b);
       Serial.print("g");
    Serial.println();
     Serial.print("z = ");
      Serial.print(c);
       Serial.print("g");
    Serial.println();
  }


//configures the registers

void ADXL375::configADXL375(){


  writeTo(0X2A,0x07); // selects axes that participates to shock detection
  writeTo(0X2C,10);   // bandwidth standard
  writeTo(0X2D,0x38); // controls the function mode.measure, with serial activity detection and autosleep
  writeTo(0X31,11);   // 200g standard
  writeTo(0X38,0x20); // fifo config (bypass)




  writeTo(0X1E,0xF7); // offset x
  writeTo(0X1F,0xF9); // offset y
  writeTo(0X20,0xFD); // offset z



  writeTo(0X1D,0xFF); // THRESH_SHOCK, 780mg/LSB -> 0x7F = 100g
  writeTo(0X21,0x0F); // Shock duration. 625µs/LSB
  writeTo(0X22,0x0F); // delay between two shocks. 1.25ms/LSB
  writeTo(0X23,0xFF); // window to trigger double shock 1.25ms/LSB


  writeTo(0X24,0x20); // THRESH_ACT, detects activity if Measure>thresh act. 780mg/LSB
  writeTo(0X25,0x10); // Thresh_inact; detects inactivity if Measure<thresh. 780mg/LSB
  writeTo(0X26,0x7F); // time needed to detect inactivity. 1s/LSB
  writeTo(0X27,0x77); // Act_inact_ctl. all axes contribution


  writeTo(0X2E,0x00); // clears interrups for configuration
  writeTo(0X2F,0xDB); // selects the pins where interruptions are directed
  writeTo(0X2E,0x60); // enables or disables interruptions

}
