/*
 * VIN MONITOR TEST
 * 
 * This code shows how to read input voltage.
 * HW Version: 1.2
 * microcontroller: ESP32-WROOM-32E or "ESP32 Dev Module"
 * 
 * You can use and modify this code under your own responsibility.
 * 
 * created 09 Feb. 2024
 * by Adrian Gallardo / Cristian Orrego
 * CEAZA
 * 
 */
const int V_MONITOR_PIN = 34;

// Monitor calibration parameters definition
// Calibration funtion: Vin = 1/VmonitorDiv * ADC + VmonitorOffSet
const float VmonitorDiv     = 151.5;    // set to 1 for calibration
const float VmonitorOffSet  = 1.4;      // set to 0 for calibration

void setup() {
  delay(100); // to avoid MCU crash

  Serial.begin(115200);
  while (!Serial){}

  Serial.println("\nVin monitor test");

}

void loop() {
  float vinMonitor = getVin();

  Serial.println("Vin: " + String(vinMonitor) + " Volts");
  delay(1000);

}


// this function calculate vin in Volts from ADC values on V_MONITOR_PIN
float getVin(void){
  int adcRead = analogRead(V_MONITOR_PIN);
  float vin = float(adcRead)/VmonitorDiv + VmonitorOffSet;
  return vin;
}
