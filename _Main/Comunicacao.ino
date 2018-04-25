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

void geraJsonSinais() {
  const unsigned int quant = 240;
//  const size_t bufferSize = JSON_ARRAY_SIZE(tamanhoSinal1) + JSON_ARRAY_SIZE(tamanhoSinal2) + JSON_OBJECT_SIZE(2);
  const size_t bufferSize = JSON_ARRAY_SIZE(quant) + JSON_ARRAY_SIZE(quant) + JSON_OBJECT_SIZE(2);
  DynamicJsonBuffer jsonBuffer(bufferSize);
  JsonObject& root = jsonBuffer.createObject();
  JsonArray& sinal1 = root.createNestedArray("sinal1");
  JsonArray& sinal2 = root.createNestedArray("sinal2");

  if (novoSinal1) {
//    for (int i=0; i<sizeof(sinalIR1)/sizeof(*sinalIR1); i++) {
//      sinal1.add(sinalIR1[i]);
//    }
//    for (int i=0; i<tamanhoSinal1; i++) {
//      sinal1.add(sinalIR1[i]);
//    }
    for (int i=0; i<quant; i++) {
      sinal1.add(sinalIR1[i]);
    }
  }
  
  if (novoSinal2) {
//    for (int i=0; i<sizeof(sinalIR2)/sizeof(*sinalIR2); i++) {
//      sinal2.add(sinalIR2[i]);
//    }
//    for (int i=0; i<tamanhoSinal2; i++) {
//      sinal2.add(sinalIR2[i]);
//    }
    for (int i=0; i<quant; i++) {
      sinal2.add(sinalIR2[i]);
    }
  }

  root.printTo(Serial);
  Serial.println("");
  
  Serial.print(F("Tamanho sinal 1: "));
  Serial.println(tamanhoSinal1);
  Serial.print(F("Tamanho JSON 1: "));
  Serial.println(sinal1.size());
  Serial.print(F("Tamanho sinal 2: "));
  Serial.println(tamanhoSinal2);
  Serial.print(F("Tamanho JSON 2: "));
  Serial.println(sinal2.size());
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
