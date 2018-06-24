/*
 * Métodos de monitoramento de presença e detecção do estado dos dispositivos
 */

void monitoraSalaCheia() {
  statusPIR = digitalRead(PIN_PIR);
  
  if (!statusPIR) { //Não há movimentação
    digitalWrite(PIN_LED_FDB, LOW);
    if (!contadorIniciado) {
      contadorIniciado = true;
      tempoContador = millis();
    } else {
      if ((millis() - tempoContador) >= 10000) {
        Serial.println(F("Requisitando autorização para desligar dispositivos..."));
        notificaServidor();
        contadorIniciado = false;
        tempoContador = 0;
      }
    }
  } else { //Há movimentação
    digitalWrite(PIN_LED_FDB, HIGH);
    contadorIniciado = false;
    tempoContador = 0;
  }
}

void monitoraSalaVazia() {
  statusPIR = digitalRead(PIN_PIR);
  
  if (statusPIR) { //Há movimentação
    digitalWrite(PIN_LED_FDB, HIGH);
    Serial.println(F("Reconectando dispositivo ao servidor..."));
    setupComunicacao();
    salaVazia = false;
    Serial.println(F("Retomando monitoramento..."));
  }
}

bool arEstaLigado() {
  ldrAr = analogRead(PIN_LDR_AR);

  if (ldrAr >= 600) { //Desligado
    Serial.println(F("Ar-condicionado desligado..."));
    return false;    
  } else { //Ligado
    Serial.println(F("Ar-condicionado ligado..."));
    return true;
  }
}

bool projetorEstaLigado() {
  ldrPr = analogRead(PIN_LDR_PR);
  
  if (ldrPr >= 150) { //Desligado
    Serial.println(F("Projetor desligado..."));
    return false;
  } else {
    Serial.println(F("Projetor ligado..."));
    return true;
  }
}

void desligaDispositivos() { 
  if (arEstaLigado()) {
    Serial.println(F("Desligando ar-condicionado..."));
    sendRawSignal(true);
  }

  delay(5000);

  if (projetorEstaLigado()) {
    Serial.println(F("Desligando projetor..."));
    sendRawSignal(false);
    delay(3000);
    if (projetorEstaLigado()) {
      Serial.println(F("Enviado segundo sinal ao projetor..."));
      sendRawSignal(false);
    }
  }
}
