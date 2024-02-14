/*
 * Calypso ULP SD Windmeter
 * Wires
 * - White:   Ground
 * - Brown:   Power Vcc
 * - Yellow:  B-
 * - Green:   A+
 * 
 * Its highly recommended connect the sensor power wire to Vcc intead to SWVIN in order to have access to 
 * moving average measurements calculates on runtime by the sensor itself.
 * 
 * Warning!: the system power supply must be < 18Vdc in order to avoid sensor damaged.
 * 
 * 
 * You can use and modify this code under your own responsibility.
 * 
 * created 14 Feb. 2024
 * by Adrian Gallardo / Cristian Orrego
 * CEAZA
 */


// Define modbus sensor constants
const int START_ADDR = 0x00C9;          // Starting address of holding registers to read
const int NUM_REGS = 13;                // Number of holding registers to read
const unsigned int MODBUS_BAUD = 9600;  // Must be different of other protocolsd used in the same channel

uint16_t windSensorData[12]; // {WS_3s, WD_3s, WS_2min, WD_2min, WS_10m, WD_10min, WGS, WGD, WS_5min, WD_5min, WGS_5min,WGS_5min} all data is multiplied x10
const String varName[12] = {"WS_3s", "WD_3s", "WS_2min", "WD_2min", "WS_10m", "WD_10min", "WGS", "WGD", "WS_5min", "WD_5min", "WGS_5min", "WGS_5min"};

#include "microModbus.h"

//***********************************************************************************
void wssDataUpdate(int slaveId, unsigned long timeOut){  
  digitalWrite(STATUS_LED_PIN,LOW);
  digitalWrite(VSPI_EN_PIN,LOW);
  
  const int bufferLength = 64;
  byte modbusInBuffer[bufferLength];
  unsigned long startPoint;
  int qtyInByte = 0;

  // Define query buffer
  unsigned char query[8];

  // Generate Modbus RTU query
  generateModbusRTUQuery03(query, slaveId, START_ADDR, NUM_REGS);
  
  // Clear in buffer array
  for(int i = 0; i < bufferLength ; i++){
    modbusInBuffer[i] = 0;
  }

  // Clear UART in buffer
  delay(20);
  while(Serial2.available()){
    byte inByte = Serial2.read();
  }

  // Send modbus query
  Serial2.write(query,8);
  Serial2.flush();


  // Wait for any answer
  startPoint = millis();
  while((millis() - startPoint < timeOut)){
    if(Serial2.available()){
      byte inByte = Serial2.read();
      modbusInBuffer[qtyInByte] = inByte;
      qtyInByte++;
    }
  }

  
  if(qtyInByte == 0) Serial.println("NO DATA");
  digitalWrite(VSPI_EN_PIN,HIGH);
  
  for(int i = 0; i < 12; i++){
    windSensorData[i] = 0;
  }
  int dataCount = 0;
  for(int i = 3; i < 28 ; i = i+2){
    if(i != 19){
      windSensorData[dataCount] = (uint16_t(modbusInBuffer[i]) << 8) + uint16_t(modbusInBuffer[i+1]);
      dataCount++;
    }
  }
  
  digitalWrite(STATUS_LED_PIN,HIGH);
}
