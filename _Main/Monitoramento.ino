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
bool haMovimentos(int pirStatus) {

}

bool arEstaLigado() {
  ldrAr = analogRead(PIN_LDR_AR);

  if (ldrAr >= 600) { //Desligado
    Serial.println("Ar-condicionado desligado...");
    return false;    
  } else { //Ligado
    Serial.println("Ar-condicionado ligado...");
    return true;
  }
}

bool projetorEstaLigado() {
  ldrPr = analogRead(PIN_LDR_PR);
  
  if (ldrPr >= 150) { //Desligado
    Serial.println("Projetor desligado...");
    return false;
  } else {
    Serial.println("Projetor ligado...");
    return true;
  }
}

void desligaDispositivos() {
  sendRawSignal(true);
  delay(3000);
  sendRawSignal(false);
  
//  if (arEstaLigado()) {
//    sendRawSignal(true);
//  }

//  if (projetorEstaLigado()) {
//    sendRawSignal(false);
//    delay(3000);
//    if (projetorEstaLigado()) {
//      sendRawSignal(false);
//    }
//  }
}
