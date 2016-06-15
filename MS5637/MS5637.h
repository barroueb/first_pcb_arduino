/* Library for MS5637 pressure captor
Created by BARROUE Baptiste, 14/06/2016
*/
#ifndef MS5637_h
#define MS5637_h

#include "Arduino.h"
#include <Wire.h>

#define MS5637Addr (0x76)
#define TO_READ    (6)

class MS5637 {
  public :
  MS5637();
  void getMS5637data(float* pressure , float* ctemp, float* ftemp);
  void displayMS5637data(float pressure, float ctemp, float ftemp);
  void initMS5637();

  private :
  unsigned long Coff[6], Ti , offi , sensi;
  unsigned int data[3];
  byte buff[TO_READ];
  char str [512]; //13 bit resolution

};

#endif
