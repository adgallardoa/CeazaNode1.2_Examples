/*
 * DEEP SLEEP TEST
 * 
 * This code shows how to put the device in sleep mode by a defined amount
 * of time at low poer consumtion mode. Which means turn off GNSS module,
 * LoRa modue, Flash memory and SWVIN.
 * HW Version: 1.2
 * microcontroller: ESP32-WROOM-32E or "ESP32 Dev Module"
 * 
 * You can use and modify this code under your own responsibility.
 * 
 * created 06 Feb 2024
 * by Adrian Gallardo / Cristian Orrego
 * CEAZA
 * 
 */

// define MCU pins
const int VINSW_EN_PIN =  5;
const int VGNSS_NOT_EN_PIN = 2;
const int VSPI_NOT_EN_PIN =  13;

// Dictionary
#define uS_TO_S_FACTOR 1000000ULL   /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  5           /* Time ESP32 will go to sleep (in seconds) */


void setup() {
  delay(100); // recommended for ESP32's

  gpio_hold_dis((gpio_num_t) VINSW_EN_PIN);

  // Serial communication begin
  Serial.begin(115200);
  while (!Serial){}     // wait until Serial its operative

  Serial.println("\nDeep sleep Low-power consumption test");

  // set power pins and its initial status
  pinMode(VINSW_EN_PIN,OUTPUT);digitalWrite(VINSW_EN_PIN,HIGH);  // Vin ON
  pinMode(VGNSS_NOT_EN_PIN,OUTPUT);digitalWrite(VGNSS_NOT_EN_PIN,LOW);   // VGNSS ON
  pinMode(VSPI_NOT_EN_PIN,INPUT_PULLUP);    // VSPI ON


  Serial.println("Going to seelp in:");
  for(int i = 5; i > 0; i--){
    Serial.println(String(i) + " seg");
  }
  

  // prepare the entire device for sleep mode
  Serial.println("Turning off internal and external loads");
  int cont = 1;
  while(digitalRead(VINSW_EN_PIN) != 0 || digitalRead(VGNSS_NOT_EN_PIN) != 1 || digitalRead(VSPI_NOT_EN_PIN) != 1)
  {
    Serial.print("Try No." + String(cont));
    cont++;
    digitalWrite(VINSW_EN_PIN,LOW);  // Vin OFF
    pinMode(VGNSS_NOT_EN_PIN,INPUT_PULLUP);   // VGNSS OFF
    pinMode(VSPI_NOT_EN_PIN,INPUT_PULLUP);    // VSPI OFF
    delay(100);
  }
  Serial.println("Done.");
  Serial.println("Going to sleep for " + String(TIME_TO_SLEEP) + " sec.");
  Serial.println("Bye!.. zzzz");
  Serial.flush();      // wait UART finish any TX before sleep
  gpio_hold_en((gpio_num_t)VINSW_EN_PIN);     //GPIO5 must be on hold during sleep mode
  gpio_deep_sleep_hold_en();    // enable pin state hold


  // go to sleep
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_deep_sleep_start();

}

void loop() {
  // This part of the code will never runs due sleep callig in Setup function
}
