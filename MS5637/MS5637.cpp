/* Library for MS5637 pressure captor
Created by BARROUE Baptiste, 14/06/2016
*/

#include "Arduino.h"
#include "MS5637.h"

MS5637::MS5637()
{
  Coff[6];
  Ti = 0;
  offi = 0;
  sensi = 0;
  }

void MS5637::getMS5637data(float* pressure , float* ctemp, float* ftemp)
{

   //reset commands
// Start I2C Transmission
Wire.beginTransmission(MS5637Addr);
// Send reset command
Wire.write(0x1E);
// Stop I2C Transmission
Wire.endTransmission();
// Start I2C Transmission
Wire.beginTransmission(MS5637Addr);
// Refresh pressure with the OSR = 256
Wire.write(0x40);
// Stop I2C Transmission
Wire.endTransmission();

   //pressure read
delay(500);
// Start I2C Transmission
Wire.beginTransmission(MS5637Addr);
// Select data register
Wire.write(0x00);
// Stop I2C Transmission
Wire.endTransmission();
// Request 3 bytes of data
Wire.requestFrom(MS5637Addr, 3);
// Read 3 bytes of data
// ptemp_msb1, ptemp_msb, ptemp_lsb
if(Wire.available() == 3)
{
data[0] = Wire.read();
data[1] = Wire.read();
data[2] = Wire.read();
}
// Convert the data
unsigned long ptemp = ((data[0] * 65536.0) + (data[1] * 256.0) + data[2]);



   //temperature read
// Start I2C Transmission
Wire.beginTransmission(MS5637Addr);
// Refresh temperature with the OSR = 256
Wire.write(0x50);
// Stop I2C Transmission
Wire.endTransmission();

delay(500);
// Start I2C Transmission
Wire.beginTransmission(MS5637Addr);
// Select data register
Wire.write(0x00);
// Stop I2C Transmission
Wire.endTransmission();
// Request 3 bytes of data
Wire.requestFrom(MS5637Addr, 3);
// Read 3 bytes of data
// temp_msb1, temp_msb, temp_lsb
if(Wire.available() == 3)
{
data[0] = Wire.read();
data[1] = Wire.read();
data[2] = Wire.read();
}
// Convert the data
unsigned long temp = ((data[0] * 65536.0) + (data[1] * 256.0) + data[2]);




// Pressure and Temperature Calculations
// 1st order temperature and pressure compensation
// Difference between actual and reference temperature
unsigned long dT = temp - ((Coff[4] * 256));
temp = 2000 + (dT * (Coff[5] / pow(2, 23)));
// Offset and Sensitivity calculation
unsigned long long off = Coff[1] * 131072 + (Coff[3] * dT) / 64;
unsigned long long sens = Coff[0] * 65536 + (Coff[2] * dT) / 128;
// 2nd order temperature and pressure compensation
if(temp < 2000)
{
Ti = (dT * dT) / (pow(2,31));
offi = 5 * ((pow((temp - 2000), 2))) / 2;
sensi = offi / 2;
if(temp < - 1500)
{
offi = offi + 7 * ((pow((temp + 1500), 2)));
sensi = sensi + 11 * ((pow((temp + 1500), 2)));
}
}
else if(temp >= 2000)
{
Ti = 0;
offi = 0;
sensi = 0;
}
// Adjust temp, off, sens based on 2nd order compensation
temp -= Ti;
off -= offi;
sens -= sensi;
// Convert the final data
ptemp = (((ptemp * sens) / 2097152) - off);
ptemp /= 32768.0;


*pressure = ptemp / 100.0;
*ctemp = temp / 100.0;
*ftemp = *ctemp * 1.8 + 32.0;

}

void MS5637::displayMS5637data(float pressure, float ctemp, float ftemp)
{
  // Output data to serial monitor
Serial.print("Temperature in Celsius : ");
Serial.print(ctemp);
Serial.println(" C");
Serial.print("Temperature in Fahrenheit : ");
Serial.print(ftemp);
Serial.println(" F");
Serial.print("Pressure : ");
Serial.print(pressure);
Serial.println(" mbar");
delay(500);
}

void MS5637::initMS5637(){

    // Read cofficients values stored in EPROM of the device
    for(int i = 0; i < 6; i++)
  {
  // Start I2C Transmission
  Wire.beginTransmission(MS5637Addr);
  // Select data register
  Wire.write(0xA2 + (2 * i));
  // Stop I2C Transmission
  Wire.endTransmission();
  // Request 2 bytes of data
  Wire.requestFrom(MS5637Addr, 2);
  // Read 2 bytes of data
  // Coff msb, Coff lsb
  if(Wire.available() == 2)
  {
  data[0] = Wire.read();
  data[1] = Wire.read();
  }
  // Convert the data
  Coff[i] = ((data[0] * 256) + data[1]);
  }
}
