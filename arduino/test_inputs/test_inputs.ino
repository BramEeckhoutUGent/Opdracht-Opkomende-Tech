// Dit is code om te controleren of de ESP32 werkt.
// Indien er geen response gegeven wordt via de Serial monitor, controleer de instellingen
void setup() {
  Serial.begin(115200);
}

void loop() {
  Serial.println("ESP32 werkt!");
  delay(1000);
}