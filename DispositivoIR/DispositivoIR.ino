#include <IRremote.h> 

#define PIN_LED 7 //Pino do LED de feedback ao usuário
#define PIN_TX 45 //Pino emissor
#define PIN_RX 44 //Pino receptor
#define maxLen 500 //Número máximo de pulsos a receber
#define FREQ 38 //Frequência da portadora em KHz

unsigned int sinalIR[maxLen]; //Armazena os tempos dos pulsos do sinal
unsigned int tamanhoSinal; //Armazena a quantidade de pulsos do sinal
IRsend irsend; //Objeto do emissor de IR

volatile  unsigned int irBuffer[maxLen]; //Armazena tempos - volatile pois é manipulado pelo ISR
volatile unsigned int x = 0; //Flag/contador do irBuffer - volatile pois é manipulado pelo ISR

void sendRawSignal() {
  irsend.enableIROut(FREQ); //Habilita emissor
  for(unsigned int i = 0; i < tamanhoSinal; i++) {
    if (i & 1)
      irsend.space(sinalIR[i]);
    else
      irsend.mark(sinalIR[i]);  
  }
  irsend.space(1);
}

void interruptHandlerRX() {
  if (x > maxLen) return; //Ignora se irBuffer já estiver cheio.
  irBuffer[x++] = micros(); //Grava continuamente o time-stamp das transições de sinais.
}

void setup() {
  Serial.begin(115200);
  attachInterrupt(0, interruptHandlerRX, CHANGE); //Inicia ISR para recepção de IR
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TX, INPUT);
  pinMode(PIN_RX, INPUT);
  digitalWrite(PIN_LED, LOW);
}

unsigned int btnEnviar;
unsigned int btnAprender;
unsigned long tempoEmissao;

void loop() {
  btnEnviar = digitalRead(PIN_TX);
  btnAprender = digitalRead(PIN_RX);

  if (btnAprender) {
//    attachInterrupt(0, interruptHandlerRX, CHANGE);//Inicia ISR para recepção de IR
    digitalWrite(PIN_LED, HIGH);
    delay(300);
    digitalWrite(PIN_LED, LOW);
    delay(5000); //Pausa de 5 segundos para receber o sinal completo
    if (x) { //Se algum sinal foi detectado
      digitalWrite(PIN_LED, HIGH);//Indicador visual de sinal detectado
      Serial.println();
      Serial.print(F("Raw: (")); //Reporta a quantidade de pulsos do sinal
      Serial.print((x - 1));
      Serial.println(F(") "));
      detachInterrupt(0);//Desliga a interrupção e captura de sinais
      for (int i = 1; i < x; i++) { //Persiste os pulsos no array de sinal
        sinalIR[i-1] = irBuffer[i] - irBuffer[i - 1];
        Serial.print(i);
        Serial.print(F(") "));
        Serial.println(sinalIR[i-1]);
      }
      Serial.println();
      tamanhoSinal = x - 1;
      x = 0;
      digitalWrite(PIN_LED, LOW); //Desliga indicador visual de sinal detectado
      attachInterrupt(0, interruptHandlerRX, CHANGE); //Inicia ISR para recepção de IR
    }
  } else if (btnEnviar) {
    Serial.println(F("--->"));
    tempoEmissao = micros();   
    sendRawSignal();
    Serial.print(F("<--- in "));
    Serial.print(micros() - tempoEmissao);
    Serial.println(F(" microSecs"));
    delay(2000); //Evita enviar sinal em duplicidade
  }
}
