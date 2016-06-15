# first_pcb_arduino
This repository is aimed to control a pcb containing several captors (Temperature, UV, pressure, shock...) for an internship project 

The PCB contains the following captors : 

MS5637-02BA03   Low Voltage Barometric Pressure Sensor

ACS712          Current Sensor IC

MVS0608         Micro Vibration Sensor

MPL115A2        Miniature I2C Digital Barometer

TMP20AIDCKT     temperature sensor

ADXL375BCCZ     Digital MEMS Accelerometer

LIS2DE          MEMS digital output motion sensor

ML 8511         UV sensor

The aim of the code is to get and analyse the data for these sensors with an Arduino in order to deduce the sudden modifications of the outdoor environment. 

Librairies will be added to simplify the use of the captors. Currently has libraries for MPL115A2, ADXL375 and MS5637. 


First, I will simply try to calibrate and get the data from the sensors. Then, I will write a loop containing interruptions, sending messages on the serial port depending on the analysis of the signals. 
