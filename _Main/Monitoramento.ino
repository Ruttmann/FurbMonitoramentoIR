/*
 * Métodos de monitoramento de presença e detecção do estado dos dispositivos
 * http://playground.arduino.cc/Code/SimpleTimer
 */

void setupMonitoramento() {
  Serial.println("Aguarde 1 minuto, inicializando sensor PIR...");
  for (int i=0; i<60; i++) {
     digitalWrite(PIN_LED_INIT, HIGH);
     delay(500);
     digitalWrite(PIN_LED_INIT, LOW);
     delay(500);
  }
}

/*
 * O sensor PIR deve estar configurado para manter saída alta
 * durante 1 minuto a cada detecção.
 */
bool haMovimentos() {
  statusPIR = digitalRead(PIN_PIR);

  if (statusPIR == LOW) { //Se não há movimento na sala
    if (!contadorIniciado) { //Se ainda não houve ausência de movimento
      contadorIniciado = !contadorIniciado;
      tempoContador = millis();
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

  //LDR >= 700: pouca luz, dispositivo desligado.
  if (ldrAr >= 700) {
    return false;
  }

  //LDR <= 200: muita luz, dispositivo ligado.
  if (ldrAr <= 200) {
    return true;
  }  
}

bool projetorEstaLigado() {
  ldrPr = analogRead(PIN_LDR_PR);

  //LDR >= 700: pouca luz, dispositivo desligado.
  if (ldrPr >= 700) {
    return false;
  }

  //LDR <= 200: muita luz, dispositivo ligado.
  if (ldrPr <= 200) {
    return true;
  }
}

void desligaDispositivos() {
  //TODO
}
