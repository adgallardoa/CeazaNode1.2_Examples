/*
 * VIN / TPCB MONITOR TEST
 * 
 * This code shows how to read input voltage and PCB temperature.
 * HW Version: 1.2
 * microcontroller: ESP32-WROOM-32E or "ESP32 Dev Module"
 * 
 * You can use and modify this code under your own responsibility.
 * 
 * created 01 Apr. 2024
 * by Adrian Gallardo / Cristian Orrego
 * CEAZA
 * 
 */

//=====================================================================================================================================

// used MCU pin names
const int V_MONITOR_PIN   = 34;
const int BUZZER_EN_PIN   = 32;
const int STATUS_LED_PIN  = 0;
const int V_SPI_EN_PIN    = 13;
const int TERMISTOR_PIN   = 35;

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

// this function calculate vin in Volts from ADC values on V_MONITOR_PIN
float getVin(void){
  // Monitor calibration parameters definition
  // Calibration funtion: Vin = 1/VmonitorDiv * ADC + VmonitorOffSet
  const float VmonitorDiv     = 151.5;    // set to 1 for calibration
  const float VmonitorOffSet  = 1.4;      // set to 0 for calibration
  int adcRead = analogRead(V_MONITOR_PIN);
  float vin = float(adcRead)/VmonitorDiv + VmonitorOffSet;
  return vin;
}

// This function reads and calculate the pcb temperature.
float getPcbTemp(void){
  pinMode(V_SPI_EN_PIN,OUTPUT);
  digitalWrite(V_SPI_EN_PIN, LOW);
  delay(500);

  const float rAux  = 10000;
  const float vcc   = 3.3;
  const float beta  = 3380;
  const float T0    = 298;
  const float R0    = 10000;
  
  float variableAux=(vcc/4095)*analogRead(TERMISTOR_PIN);  // get v at ADC
  variableAux = rAux/((vcc/variableAux)-1.);               // get actual thermistor resistance
  variableAux = beta/(log(variableAux/R0)+(beta/T0));     // get thermistor temperature in *K
  variableAux = variableAux - 273.;                       // convert temperature to *C
  
  digitalWrite(V_SPI_EN_PIN, HIGH);
  return variableAux;
}


//=====================================================================================================================================

unsigned int cont = 0;

void setup() {
  delay(100);

  Serial.begin(115200);
  Serial.println("\nVin - PCB Temperature Test\n");
}

void loop() {
  setLed(true);
  float temperature = getPcbTemp();
  float vin = getVin();
  setLed(false);
  cont++;

  Serial.println("Data No." + String(cont));
  Serial.println("PCB Temp: " + String(temperature) + "*C");
  Serial.println("Vin: " + String(vin) + "V\n");

  delay(2000);

}
