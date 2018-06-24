/*
 * Funções para recepção e emissão de IR
 */

void setupIR() {
  attachInterrupt(0, interruptHandlerRX, CHANGE); //Inicia ISR para recepção de IR
  Serial.println(F("Pressione o botão do controle - apenas uma vez"));
  delay(5000); //Pausa de 5 segundos para receber o sinal completo
}

void loopIR(bool sinal1) {
    if (x) { //Se algum sinal foi detectado
      Serial.println();
      Serial.print(F("Raw: (")); //Reporta os pulsos do sinal
      Serial.print((x - 1));
      Serial.println(F(") "));
      detachInterrupt(0); //Desliga a interrupção de captura de sinais
      for (int i = 1; i < x; i++) { //Persiste os pulsos no array de sinal
        if (sinal1)
          sinalIR1[i-1] = irBuffer[i] - irBuffer[i - 1];
        else
          sinalIR2[i-1] = irBuffer[i] - irBuffer[i - 1];  
        Serial.print(i);
        Serial.print(F(") "));
        if (sinal1)
          Serial.println(sinalIR1[i-1]);
        else
          Serial.println(sinalIR2[i-1]);
      }
      Serial.println();
      if (sinal1) {
        tamanhoSinal1 = x - 1;
        novoSinal1 = true;
      } else {
        tamanhoSinal2 = x - 1;
        novoSinal2 = true;
      }
      x = 0;
    }
}

/*
 * Interrupção de recepção de IR
 */
void interruptHandlerRX() {
  if (x > MAX_LENGTH) return; //Ignora se irBuffer já estiver cheio.
  irBuffer[x++] = micros(); //Grava continuamente o time-stamp das transições de sinais.
}

/*
 * Envio do sinal IR RAW
 */
void sendRawSignal(bool sinal1) {
  irsend.enableIROut(FREQ); //Habilita emissor
  for(unsigned int i = 0; i < MAX_LENGTH; i++) {
    if (i & 1) {
      if (sinal1)
        irsend.space(sinalIR1[i]);
      else 
        irsend.space(sinalIR2[i]);
    } else {
      if (sinal1)
        irsend.mark(sinalIR1[i]);
      else 
        irsend.mark(sinalIR2[i]);  
    }  
  }
  irsend.space(1);
}

void zerarArraysIR() {
  memset(sinalIR1,0,sizeof(sinalIR1));
  memset(sinalIR2,0,sizeof(sinalIR2));
}
