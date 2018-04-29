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

void geraJsonSinais(unsigned int tamanhoSinal, char nomeSinal[], unsigned int* sinais) {
  unsigned int quantDivisoes = 0;
  bool ehMultiplo15 = false;
  unsigned int contadorSinal = 0;
  
  if (tamanhoSinal%15 == 0) {
    quantDivisoes = tamanhoSinal/15;
    ehMultiplo15 = true;
  } else {
    quantDivisoes = tamanhoSinal/15+1;
    ehMultiplo15 = false;
  }

  /*
   * Colocar dentro de um método
   */
  for (int i=1; i<=quantDivisoes; i++) { //Cada iteração gera um novo JSON
    StaticJsonBuffer<JSON_ARRAY_SIZE(15)> jb;
    JsonArray& arrayJSON = jb.createArray();
    if (ehMultiplo15) { //Quando o total de pulsos é múltiplo de 15
      Serial.println("MULTIPLO 15");
      for (int j=contadorSinal; j<contadorSinal+15; j++) {
        arrayJSON.add(sinais[j]);
      }
      contadorSinal += 15;
      Serial.println("MULTIPLO 15");
    } else {
      if (i < quantDivisoes) { //Entra aqui para gerar JSON de 15 elementos
        Serial.println("NAO MULTIPLO");
        for (int k=contadorSinal; k<contadorSinal+15; k++) {
          arrayJSON.add(sinais[k]);
        }
        arrayJSON.printTo(Serial);
        Serial.println(arrayJSON.size());
        contadorSinal += 15;
      } else { //Entra aqui para gerar o último JSON com < 15 elementos
        for (int l=contadorSinal; l<tamanhoSinal; l++) {
          arrayJSON.add(sinais[l]);
        }
        arrayJSON.printTo(Serial);
        Serial.println(arrayJSON.size());
        contadorSinal += tamanhoSinal - contadorSinal; //Apenas para debug, excluir após testar
      }
    }
    //TODO: Enviar o arrayJSON para o servidor
  }
  /*
   * Colocar dentro de um método
   */
  
  
//    for (int i=0; i<=tamanhoSinal; i++) {
//    for (int i=0; i<15; i++) {
//      sinal.add(sinais[i]);
//        root.add(sinais[i]);
//    }

//  Serial.println("");
//  
//  Serial.print(F("Tamanho sinal: "));
//  Serial.println(tamanhoSinal);
//  Serial.print(F("Tamanho JSON: "));
//  Serial.println(root.size());
  
//  enviarJSON(root);
}

void enviarJSON(JsonArray& root) {
  String json;
  root.printTo(json);
  if (client.connected()) {
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
