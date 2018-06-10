/*
 * Métodos para comunicação com o servidor remoto
 */

void setupComunicacao() {
  ficarOnline();
  conectarServidor();  
}

void ficarOnline() {
  Serial.println("Aguardando IP...");
  if (!Ethernet.begin(mac)) {
    Serial.println(F("ERRO: Conexão Internet!"));
  } else {
    Serial.println(F("Online..."));
    Serial.println(Ethernet.localIP());
  }
}

void conectarServidor() {
  Serial.println("Conectando...");
  if (!client.connect(hostname, port, nameSpace)) {
    Serial.println(F("ERRO: Conexão Servidor!"));
  } else {
    Serial.println(F("Conectado ao servidor..."));
  }
  enviarMensagem("identify", "id", identificador);
  delay(2000);
}

void geraJSONsinais(unsigned int tamanhoSinal, unsigned int* sinais) {
  unsigned int quantDivisoes = 0;
  bool ehMultiplo15 = false;
  unsigned int contadorSinal = 0;
  const char idMensagem[] = "arrayPart";
  
  if (tamanhoSinal%15 == 0) {
    quantDivisoes = tamanhoSinal/15;
    ehMultiplo15 = true;
  } else {
    quantDivisoes = tamanhoSinal/15+1;
    ehMultiplo15 = false;
  }

  enviarMensagem("sigSend", "msg", "start"); //Avisa ao servidor que vai começar a enviar sinais

  for (int i=1; i<=quantDivisoes; i++) { //Cada iteração gera um novo JSON
    StaticJsonBuffer<JSON_ARRAY_SIZE(15)> jb;
    JsonArray& arrayJSON = jb.createArray();
    if (ehMultiplo15) { //Quando o total de pulsos é múltiplo de 15
      for (int j=contadorSinal; j<contadorSinal+15; j++) {
        arrayJSON.add(sinais[j]);
      }
      arrayJSON.printTo(Serial);
      Serial.println("\n");
      contadorSinal += 15;
      enviarJSON(idMensagem, arrayJSON);
    } else { //Quando o total de pulsos não é múltiplo de 15
      if (i < quantDivisoes) { //Entra aqui para gerar JSON de 15 elementos
        for (int k=contadorSinal; k<contadorSinal+15; k++) {
          arrayJSON.add(sinais[k]);
        }
        arrayJSON.printTo(Serial);
        Serial.println("\n");
        contadorSinal += 15;
        enviarJSON(idMensagem, arrayJSON);
      } else { //Entra aqui para gerar o último JSON com < 15 elementos
        for (int l=contadorSinal; l<tamanhoSinal; l++) {
          arrayJSON.add(sinais[l]);
        }
        arrayJSON.printTo(Serial);
        Serial.println("\n");
        enviarJSON(idMensagem, arrayJSON);
      }
    }
  }

   enviarMensagem("sigSend", "msg", "end"); //Avisa ao servidor que o envio de sinais terminou
}

void zerarArraysIR() {
  memset(sinalIR1,0,sizeof(sinalIR1));
  memset(sinalIR2,0,sizeof(sinalIR2));
}

void enviarJSON(char id[], JsonArray& json) {
  String jsonString;
  json.printTo(jsonString);
  
  if (client.connected()) {
    client.sendJSON(id, jsonString);
  } else {
    Serial.println(F("ERRO: Desconectado Servidor."));
    while(1);
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

bool recebeuJSON(char id[]) {
  if (client.monitor()) {
    return RID == id;
  }
}

bool recebeuMensagem(char id[], char subId[]) {
  if (client.monitor()) {
    return RID == id && Rname == subId;
  }
}

void notificaServidor() {
  enviarMensagem("monitoring", "msg", "emptyRoom");
  delay(2000);
  if (recebeuMensagem("monitoring", "msg") && Rcontent == "ok") {
    if (recebeuSinais()) {
      desligaDispositivos();
    }
  } else {
    Serial.println("Fora do período de desligamento.");
  }
}

bool recebeuSinais() {
  bool rec1 = false;
  bool rec2 = false;
  enviarMensagem("monitoring", "msg", "getSignal");
  delay(2000);
  if (recebeuMensagem("monitoring", "msg") && Rcontent == "signal1") {
    rec1 = recebeSinal1();
  }
  rec2 = recebeSinal2();
  
  return rec1 || rec2;
}

bool recebeSinal1() {
  bool receiving = true;
  while(receiving) {
    if (recebeuMensagem("monitoring", "msg") && Rcontent == "endSignals") {
      return;
    }
    armazenaSinal(1);
  }
}

bool recebeSinal2() {
  enviarMensagem("monitoring", "msg", "haveSig2");
  delay(2000);
  if (recebeuMensagem("monitoring", "msg") && Rcontent == "signal2") {
    bool receiving = true;
    while (receiving) {
      if (recebeuMensagem("monitoring", "msg") && Rcontent == "endSignals") {
        return;
      }
      armazenaSinal(2);
    }
  }
}

void armazenaSinal(unsigned int idSinal) {
  StaticJsonBuffer<JSON_ARRAY_SIZE(15)> jb;
  bool receiving = true;
  
  if (idSinal == 1) {
    enviarMensagem("monitoring", "msg", "send1");
  } else {
    enviarMensagem("monitoring", "msg", "send2");
  }
  
  delay(1000);
  while (receiving) {
    if (recebeuJSON("sigPart")) {
      JsonArray& arrayJSON = jb.parseArray(Rcontent);
    }
    if (recebeuMensagem("monitoring", "msg") && Rcontent == "endSignals") {
      return;
    }
  }
}
