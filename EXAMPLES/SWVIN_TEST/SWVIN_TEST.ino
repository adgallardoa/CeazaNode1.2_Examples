#define SWVIN_CTRL_PIN  5

int cont = 0;


void setup() {
  delay(100); // To avoid CPU crash during init

  Serial.begin(115200);

  pinMode(SWVIN_CTRL_PIN, OUTPUT); digitalWrite(SWVIN_CTRL_PIN, LOW); // turn-on SwVin
  
}

void loop() {
  
  digitalWrite(SWVIN_CTRL_PIN, !digitalRead(SWVIN_CTRL_PIN));
  Serial.println(String (cont) + ", SWVIN: " + String(digitalRead(SWVIN_CTRL_PIN)));
  
  cont ++;
  delay(5000);
}
