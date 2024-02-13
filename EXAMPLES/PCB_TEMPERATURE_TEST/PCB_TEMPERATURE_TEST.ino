/*
 * PCB Temperature Test
 * 
 * This code shows an example of PCB temperature measurement by a 10Kohm 
 * thermistor.
 * If you need to have everytime VSPI on, should consider the thermistor self heating effect.
 * 
 * You can use and modify this code under your own responsibility.
 * 
 * created 09 feb. 2024
 * by Adrian Gallardo / Cristian Orrego
 * CEAZA
 */
const int V_SPI_EN_PIN = 13;
const int TERMISTOR_PIN = 35;

void setup() {
  delay(100);
  
  Serial.begin(115200);
  while(!Serial){}

  Serial.println("\nPCB temperature Test!");

  pinMode(V_SPI_EN_PIN,OUTPUT);digitalWrite(V_SPI_EN_PIN,HIGH);

}

void loop() {
  float pcbTemp;
  
  pcbTemp = getPcbTemp();
  Serial.println("PCB Temp: " + String(pcbTemp) + " *C");

  delay(100);
}

float getPcbTemp(void){
  digitalWrite(V_SPI_EN_PIN, LOW);
  delay(1000);

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
