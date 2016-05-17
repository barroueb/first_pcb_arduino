#include <Wire.h>


// ADXL375 I2C address is 0x53
#define ADXL375 (0x53)
#define TO_READ (6)
// MS5637_02BA03 I2C address is 0x76(118)
#define MS5637Addr 0x76

unsigned long Coff[6], Ti = 0, offi = 0, sensi = 0;
unsigned int data[3];
byte buff[TO_READ];
char str [512]; //13 bit resolution
 
//MPL115A2 library
#include <Adafruit_MPL115A2.h>

Adafruit_MPL115A2 mpl115a2;


//Hardware pin definitions
int TEMPOUT = A1; // Output from the Temp sensor
int UVOUT = A2; //Output from the UV sensor
int REF_3V3 = A5; //3.3V power on the Arduino board


void setup()
{
// Initialise I2C communication as MASTER
Wire.begin();
// Initialise Serial Communication, set baud rate = 9600
Serial.begin(9600);
//read calibration data from MS5637
 initMS5637(Coff, data);
//inits MPL115A2
 mpl115a2.begin();
// init ADXL375
 configADXL375();
//define analog pins
 pinMode(TEMPOUT, INPUT);
 pinMode(UVOUT, INPUT);
 pinMode(REF_3V3, INPUT);

delay(300);
}


void loop() 
{
  
  //MPL115A2 vars 
  float pressureKPA = 0, temperatureC = 0;
  //MS5637 vars
  float pressure;
  float ctemp;
  float fTemp;
  //analogue vars
  int tempLevel = averageAnalogRead(TEMPOUT);
  int uvLevel = averageAnalogRead(UVOUT);
  int refLevel = averageAnalogRead(REF_3V3);
  
  
  getMS5637data(Coff, data, &Ti, &offi, &sensi, &pressure , &ctemp, &fTemp);

  displayMS5637data(pressure, ctemp, fTemp);

  displayTempUvValues(tempLevel, uvLevel, refLevel);
  
  
  mpl115a2.getPT(&pressureKPA,&temperatureC);
  Serial.print("Pressure (kPa): "); Serial.print(pressureKPA, 4); Serial.print(" kPa  ");
  Serial.print("Temp (*C): "); Serial.print(temperatureC, 1); Serial.println(" *C both measured together");
}

//----------------------ADXL375 functions-----------------------------//
//configures the registers

void configADXL375(){
  writeTo(ADXL375,0X2A,0x07); // selects axes that participates to shock detection
  writeTo(ADXL375,0X2C,10);   // bandwidth standard
  writeTo(ADXL375,0X2D,0x38); // controls the function mode.measure, with serial activity detection and autosleep
  writeTo(ADXL375,0X31,11);   // 200g standard
  writeTo(ADXL375,0X38,0x20); // fifo config (bypass)
  
  writeTo(ADXL375,0X1E,0x00); // offset x
  writeTo(ADXL375,0X1F,0x00); // offset y
  writeTo(ADXL375,0X20,0x00); // offset z
  

  
  writeTo(ADXL375,0X1D,0x7F); // THRESH_SHOCK, 780mg/LSB -> 0x7F = 100g
  writeTo(ADXL375,0X21,0x0F); // Shock duration. 625µs/LSB
  writeTo(ADXL375,0X22,0x0F); // delay between two shocks. 1.25ms/LSB
  writeTo(ADXL375,0X23,0xFF); // window to trigger double shock 1.25ms/LSB
  
  writeTo(ADXL375,0X24,0x20); // THRESH_ACT, detects activity if Measure>thresh act. 780mg/LSB
  writeTo(ADXL375,0X25,0x10); // Thresh_inact; detects inactivity if Measure<thresh. 780mg/LSB
  writeTo(ADXL375,0X26,0x7F); // time needed to detect inactivity. 1s/LSB
  writeTo(ADXL375,0X27,0x77); // Act_inact_ctl. all axes contribution
  
  writeTo(ADXL375,0X2E,0x00); // clears interrups for configuration
  writeTo(ADXL375,0X2F,0x9B); // selects the pins where interruptions are directed
  writeTo(ADXL375,0X2E,0x78); // enables or disables interruptions
  }

//Writes val to address register on device
void writeTo(int device, byte address, byte val) {
   Wire.beginTransmission(device); //start transmission to device
   Wire.write(address);        // send register address
   Wire.write(val);        // send value to write
   Wire.endTransmission(); //end transmission
}
 
 
//reads num bytes starting from address register on device in to buff array
void readFrom(int device, byte address, int num, byte buff[]) {
  Wire.beginTransmission(device); //start transmission to device
  Wire.write(address);        //sends address to read from
  Wire.endTransmission(); //end transmission
 
  Wire.beginTransmission(device); //start transmission to device
  Wire.requestFrom(device, num);    // request 6 bytes from device
 
  int i = 0;
  while(Wire.available())    //device may send less than requested (abnormal)
  {
    buff[i] = Wire.read(); // receive a byte
    i++;
  }
  Wire.endTransmission(); //end transmission
}
//---------UV and temperature analogue captors functions-------------//

void displayTempUvValues(int tempLevel, int uvLevel, int refLevel){
  
  //Use the 3.3V power pin as a reference to get a very accurate output value from sensor
  float outputTempVoltage = 3.3 / refLevel * tempLevel;
  float outputUvVoltage = 3.3 / refLevel * uvLevel;
  
  float tempIntensity = mapfloat(outputTempVoltage, 0.99, 2.8, -55, 130); //Convert the voltage to a UV intensity level
  float uvIntensity = mapfloat(outputUvVoltage, 0.99, 2.8, 0.0, 15.0); //Convert the voltage to a UV intensity level

  Serial.print("ref output: ");
  Serial.print(refLevel);

  Serial.print("ML8511 output: ");
  Serial.print(uvLevel);

  Serial.print(" / ML8511 voltage: ");
  Serial.print(outputUvVoltage);

  Serial.print(" / UV Intensity (mW/cm^2): ");
  Serial.print(uvIntensity);
  
  Serial.println();
  
  Serial.print("TMP20 output: ");
  Serial.print(tempLevel);

  Serial.print(" / TMP20 voltage: ");
  Serial.print(outputTempVoltage);

  Serial.print(" / Temperature Intensity (°): ");
  Serial.print(tempIntensity);
  
  
  delay(100);
  }




//Takes an average of readings on a given pin
//Returns the average
int averageAnalogRead(int pinToRead)
{
  byte numberOfReadings = 8;
  unsigned int runningValue = 0; 

  for(int x = 0 ; x < numberOfReadings ; x++)
    runningValue += analogRead(pinToRead);
  runningValue /= numberOfReadings;

  return(runningValue);  
}

//The Arduino Map function but for floats
//From: http://forum.arduino.cc/index.php?topic=3922.0

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}





//----------------------MS5637 Functions----------------------------//
//get and calculates the data from MS5637

void getMS5637data(unsigned long Coff[], unsigned int data[],unsigned long* Ti,
unsigned long* offi, unsigned long* sensi,float* pressure , float* ctemp, float* fTemp){
   
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
*Ti = (dT * dT) / (pow(2,31));
*offi = 5 * ((pow((temp - 2000), 2))) / 2;
*sensi = *offi / 2; 
if(temp < - 1500)
{
*offi = *offi + 7 * ((pow((temp + 1500), 2))); 
*sensi = *sensi + 11 * ((pow((temp + 1500), 2)));
}
}
else if(temp >= 2000)
{
*Ti = 0;
*offi = 0;
*sensi = 0;
}
// Adjust temp, off, sens based on 2nd order compensation 
temp -= *Ti;
off -= *offi;
sens -= *sensi;
// Convert the final data
ptemp = (((ptemp * sens) / 2097152) - off);
ptemp /= 32768.0;


*pressure = ptemp / 100.0;
*ctemp = temp / 100.0;
*fTemp = *ctemp * 1.8 + 32.0;
  
}
  


// displays MS5637's data
void displayMS5637data(float pressure, float ctemp, float fTemp){
  // Output data to serial monitor
Serial.print("Temperature in Celsius : ");
Serial.print(ctemp);
Serial.println(" C");
Serial.print("Temperature in Fahrenheit : ");
Serial.print(fTemp);
Serial.println(" F");
Serial.print("Pressure : ");
Serial.print(pressure);
Serial.println(" mbar"); 
delay(500); 
  }


//initialisation du MS5637

void initMS5637(unsigned long Coff[],unsigned int data[] ){
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
