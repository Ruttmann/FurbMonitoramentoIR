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
    enviarMensagem("endBoot", "msg", "restart");
    Serial.println(F("Retomando monitoramento..."));
  }
}

bool arEstaLigado() {
  ldrAr = analogRead(PIN_LDR_AR);

  if (ldrAr >= 600) { //Desligado
    Serial.println(F("Ar-condicionado desligado..."));
    Serial.print(F("Medição: "));
    Serial.println(ldrAr);
    return false;    
  } else { //Ligado
    Serial.println(F("Ar-condicionado ligado..."));
    Serial.print(F("Medição: "));
    Serial.println(ldrAr);
    return true;
  }
}

bool projetorEstaLigado() {
  ldrPr = analogRead(PIN_LDR_PR);
  
  if (ldrPr >= 150) { //Desligado
    Serial.println(F("Projetor desligado..."));
    Serial.print(F("Medição: "));
    Serial.println(ldrPr);
    return false;
  } else {
    Serial.println(F("Projetor ligado..."));
    Serial.print(F("Medição: "));
    Serial.println(ldrPr);
    return true;
  }
}

void desligaDispositivos() {
  for (int i=0; i<3; i++) {
    if (arEstaLigado()) {
      Serial.println(F("Desligando ar-condicionado..."));
      sendRawSignal(true);
      delay(1000);
    }
  }

  if (arEstaLigado()) {
    enviarMensagem("failure", "id", "ar");
  }
  
  delay(2000);

  for (int i=0; i<3; i++) {
    if (projetorEstaLigado()) {
      Serial.println(F("Desligando projetor..."));
      sendRawSignal(false);
      delay(2000);
      if (projetorEstaLigado()) {
        Serial.println(F("Enviado segundo sinal ao projetor..."));
        sendRawSignal(false);
      }
    }
  }

  if (projetorEstaLigado()) {
    enviarMensagem("failure", "id", "projetor");
  }
}
