/*
 * Calypso ULP SD Windmeter by modbus.
 * Instrument webpage: https://calypsoinstruments.com/shop/product/ultra-low-power-ultrasonic-wind-meter-ulp-std-6#attr=76,56,65
 * Ensure to properly configurate the sensor with modbus RTU at 9600baud and slave ID 3.
 * 
 * You can use and modify this code under your own responsibility.
 * 
 * created 14 Feb. 2024
 * by Adrian Gallardo / Cristian Orrego
 * CEAZA
 */

const int VSPI_EN_PIN = 13;
const int STATUS_LED_PIN  = 0;

const int ULP_ID = 3;
const int MODBUS_TIMEOUT_MS = 100;

#include "calypsoULPSDwindMeter.h"

#define Serial485 Serial2



void setup() {
  delay(100);
  
  Serial.begin(115200);
  while (!Serial){}
  Serial.println("Calypso Instruments all data query test by Modbus RTU\nSending Query...\n");

  Serial485.begin(9600);
  while (!Serial485){}

  pinMode(VSPI_EN_PIN,OUTPUT);digitalWrite(VSPI_EN_PIN,HIGH);
  pinMode(STATUS_LED_PIN,OUTPUT);digitalWrite(STATUS_LED_PIN,HIGH);
}



void loop() {
  wssDataUpdate(ULP_ID,MODBUS_TIMEOUT_MS);
  Serial.println("\nSENSOR ID " + String(ULP_ID));
  for(int i = 0; i < 12; i++){
     Serial.println("Data " + String(i) + " rcvd ("+ varName[i] + "): " + String(windSensorData[i]));
  }

  Serial.println("");
  delay(1000);
}
