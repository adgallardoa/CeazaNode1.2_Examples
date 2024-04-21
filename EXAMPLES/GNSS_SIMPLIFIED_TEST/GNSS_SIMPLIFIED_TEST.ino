
#include "gnss_ceazanode.h"

//GNSS_DATA myGnss;

void setup() {
  delay(100);

  Serial.begin(115200);
  
  GnssComBegin();
  setGnssPower(true);
}

void loop() {
  gnssUpdate();

  delay(1000);
}
