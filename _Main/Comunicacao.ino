/*
 * Métodos para comunicação com o servidor remoto
 */

void setupComunicacao() {
  ficarOnline();
  conectarServidorWS();  
}

void ficarOnline() {
  if (!Ethernet.begin(mac)) {
    Serial.println(F("ERRO: Conexão Internet."));
  } else {
    Serial.println(F("Online."));
  }
}

void conectarServidorWS() {
  if (!client.connect(hostname))
    Serial.println(F("ERRO: Conexão Servidor."));
    
  if (client.connected()) {
    client.send("connection", "message", "Connected !!!!");
  } else {
    Serial.println(F("ERRO: Desconectado Servidor."));
    while(1);
  }
  delay(1000);
}

void enviarIdentificacao() {
  
}

void gerarJSON() {
  
}

void enviarJSON() {
  
}
