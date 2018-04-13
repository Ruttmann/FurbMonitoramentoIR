//Definições de comunicação
#define W5100
#include <SocketIOClient.h>
#include <Ethernet.h>
#include <SPI.h>

//Definições de IR
#include <IRremote.h>
#define PIN_LED 7 //Pino do LED de feedback ao usuário
#define PIN_TX 45 //Emissor IR
#define PIN_RX 44 //Receptor IR
#define PIN_CAD1 46 //Pino cadastrar sinal 1
#define PIN_CAD2 47 //Pino cadastrar sinal 2
#define PIN_INICIAR 48 //Pino iniciar operação
#define maxLen 500 //Tamanho do trem de pulsos
#define FREQ 38 //Frequência da portadora em KHz

//Variáveis de comunicação
SocketIOClient client; //Cliente Websocket
byte mac[] = { 0xAA, 0x00, 0xBE, 0xEF, 0xFE, 0xEE };
char hostname[] = "intense-eyrie-51108.herokuapp.com";
int port = 3484; //Não obrigatório quando se conecta com URL.
extern String RID;
extern String Rname;
extern String Rcontent;

//Variáveis de IR
IRsend irsend; //Emissor de IR
volatile  unsigned int irBuffer[maxLen]; //Armazena os pulsos temporariamente - volatile pois é manipulado pelo ISR
volatile unsigned int x = 0; //Flag/contador do irBuffer - volatile pois é manipulado pelo ISR
unsigned int sinalIR1[maxLen]; //Armazena os tempos dos pulsos do sinal
unsigned int sinalIR2[maxLen]; //Armazena os tempos dos pulsos do sinal
unsigned int tamanhoSinal; //Armazena a quantidade de pulsos do sinal

//Botões
unsigned int btnCad1; //Cadastrar sinal 1.
unsigned int btnCad2; //Cadastrar sinal 2.
unsigned int btnIniciar; //Iniciar operação.

//Flags
bool haNovosSinais = false;
bool rodouBoot = false;

void setup() {
  Serial.begin(9600);
  boot();
}

void loop() {
    //Inicia monitoramento...
}

//Rotinas de boot do Arduino
void boot() {
  pinMode(PIN_CAD1, INPUT);
  pinMode(PIN_CAD2, INPUT);
  pinMode(PIN_INICIAR, INPUT);
  pinMode(PIN_TX, INPUT);
  pinMode(PIN_RX, INPUT);
  bool bootConcluido = false;

  while (!bootConcluido) {
    btnCad1 = digitalRead(PIN_CAD1);
    btnCad2 = digitalRead(PIN_CAD2);
    btnIniciar = digitalRead(PIN_INICIAR);

    if (btnCad1) { //Cadastrar sinal 1.
      setupIR();
      loopIR(true);
      haNovosSinais = true;
    } else if (btnCad2) { //Cadastrar sinal 2.
      setupIR();
      loopIR(false);
      haNovosSinais = true;
    }

    if (btnIniciar) { //Finalização do boot.
      setupComunicacao();
      enviarIdentificacao();
      if (haNovosSinais) {
        gerarJSON();
        enviarJSON();
      }
      bootConcluido = true;
    }
  }
  rodouBoot = true;
}
