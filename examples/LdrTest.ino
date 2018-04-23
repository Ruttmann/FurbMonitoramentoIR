int ledPin1 = 49;
int ledPin2 = 44;
int ldrPin = 0; //Pino analógico A0
int ldrValor = 0;

void setup() {
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  ldrValor = analogRead(ldrPin);
  Serial.println(ldrValor);

  //Quando LDR >= 700: pouca luz, dispositivo desligado.
  if (ldrValor >= 700) digitalWrite(ledPin1, HIGH);
  else digitalWrite(ledPin1, LOW);

  //Quando LDR <= 200: muita luz, dispositivo ligado.
  if (ldrValor <= 200) digitalWrite(ledPin2, HIGH);
  else digitalWrite(ledPin2, LOW);
  
  delay(100);
}
