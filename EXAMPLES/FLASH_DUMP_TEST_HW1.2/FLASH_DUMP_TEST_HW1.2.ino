/*
 * FLASH DUMP TEST
 * 
 * This code shows how to retrive data at low level from datalogger's flash memory.
 * Node HW Version: 1.2
 * microcontroller: ESP32-WROOM-32E or "ESP32 Dev Module"
 * Flash memory: Windbond W25N01GVZEIG TR
 * You can use and modify this code under your own responsibility.
 * 
 * created 30 Jan 2024
 * by Adrian Gallardo / Cristian Orrego
 * CEAZA
 * 
 */
#include <SPI.h>
#include "WinbondW25N.h"

// define MCU pins
const int FLASH_SS_PIN    =   15;
const int V_VSPI_CTRL_PIN =   13;
const int V_GNSS_CRTL_PIN =   2;
const int LORA_SS_PIN     =   12;

W25N flash = W25N();

char buf[256];

void setup() {
  delay(100); // recommended for ESP32's

  Serial.begin(115200);
  while (!Serial){}
  
  Serial.println("\nFlash dump!!");

  // set pins and its initial status
  pinMode(V_GNSS_CRTL_PIN,INPUT_PULLUP);  // GNSS receiver module off to save battery.
  pinMode(V_VSPI_CTRL_PIN,OUTPUT);digitalWrite(V_VSPI_CTRL_PIN,LOW);  // activate VSPI to turn on the flash memory, LORA module and PCB temperature thermistor.
  pinMode(LORA_SS_PIN,OUTPUT);digitalWrite(LORA_SS_PIN,HIGH);   // set LORA module disable in the SPI bus to avoid data crashing
  
  delay(10); // wait for VSPI-on stabilization and flash-on delay

  // flash init
  if(flash.begin(FLASH_SS_PIN) == 0){
    Serial.println("Flash init successful");
  } else {
    Serial.println("Flash init Failed");
    while(1); // do nothing.
  }  

  // ask for flash basic information as a litle communication test.
  char jedec[5] = {W25N_JEDEC_ID, 0x00, 0x00, 0x00, 0x00};
  flash.sendData(jedec, sizeof(jedec));
  Serial.print("MAN_ID:");Serial.println(jedec[2],HEX); 
  Serial.print("DEV_ID:");Serial.print(jedec[3],HEX);Serial.println(jedec[4],HEX);

  // print data saved in flash until it find empty space
  for(long pageIndex = 0; pageIndex < 65000; pageIndex++){
    zeroBuffer(buf, sizeof(buf));
    flash.pageDataRead(pageIndex);
    flash.read(0, buf, sizeof(buf));
    if(buf[10] != 0xff){
      Serial.print(String(pageIndex) + ",");Serial.println(buf);
    }
    
  }
  Serial.println("End.");

}

void loop() {

}



void zeroBuffer(char* buf, uint32_t buflen){
  for(int i = 0; i < buflen; i++){
    buf[i] = 0;
  }
}
