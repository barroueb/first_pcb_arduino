/* Library for ADXL375 captor
Created by BARROUE Baptiste, 15/06/2016
*/
#ifndef ADXL375_h
#define ADXL375_h

#include "Arduino.h"
#include <Wire.h>

#define ADXL375_address (0x53)

class ADXL375
{
  public :
  ADXL375();
  void configADXL375();
  void writeTo(byte address, byte val);
  void readFrom(byte address, int num);
  void getAcceleration();
  void getInterruption();
  byte buff[6];


};
#endif
