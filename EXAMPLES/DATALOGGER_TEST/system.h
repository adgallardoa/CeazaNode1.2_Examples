#include <SPI.h>
#include "WinbondW25N.h"

RTC_DATA_ATTR bool hallSensorFlag = false;

String fields[30];
unsigned long flash_next_line;
unsigned long FLASH_MAX_LINES=400000;


//=============================================================================
// LOW LEVEL FUNCTIONS
//=============================================================================

void IRAM_ATTR ISR() {
  hallSensorFlag = true; 
}

void butonInit(void){
  const int HALL_SENS_PIN = 33;
  pinMode(HALL_SENS_PIN, INPUT);
  attachInterrupt(HALL_SENS_PIN , ISR, FALLING);
}


void spiPowerStatus(bool powerStatus){
  const int V_SPI_EN_PIN =   13;
  if(powerStatus){
    pinMode(V_SPI_EN_PIN,OUTPUT);
    digitalWrite(V_SPI_EN_PIN, LOW);
  }else{
    pinMode(V_SPI_EN_PIN,INPUT_PULLUP);
  }
}

void gnssPowerStatus(bool powerStatus){
  const int V_GNSS_CRTL_PIN = 2;
  if(powerStatus){
    pinMode(V_GNSS_CRTL_PIN,OUTPUT);
    digitalWrite(V_GNSS_CRTL_PIN,LOW);
  }else{
    pinMode(V_GNSS_CRTL_PIN,INPUT_PULLUP);
  }
}

void vinPowerStatus(bool powerStatus){
  const int SWVIN_CTRL_PIN = 5;
  gpio_hold_dis((gpio_num_t)SWVIN_CTRL_PIN);
  pinMode(SWVIN_CTRL_PIN, OUTPUT);
  if(powerStatus){
    digitalWrite(SWVIN_CTRL_PIN, HIGH);
  }else{
    digitalWrite(SWVIN_CTRL_PIN, LOW);
  }
}

// LED DRIVER
void setLed(bool ledSatuts){
  const int STATUS_LED_PIN  = 0;
  if(ledSatuts){
    pinMode(STATUS_LED_PIN,OUTPUT);
    digitalWrite(STATUS_LED_PIN,LOW);
  }else{
    pinMode(STATUS_LED_PIN,INPUT_PULLUP);
  }
}

// BUZZER DRIVER
void buzzerBeep(int beepQty, int pulseTrainTime_ms){
  const int BUZZER_EN_PIN = 32;
  pinMode(BUZZER_EN_PIN,OUTPUT);
  for(int i = 0; i < (beepQty * 2); i++){
    digitalWrite(BUZZER_EN_PIN, !digitalRead(BUZZER_EN_PIN));
    delay(pulseTrainTime_ms);
  }
  digitalWrite(BUZZER_EN_PIN, LOW);
}


// VIN MONITOR DRIVER
float getVin(void){
  // Monitor calibration parameters definition
  // Calibration funtion: Vin = 1/VmonitorDiv * ADC + VmonitorOffSet
  const int V_MONITOR_PIN     = 34;
  const float VmonitorDiv     = 151.5;    // set to 1 for calibration
  const float VmonitorOffSet  = 1.4;      // set to 0 for calibration
  int adcRead = analogRead(V_MONITOR_PIN);
  float vin = float(adcRead)/VmonitorDiv + VmonitorOffSet;
  return vin;
}

// PCB TEMPERATURE DRIVER
float getPcbTemp(void){
  const int TERMISTOR_PIN   = 35;
  spiPowerStatus(true);
  delay(100);

  const float rAux  = 10000;
  const float vcc   = 3.3;
  const float beta  = 3380;
  const float T0    = 298;
  const float R0    = 10000;
  
  float variableAux=(vcc/4095)*analogRead(TERMISTOR_PIN);  // get v at ADC
  variableAux = rAux/((vcc/variableAux)-1.);               // get actual thermistor resistance
  variableAux = beta/(log(variableAux/R0)+(beta/T0));     // get thermistor temperature in *K
  variableAux = variableAux - 273.;                       // convert temperature to *C
  
  return variableAux;
}


void GnssComBegin(void){
  #define SerialGPS   Serial1
  const int TXD1_PIN  = 14;
  const int RXD1_PIN  = 39;
  SerialGPS.begin(9600,SERIAL_8N1,RXD1_PIN,TXD1_PIN);
}






//=============================================================================
// HIGH LEVEL FUNCTIONS
//=============================================================================






// GNSS module NMEA buffer
String readNMEASerialData()
{
  #define SerialGPS   Serial1
  
  unsigned long startTime = millis();
  String inData = "";

  if (SerialGPS.available())
  {
    while (millis() - startTime < 250)
    {
      if (SerialGPS.available())
      {
        char c = SerialGPS.read();
        if (c == '\n')
        {
          break;
        }
        inData += c;
      }
    }
  }
  return inData;
}



// NMEA data gathering
bool extractNMEAData(String response, String* dataArray, int dataCount)
{
  // Check that the response starts with a '$' and ends with a '*'
  if (response.charAt(0) != '$' || response.indexOf('*') == -1 || response.length() < 6)
  {
    return false;
  }
  
  // Extract the expected checksum value from the response
  int expectedChecksum = strtol(response.substring(response.indexOf('*') + 1).c_str(), NULL, 16);
  
  // Remove the leading '$' and trailing checksum from the response
  response = response.substring(1, response.indexOf('*'));
  
  // Calculate the checksum of the response
  int actualChecksum = 0;
  
  for (int i = 0; i < response.length(); i++)
  {
    actualChecksum ^= response.charAt(i);
  }
  
  // Check that the calculated checksum matches the expected checksum
  if (actualChecksum != expectedChecksum)
  {
    Serial.println("checksum fail!");
    return false;
  }
  
  // Split the response into an array of data fields
  for(int i=0;i<30;i++){
    fields[dataCount] = "";
  }
  int fieldCount = 0;
  int startIndex = 0;
  int endIndex = response.indexOf(',');
  
  while (endIndex != -1 && fieldCount < dataCount)
  {
    fields[fieldCount++] = response.substring(startIndex, endIndex);
    startIndex = endIndex + 1;
    endIndex = response.indexOf(',', startIndex);
  }
  
  // If there are still fields remaining, add the last one
  if (startIndex < response.length() && fieldCount < dataCount)
  {
    fields[fieldCount++] = response.substring(startIndex);
  }
  
  // Copy the data fields to the output array
  for (int i = 0; i < dataCount && i < fieldCount; i++)
  {
    dataArray[i] = fields[i];
  }
  
  
  return true;
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


// Flash functions
W25N flash = W25N();




 void flash_get256(uint32_t linea, char *buffer256)
{
  if (linea<0) linea=0;
  uint32_t pagina = uint32_t(linea/4);
  uint16_t offset=uint16_t(linea%4)*256;
  flash.pageDataRead(pagina);
  flash.read(offset, buffer256, 256);
}


unsigned long flash_getnextline()
{
  char buf[256];
  memset((void *)&buf[0], 0, sizeof(buf));

  char buf1[256];char buf2[256];

  unsigned long indice1=0;
  unsigned long indice2=1;
  unsigned long last_known_line=0;

  for (unsigned int k=0;k<FLASH_MAX_LINES;k+=1000)
  {
   flash_get256(k,&buf1[0]);
   flash_get256(k+1000,&buf2[0]);  
   if(buf1[0] != 0xff && buf2[0] == 0xff) {last_known_line=k;break;}
  }
  
  for (unsigned int k=last_known_line;k<FLASH_MAX_LINES;k+=100)
  {
   flash_get256(k,&buf1[0]);
   flash_get256(k+100,&buf2[0]);  
   if(buf1[0] != 0xff && buf2[0] == 0xff) {last_known_line=k;break;}
  }  
  for (int k=last_known_line;k<FLASH_MAX_LINES;k++)
  {
   flash_get256(k,&buf1[0]);
   if(buf1[0] == 0xff) {return k;}
  }
  return 0;
 }//fin flash_getnextpage


void zeroBuffer(char* buf, uint32_t buflen){
  for(int i = 0; i < buflen; i++){
    buf[i] = 0;
  }
}


void retriveData(void){
  char buf[256];
  
  // ask for flash basic information as a litle communication test.
  char jedec[5] = {W25N_JEDEC_ID, 0x00, 0x00, 0x00, 0x00};
  flash.sendData(jedec, sizeof(jedec));
  Serial.println("\nData Retrieval");
  Serial.println("===============");
  Serial.print("Flash MAN_ID:");Serial.println(jedec[2],HEX); 
  Serial.print("Flash DEV_ID:");Serial.print(jedec[3],HEX);Serial.println(jedec[4],HEX);
  Serial.println("\nLog:");
  // print data saved in flash until it find empty space
  for(long pageIndex = 0; pageIndex < 65000; pageIndex++){
    zeroBuffer(buf, sizeof(buf));
    flash.pageDataRead(pageIndex);
    flash.read(0, buf, sizeof(buf));
    if(buf[10] != 0xff){
      Serial.print(String(pageIndex) + ",");Serial.println(buf);
    }else{
      break;
    }
    
  }
  Serial.println("End.");
}


void flashInit(void){
  spiPowerStatus(true);
  const int FLASH_SS_PIN    =   15;
  const int LORA_SS_PIN     =   12;
  pinMode(LORA_SS_PIN,OUTPUT);digitalWrite(LORA_SS_PIN,HIGH); // Lora disable
  if(flash.begin(FLASH_SS_PIN) != 0){
    Serial.println("Flash init Failed. Program stop!.");
    while(1); // do nothing.
  }
  flash_next_line = flash_getnextline();


  if(hallSensorFlag) {
    hallSensorFlag = false;
    retriveData();
  }
}



void flash_format()
{
  Serial.print("Formating flash...");
 flash.bulkErase();
 Serial.println("OK");
}


void flash_write256(unsigned long linea, char *buffer256)//solo pide el ID de dispositivo para comprobar que el chip esta
{
  char auxmem[256];
  uint32_t pagina = uint32_t(linea/4);
  uint16_t offset=uint16_t(linea%4)*256;
  flash_get256(linea, &auxmem[0]);//al parecer leerla antes de escribirla evita la corrupcion de memoria (2024-02-12)
  Serial.print("Flash_write256[");Serial.print(linea);Serial.print("]<-");Serial.println(buffer256);
  memcpy( &auxmem[0], &buffer256[0], 256 );//<-la transferencia SPI vacia el buffer asi que se crea un temporal
  flash.loadRandProgData(offset, &auxmem[0], 256); //W25N::loadProgData    (uint16_t columnAdd, char* buf, uint32_t dataLen, uint32_t pageAdd)
  flash.ProgramExecute(pagina);
  if (flash.check_WIP()) {delay(15);}//si todavia esta escribiendo le da 15ms para terminar
  delay(10);//<--importante
  flash_next_line=flash_next_line+1;
}

void flash_write_srt(unsigned long line, String line_srt)
{
  char buf[256];
  memset((void *)&buf[0], 0, sizeof(buf));
  line_srt.toCharArray(buf, line_srt.length()+1);
  flash_write256(line, &buf[0]);
}

void logString(String toSave){
  flash_write_srt(flash_next_line,toSave);
}







void lowPoweSleepMs(unsigned long sleepMs){
  unsigned long stamp = millis();
  if(!digitalRead(33)) flash_format();
  const int SW_VIN_PIN = 5;
  const unsigned long mS_TO_S_FACTOR = 1000;
  spiPowerStatus(false);

  gpio_hold_en((gpio_num_t)SW_VIN_PIN); // GPIO5 debe quedar en hold durante el deep sleep
  gpio_deep_sleep_hold_en();

  stamp = millis() - stamp;
  if(stamp < sleepMs) sleepMs = sleepMs - stamp;

  esp_sleep_enable_timer_wakeup(sleepMs * mS_TO_S_FACTOR); 
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_33, 0);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);

  Serial.flush();
  esp_deep_sleep_start();

  spiPowerStatus(true);
}

String getGnssDateTime(void){
  bool searchLock = true;
  bool searchDateTime = false;
  bool fillResponse = false;
  
  String selectedData = "";
  GnssComBegin();
  gnssPowerStatus(true);
  delay(100);

  while(searchLock){
    String data = readNMEASerialData();
    
    if (data.length() > 0)
    {
      const int dataCount = 20;
      String dataArray[dataCount];
      
      if(extractNMEAData(data, dataArray, dataCount))
      {
        if(dataArray[0] == "GNGGA"){
          selectedData = dataArray[1];
          if(selectedData == ""){
            if(!searchDateTime)Serial.println("Searching date/time from GPS...");
            searchDateTime = true;
          }
          if(searchDateTime){
            int satInView = dataArray[7].toInt();
            if(satInView >= 10) searchDateTime = false;
          }else if(selectedData != ""){
            fillResponse = true;
          }  
        }
        if(fillResponse && dataArray[0] == "GNZDA"){
          searchLock = false;
          selectedData = dataArray[4] + "-";
          selectedData += dataArray[3] + "-";
          selectedData += dataArray[2] + "T";
          selectedData += dataArray[1].substring(0, 2) + ":";
          selectedData += dataArray[1].substring(2, 4) + ":";
          selectedData += dataArray[1].substring(4, 6) + "Z";
        }
      }
    }
  }
  gnssPowerStatus(false);
  return selectedData;
}



void wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : retriveData(); break;
  }
}


void systemInit(void){
  spiPowerStatus(true);
  gnssPowerStatus(false);
  vinPowerStatus(false);
  flashInit();
  butonInit();
  wakeup_reason();
  
}
    
