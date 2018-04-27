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
    Serial.println(Ethernet.localIP());
  }
}

void conectarServidorWS() {
  if (!client.connect(hostname, port, nameSpace)) {
    Serial.println(F("ERRO: Conexão Servidor!"));
  } else {
    Serial.println(F("Conectado ao servidor..."));
  }
  enviarMensagem("connection", "id", identificador);
  delay(1000);
}

void enviarNovosSinais() {
  if (novoSinal1) {
    
  }

  if (novoSinal2) {
    
  }
  
}

void geraJsonSinais(int tamanhoSinal, char nomeSinal[], int* sinais) {
//  DynamicJsonBuffer jsonBuffer(JSON_ARRAY_SIZE(tamanhoSinal) + JSON_OBJECT_SIZE(1));
  DynamicJsonBuffer jsonBuffer(JSON_ARRAY_SIZE(10) + JSON_OBJECT_SIZE(1));
  JsonObject& root = jsonBuffer.createObject();
  JsonArray& sinal = root.createNestedArray(nomeSinal);

//    for (int i=0; i<=tamanhoSinal; i++) {
    for (int i=0; i<=10; i++) {
      sinal.add(sinais[i]);
    }

  Serial.println("");
  
  Serial.print(F("Tamanho sinal: "));
  Serial.println(tamanhoSinal);
  Serial.print(F("Tamanho JSON: "));
  Serial.println(sinal.size());
  
  enviarJSON(root);
}

void enviarJSON(JsonObject& root) {
  String json;
  root.printTo(json);
  if (client.connected()) {
//    client.sendJSON("newSignal", "{\"sinal1\":[4484,4524,556,1680,532]}");
    client.sendJSON("newSignal", json);
  } else {
    Serial.println(F("ERRO: Desconectado Servidor."));
    while(1);
  }
  
  while(true) {
    enviarMensagem("haha", "tag", "manda o json");
    delay(3000);
    if (client.monitor()) {
      Serial.println("ANTESS");
      Serial.println(RID);
      Serial.println(Rname);
      Serial.println(Rcontent);
//      if (RID == "resp") {
//        Serial.println(Rcontent);
//      }
    }
  }
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
