/*
 * LoRa CHAT
 * -------------
 * 
 * This example shows you how the LoRa bus works on CEAZAnode V1.2
 * You should asign an onike name to identify your messages on the network.
 * Remember dont send messages meanwahile other useres or devices ar sendind data.
 * 
 * WARNING!: When you send any data by LoRa, anyone around who can receive lora packets will be able 
 * to read your messages. This sketch does not encode transmited data. 
 * Also remember that you are using a free band and it is not recommended to monopolize it.
 * 
 * You can use and modify this code under your own risk
 * Adrian Gallardo / Cristian Orrego
 * CEAZA
 * 03.04.2024
 * 
 */


#include <SPI.h>
#include <LoRa.h>

const String userName = "Myself";   // write you chat name here between " "
String outMsg = "";

const int V_SPI_EN_PIN    = 13;
const int FLASH_SS_PIN    = 15;
const int LORA_SS_PIN     = 12;
const int LORA_RESET_PIN  = 4;
const int LORA_DIO0_PIN   = 36;
const int V_GNSS_CRTL_PIN = 2;


#define LORA_FREC       915E6 //(`433E6`, `866E6`, `915E6`)
#define LORA_SF         10     //6 a 12
#define LORA_TX_POW     18    //2 a 20
#define LORA_BANDWIDTH  125E3 //`7.8E3`, `10.4E3`, `15.6E3`, `20.8E3`, `31.25E3`, `41.7E3`, `62.5E3`, `125E3`, and `250E3`.



void setup() {
  delay(100);
  
  Serial.begin(115200);
  while(!Serial);

  Serial.println("\n\nCEAZA IOT NODE LoRa CHAT TEST!");
  Serial.println("---------------------------------\n");

  // Init MCU pins
  pinMode(LORA_SS_PIN,OUTPUT);digitalWrite(LORA_SS_PIN,HIGH); 
  pinMode(FLASH_SS_PIN,OUTPUT);digitalWrite(FLASH_SS_PIN,HIGH); // Flash memory will be disable for this example
  pinMode(V_SPI_EN_PIN,OUTPUT);digitalWrite(V_SPI_EN_PIN,LOW);  // Turn on SPI devices (flash memory and LoRa module)
  pinMode(V_GNSS_CRTL_PIN,INPUT_PULLUP);                        // Turn off gnss receiver to save energy

  // Init Lora module
  Serial.print("LoRa Module Init: ");
  LoRa.setPins(LORA_SS_PIN, LORA_RESET_PIN, LORA_DIO0_PIN);
  if (!LoRa.begin(LORA_FREC)) {    
    Serial.print("N");
  }
  Serial.println("OK!\n");
  LoRa.setTxPower(LORA_TX_POW);
  LoRa.setSpreadingFactor(LORA_SF);
  LoRa.setSignalBandwidth(LORA_BANDWIDTH);


  Serial.println("chat with anyone connected to this LoRa network!");
  Serial.println("remember use \"Both NL & CR\" as end of message.");
  Serial.println("Only one device can transmit at time.\n");

}

void loop() {
  // detect if you're writing a message
  if(Serial.available()){
    char inChar = Serial.read();
    if(inChar == '\n'){
      // send packet
      LoRa.beginPacket();
      LoRa.print(userName + " say: " + outMsg);
      LoRa.endPacket();
      
      Serial.println("->" + outMsg);
      outMsg = "";
    }else{
      outMsg += inChar;
    }
  }


  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    Serial.print("Received packet '");

    // read packet
    while (LoRa.available()) {
      Serial.print((char)LoRa.read());
    }

    // print RSSI of packet
    Serial.print(", with RSSI ");
    Serial.println(LoRa.packetRssi());
  }

}
