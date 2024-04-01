/*
 * BUZZER TEST
 * 
 * This code shows how to use the built in 3 KHz buzer.
 * HW Version: 1.2
 * microcontroller: ESP32-WROOM-32E or "ESP32 Dev Module"
 * 
 * You can use and modify this code under your own responsibility.
 * 
 * created 7 Feb. 2024
 * by Adrian Gallardo / Cristian Orrego
 * CEAZA
 * 
 */
 // used MCU pin names
const int BUZZER_EN_PIN = 32;


void setup() {
  delay(100);   // to avoid boot problems with ESP32

  // define pin modes and initial status of each one
  pinMode(BUZZER_EN_PIN,OUTPUT);digitalWrite(BUZZER_EN_PIN,LOW);    // define buzzer controller pin with initia stats off.

  // sound routine. on this case, it sounds 2 times
  for(int i = 0; i < 4; i++){
    digitalWrite(BUZZER_EN_PIN, !digitalRead(BUZZER_EN_PIN)); // the buzzer will sound while BUZZER_EN_PIN have a high level output 
    delay(100);
  }
  digitalWrite(BUZZER_EN_PIN,LOW);    // turn off the buzzer
  
}

void loop() {


}
