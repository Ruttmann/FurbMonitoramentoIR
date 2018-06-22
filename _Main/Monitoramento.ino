/*
 * Métodos de monitoramento de presença e detecção do estado dos dispositivos
 */

void setupMonitoramento() {
  pinMode(PIN_LED_INIT, OUTPUT);
  Serial.println("Aguarde 1 minuto, inicializando sensor PIR...");
  for (int i=0; i<63; i++) {
     digitalWrite(PIN_LED_INIT, HIGH);
     delay(500);
     digitalWrite(PIN_LED_INIT, LOW);
     delay(500);
  }
  Serial.println("Sensor PIR inicializado...");
}

/*
 * O sensor PIR deve estar configurado para manter saída alta
 * durante 1 minuto a cada detecção.
 */
bool haMovimentos() {
  if (digitalRead(PIN_PIR) == LOW) { //Se não há movimento na sala
    if (!contadorIniciado) { //Se ainda não houve ausência de movimento
      contadorIniciado = !contadorIniciado;
      tempoContador = millis();
      return true;
    } else { //Já foi iniciada uma contagem. Verifica há quanto tempo está sem movimentação
      if ((millis() - tempoContador) >= 60000) {
        return false;
      }
    }
  }
  //Se há movimento na sala
  contadorIniciado = false;
  tempoContador = 0;
  return true;
}

bool arEstaLigado() {
  ldrAr = analogRead(PIN_LDR_AR);

  if (ldrAr >= 600) { //Desligado
    return false;    
  } else { //Ligado
    return true;
  }
}

bool projetorEstaLigado() {
  ldrPr = analogRead(PIN_LDR_PR);
  
  if (ldrPr >= 150) { //Desligado
    return false;
  } else {
    return true;
  }
}

void desligaDispositivos() {
  if (arEstaLigado()) {
    sendRawSignal(true);
  }

  if (projetorEstaLigado()) {
    sendRawSignal(false);
    delay(3000);
    if (projetorEstaLigado()) {
      sendRawSignal(false);
    }
  }
}
