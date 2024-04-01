/*
 * LED and BUZZER TEST
 * 
 * This code shows how to use the built in 3 KHz buzer.
 * HW Version: 1.2
 * microcontroller: ESP32-WROOM-32E or "ESP32 Dev Module"
 * 
 * You can use and modify this code under your own responsibility.
 * 
 * created 1 apr. 2024
 * by Adrian Gallardo / Cristian Orrego
 * CEAZA
 * 
 */




//============================================================================================================
// Program Functions.
//*********************************************************

 // used MCU pin names
const int BUZZER_EN_PIN = 32;
const int STATUS_LED_PIN  = 0;


// This function drive the embedded LED at high efficiency way
void setLed(bool ledSatuts){
  if(ledSatuts){
    pinMode(STATUS_LED_PIN,OUTPUT);
    digitalWrite(STATUS_LED_PIN,LOW);
  }else{
    pinMode(STATUS_LED_PIN,INPUT_PULLUP);
  }
}

// This function controls an LED train pulse
void pulseLed(int pulseQty, int pulseTrainTime_ms){
  bool statusLED = false;
  for(int i = 0; i < (pulseQty * 2); i++){
    statusLED = !statusLED;
    setLed(statusLED);   
    delay(pulseTrainTime_ms);
  }
  setLed(false);
}


// This function drive the embedded buzzer
void buzzerBeep(int beepQty, int pulseTrainTime_ms){
  pinMode(BUZZER_EN_PIN,OUTPUT);
  for(int i = 0; i < (beepQty * 2); i++){
    digitalWrite(BUZZER_EN_PIN, !digitalRead(BUZZER_EN_PIN));
    delay(pulseTrainTime_ms);
  }
  digitalWrite(BUZZER_EN_PIN, LOW);
}


//============================================================================================================




//*********************************************************
// This code runs one time at begining of program.
//*********************************************************
void setup() {
  delay(100);   // to avoid boot problems with ESP32

  buzzerBeep(2,100);
}


//*********************************************************
// This code runs repeatedily.
//*********************************************************
void loop() {
  pulseLed(2,100);
  
  delay(500);
}
