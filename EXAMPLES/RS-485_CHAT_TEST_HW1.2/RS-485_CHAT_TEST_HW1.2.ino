/*
 * RS-485 CHAT
 * -------------
 * 
 * This example shows you how the RS-485 bus works on CEAZAnode V1.2
 * You should asign an onike name to identify your messages on the network.
 * Remember dont send messages to the bus meanwahile other useres or devices ar sendind data.
 * 
 * You can use and modify this code under your own risk
 * Adrian Gallardo / Cristian Orrego
 * CEAZA
 * 03.04.2024
 * 
 */

// Give a name to pins
const int VSPI_EN_PIN  = 13;

const String userName = "Myself";   // write you chat name here between " "
String outMsg = "";

#define Serial485 Serial2


void setup() {
  delay(100);   // to avoid random ESP32 crash
  
  pinMode(VSPI_EN_PIN,OUTPUT);digitalWrite(VSPI_EN_PIN,LOW);  // Turn-on RS-485 driver

  // begin Serial monitor port at 115200 bits per second
  Serial.begin(115200);
  while(!Serial);

  // begin RS485 com port at 9600 bits per second
  Serial485.begin(9600);
  while(!Serial485);
  
  Serial.println("\n\nCEAZA IOT NODE RS-485 CHAT TEST!");
  Serial.println("---------------------------------\n");
  Serial.println("chat with anyone connected to this RS-485 network!");
  Serial.println("remember use \"Both NL & CR\" as end of message.\n");
}
  


void loop() {
  // detect if you're writing a message
  if(Serial.available()){
    char inChar = Serial.read();
    if(inChar == '\n'){
      Serial485.println(userName + " say: " + outMsg);
      Serial.println("->" + outMsg);
      outMsg = "";
    }else{
      outMsg += inChar;
    }
  }

  // show you incoming message
  if(Serial485.available()){
    String inMsg = "";
    byte inChar = Serial485.read();
    Serial.write(inChar);
  }
}
