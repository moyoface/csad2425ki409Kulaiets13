void setup() {
  Serial.begin(9600);
}

void loop() {
  if (Serial.available() > 0) { 
    String receivedMessage = Serial.readStringUntil('\n');
    String modifiedMessage = "Server: " + receivedMessage;
    Serial.println(modifiedMessage);
  }
}