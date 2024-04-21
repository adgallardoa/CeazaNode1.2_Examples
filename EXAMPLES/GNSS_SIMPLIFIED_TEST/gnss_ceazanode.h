
#define SerialGPS Serial1



/*


//================================================================================================================
// ESTRUCTURA DE DATOS EN EEPROM
struct SCALE_CONFIG
{  
   bool     default_config;   // booleano que indica si se utiliza la configuración por defecto o no
   uint8_t  rs485_id;         // ID del sensor en red local RS-485
   int      baud_factor;      // Multiplicador para el cálculo de comunicacion serial
   float    prop[4];
   float    offset[4];
   char     sensor_name[nameNumChars];   // Nombre que se asignará al sensor en redes virtuales
   int      masa_smpl_avg;    // Cantidad de promedio de mediciones para calcular 
   float    tara;
   float    cal_p;
   float    cal_o;
   int      transmitDelay;
};

SCALE_CONFIG sensorConfig;

*/



/*
struct GNSS_DATA
{
  int   gnssStatus;
  int   antennaStatus;
  
};
*/


class GNSSmodule{
  public:

    
}


String fields[30];


void GnssComBegin(void){
  const int TXD1_PIN  = 14;
  const int RXD1_PIN  = 39;
  SerialGPS.begin(9600,SERIAL_8N1,RXD1_PIN,TXD1_PIN);
}


void setGnssPower(bool gnssPowerStatus){
  const int V_GNSS_CRTL_PIN = 2;
  if(gnssPowerStatus){
    pinMode(V_GNSS_CRTL_PIN,OUTPUT);
    digitalWrite(V_GNSS_CRTL_PIN,LOW);
  }else{
    pinMode(V_GNSS_CRTL_PIN,INPUT_PULLUP);
  }
}

String readNMEASerialData()
{
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






bool gnssUpdate(void){
  bool answer = false;
  String data = readNMEASerialData();
  if (data.length() > 0){
    const int dataCount = 20;
    String dataArray[dataCount];
    if(extractNMEAData(data, dataArray, dataCount)){
      if(dataArray[0] == "GPTXT"){
        answer = true;
        if(fields[4] == "ANTENNA OK"){
          
        }else if(fields[4] == "ANTENNA OPEN"){
          
        }else if(fields[4] == "ANTENNA SHORT"){
          
        }else{
          // error
        }    
      }else if(dataArray[0] == "GNGGA"){
        answer = true;
        // UTC time
        dataArray[1];
        // GPS status
        if(dataArray[6] == "0"){  
          //not positioned
          
        }else if(dataArray[6] == "1"){
          //single point positioning
          
        }else if(dataArray[6] == "2"){
          //differential GPS fixed solution
          
        }else if(dataArray[6] == "4"){
          //RTK Fixed
          
        }else if(dataArray[6] == "5"){
          //floating point solution

        }else if(dataArray[6] == "6"){
          //INS Dead reckoning

        }else{
          //Not valid value!  
        }
        //Satellites used dataArray[7]
      }
    }
  }
  return answer;
}
