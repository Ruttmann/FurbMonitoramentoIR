/*
 * Métodos para comunicação com o servidor remoto
 */

void setupComunicacao() {
  ficarOnline();
  conectarServidorWS();  
}

void ficarOnline() {
  if (!Ethernet.begin(mac)) {
    Serial.println(F("ERRO: Conexão Internet!"));
  } else {
    Serial.println(F("Online..."));
  }
}

void conectarServidorWS() {
  if (!client.connect(hostname, nameSpace)) {
    Serial.println(F("ERRO: Conexão Servidor!"));
  } else {
    Serial.println(F("Conectado ao servidor..."));
  }
  enviarMensagem("connection", "message", "Conectado!!!"); //
  delay(1000);
}

void enviarIdentificacao() {
  
}

void gerarJSON() {
  
}

void enviarJSON() {
  
}

void enviarMensagem(char id[], char subId[], char conteudo[]) {
  if (client.connected()) {
    client.send(id, subId, conteudo);
  } else {
    Serial.println(F("ERRO: Desconectado Servidor."));
    while(1);
  }
}

void receberMensagem() {
  if (client.monitor()) {
    /*
     * TODO: Através do RID, Rname e Rcontent verificar a procedência da mensagem
     * e chamar o método correspondente para tratamento adequado
     */
    
  }
}
