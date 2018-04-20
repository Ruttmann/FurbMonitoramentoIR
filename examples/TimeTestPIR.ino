long currentMillis = 0;
bool wentHigh = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Wait for PIR init...");
  delay(60000);
  Serial.println("PIR ready!");
  pinMode(3, INPUT); //PIR pin
  pinMode(51, OUTPUT); //LED pin
  digitalWrite(51, LOW);
}

void loop() {
  if (!wentHigh && digitalRead(3) == HIGH) {
    Serial.println("DETECTION!");
    currentMillis = millis();
    wentHigh = !wentHigh;
    digitalWrite(51, HIGH);
  }

  if (wentHigh && digitalRead(3) == LOW) {
    Serial.print("Time high: ");
    Serial.println((millis() - currentMillis) / (long)1000);
    wentHigh = !wentHigh;
    digitalWrite(51, LOW);
  }
}
