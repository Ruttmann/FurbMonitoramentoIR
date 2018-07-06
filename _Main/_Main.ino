/*
 * Definições de comunicação
 */
#include <SocketIOClient.h>
#include <ArduinoJson.h>
#include <Ethernet.h>
#include <SPI.h>

/*
 * Definições de IR
 * Conexões:
 *  V+           ->  +5v
 *  GND          ->  GND
 *  Signal In    ->  Pino digital 2 (padrão biblioteca IRremote)
 *  Signal Out   ->  Pino digital 46 (customizado na biblioteca IRremote)
 */
#include <IRremote.h>
#define MAX_LENGTH 500 //Tamanho do trem de pulsos
#define FREQ 38 //Frequência da portadora em KHz

/*
 * Definições de boot
 */
#define PIN_INICIAR 7 //Pino digital iniciar operação
#define PIN_CAD1 6 //Pino digital cadastrar sinal 1
#define PIN_CAD2 5 //Pino digital cadastrar sinal 2

/*
 * Definições de monitoramento 
 */
#define PIN_PIR 37 //Pino digital do sensor de presença
#define PIN_LDR_AR 0 //Pino analógico A0 do sensor LDR ar-condicionado
#define PIN_LDR_PR 1 //Pino analógico A1 do sensor LDR projetor
#define PIN_LED_FDB 53 //Pino digital do led de status de inicialização do sensor PIR

/*
 * Constantes e variáveis de comunicação
 */
SocketIOClient client; //Instância do cliente websocket
const byte mac[] = { 0xAA, 0x00, 0xBE, 0xEF, 0xFE, 0xEE }; //MAC Address do Ethernet Shield
const char hostname[] = "pdge4-furb.herokuapp.com"; //Hostname do servidor
//const char hostname[] = "192.168.0.15"; //Hostname do servidor
const int port = 3000; //Porta para conexão ao servidor (Não obrigatório)
const char nameSpace[] = "arduino"; //Namespace no servidor websocket
const char identificador[] = "S111"; //Identificador do dispositivo
extern String RID; //Guarda o Id das mensagens recebidas do servidor
extern String Rname; //Guarda o SubId das mensagens recebidas do servidor
extern String Rcontent; //Guarda o conteúdo das mensagens recebidas do servidor
unsigned long contKeepAlive = 0; //Contador para envio de mensagem keepAlive

/*
 * Variáveis de IR
 */
IRsend irsend; //Emissor de IR
volatile unsigned int irBuffer[MAX_LENGTH]; //Armazena os pulsos temporariamente
volatile unsigned int x = 0; //Contador de pulsos armazenados no irBuffer
unsigned int sinalIR1[MAX_LENGTH]; //Armazena os tempos dos pulsos do sinal 1
unsigned int tamanhoSinal1; //Armazena a quantidade de pulsos do sinal 1
unsigned int sinalIR2[MAX_LENGTH]; //Armazena os tempos dos pulsos do sinal 2
unsigned int tamanhoSinal2; //Armazena a quantidade de pulsos do sinal 2

/*
 * Variáveis de monitoramento
 */
unsigned int ldrAr; //Valor do sensor LDR do ar-condicionado
unsigned int ldrPr; //Valor do sensor LDR do projetor
unsigned int statusPIR; //Status do sensor PIR
bool contadorIniciado = false; //Controle de inicialização do contador
unsigned long tempoContador = 0; //Contador de tempo sem detecções
bool salaVazia = false; //Flag de controle das modalidades de monitoramento

/*
 * Botões
 */
unsigned int btnCad1; //Cadastrar sinal 1
unsigned int btnCad2; //Cadastrar sinal 2
unsigned int btnIniciar; //Iniciar operação

/*
 * Flags gerais
 */
bool novoSinal1 = false; //TRUE se for cadastrado novo sinal 1
bool novoSinal2 = false; //TRUE se for cadastrado novo sinal 2
bool rodouBoot = false; //Controle de boot e execução

void setup() {
  Serial.begin(9600);
  boot();
}

void loop() {
  if (salaVazia) {
    monitoraSalaVazia();
  } else {
    enviarKeepAlive();
    monitoraSalaCheia();
  }
}

/*
 * Rotinas de boot do Arduino
 */
void boot() {
  pinMode(PIN_CAD1, INPUT);
  pinMode(PIN_CAD2, INPUT);
  pinMode(PIN_INICIAR, INPUT);
  pinMode(PIN_PIR, INPUT);
  pinMode(PIN_LED_FDB, OUTPUT);
  bool bootConcluido = false;

  while (!bootConcluido) {
    btnCad1 = digitalRead(PIN_CAD1);
    btnCad2 = digitalRead(PIN_CAD2);
    btnIniciar = digitalRead(PIN_INICIAR);

    if (btnCad1) { //Cadastrar sinal ar-condicionado
      Serial.println(F("Cadastrar sinal ar-condicionado"));
      setupIR();
      loopIR(true);
    } else if (btnCad2) { //Cadastrar sinal projetor
      Serial.println(F("Cadastrar sinal projetor"));
      setupIR();
      loopIR(false);
    }

    if (btnIniciar) { //Finaliza boot e inicia monitoramento
      setupComunicacao();
      if (novoSinal1) {
        int *sinais = sinalIR1;
        geraJSONsinais(tamanhoSinal1, sinais);
      }
      if (novoSinal2) {
        int *sinais = sinalIR2;
        geraJSONsinais(tamanhoSinal2, sinais);
      }
      zerarArraysIR();
      bootConcluido = true;
      delay(3000);
      Serial.println(F("Fim do boot..."));
      enviarMensagem("endBoot", "msg", "start");
      Serial.println(F("Iniciando monitoramento..."));
    }
  }
  rodouBoot = true;
}
