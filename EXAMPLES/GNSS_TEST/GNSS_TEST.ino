/*
 * GNSS module Test
 * 
 * This code shows an example of interaction at low level with the GNSS module
 * embedded to the system.
 * To test this code is very important the device have clear view to sky to allows the module 
 * receive data from GPS / Beidou networks.
 * 
 * You can use and modify this code under your own responsibility.
 * 
 * created 30 Jan 2024
 * by Adrian Gallardo / Cristian Orrego
 * CEAZA
 */


// define MCU pins
const int TXD1_PIN        = 14;
const int RXD1_PIN        = 39;
const int V_GNSS_CRTL_PIN = 2;

#define SerialGPS   Serial1

String fields[30];

void setup() {
  delay(100);
  
  Serial.begin(115200);
  while (!Serial){}

  Serial.println("\nGNSS module test");

  // begin serial used to communicate with GNSS module
  SerialGPS.begin(9600,SERIAL_8N1,RXD1_PIN,TXD1_PIN);

  // set pins and its initial status
  pinMode(V_GNSS_CRTL_PIN,OUTPUT);digitalWrite(V_GNSS_CRTL_PIN,LOW); // Turn-on GNSS receiver
}




void loop() {
  String data = readNMEASerialData();

  if (data.length() > 0)
    {
      const int dataCount = 20;
      String dataArray[dataCount];
      
      if(extractNMEAData(data, dataArray, dataCount))
      {

        
       
        if(dataArray[0] == "GPTXT"){
          Serial.print("- GNSS antenna: ");
          if(fields[4] == "ANTENNA OK"){
            Serial.println("OK\n");
          }else if(fields[4] == "ANTENNA OPEN"){
            Serial.println("OPEN\n");
          }else if(fields[4] == "ANTENNA SHORT"){
            Serial.println("SHORTED\n");
          }else{
            Serial.println("COM. ERROR\n");
          }
          
        }else if(dataArray[0] == "GNGGA"){
          Serial.print("- UTC time: ");
          Serial.println(dataArray[1]);
          Serial.print("- GPS status: ");
          if(dataArray[6] == "0"){
            Serial.println("not positioned");
          }else if(dataArray[6] == "1"){
            Serial.println("single point positioning");
          }else if(dataArray[6] == "2"){
            Serial.println("differential GPS fixed solution");
          }else if(dataArray[6] == "4"){
            Serial.println("RTK Fixed");
          }else if(dataArray[6] == "5"){
            Serial.println("floating point solution");
          }else if(dataArray[6] == "6"){
            Serial.println("INS Dead reckoning");
          }else{
            Serial.println("Not valid value! \"" + String(dataArray[6]) + '\"');
          } //INS Dead reckoning
          Serial.print("- Satellites used: ");
          Serial.println(dataArray[7]);
        }
      }

    }


}





//=================================================================================================
//  READ SWARM MODULE MESSAGES
//=================================================================================================
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




//=================================================================================================
//  GET MESSAGE FROM INCOMMING NMEA MODULE RESPONSE AND VALIDATE IT'S INTEGRITY
//=================================================================================================
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
