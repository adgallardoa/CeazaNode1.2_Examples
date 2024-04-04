/*
 * Datalogging System
 * ==================
 * 
 * This sketch shows you how to use the CEAZAnode V1.2 as
 * a complete datalogging solition
 * 
 * 
 * You can use and modify this code under your own risk.
 * by Adrian Gallardo / Cristian Orrego
 * CEAZA 
 * 03.04.2024
 * 
 */
#include "system.h"

void setup() {
  delay(100);

  Serial.begin(115200);
  while(!Serial);

  Serial.println("\nDATALOGGER TEST");
  Serial.println("================\n");

  systemInit();
}

void loop() {
  // Update date/time and measurements data--------
  String dateTime = getGnssDateTime();
  
  setLed(true);
  float vin = getVin();
  float temp = getPcbTemp();
  setLed(false);

  // Generate new data line------------------------
  String dataLine = "";
  dataLine += dateTime;
  dataLine += ",vin," + String(vin);
  dataLine += ",temp," + String(temp);

  // Save new register-----------------------------
  logString(dataLine);

  // go to sleep at low power consumption----------
  unsigned long sleepTime = 60000;
  if(millis() < sleepTime) sleepTime = sleepTime - millis();
  lowPoweSleepMs(sleepTime);   // After a low power sleep the device is rebooted
}
