#include <SocketIOClient.h>

/*
 * Variáveis externas
 */
String tmpdata = "";
String RID = "";
String Rname = "";
String Rcontent = "";

bool SocketIOClient::connect(char thehostname[], int theport, char thensp[]) {
	while(!client.connect(thehostname, theport)){
	    Serial.println("LIB: Sem conexão...");
	    delay(200);
	}
  	Serial.println("LIB: Conexão estabelecida...");
	hostname = thehostname;
	port = theport;
	nsp = thensp;
	sendHandshake(hostname);
	Serial.println("LIB: Handshake estabelecido...");
	bool result = readHandshake();
	return result;
}

bool SocketIOClient::reconnect(char thehostname[], int theport, char thensp[])
{
	return connect(thehostname, theport, thensp);
}

bool SocketIOClient::connected() {
	return client.connected();
}

void SocketIOClient::disconnect() {
	client.stop();
}

// find the nth colon starting from dataptr
void SocketIOClient::findColon(char which) {
	while (*dataptr) {
		if (*dataptr == ':') {
			if (--which <= 0) return;
		}
		++dataptr;
	}
}

// terminate command at dataptr at closing double quote
void SocketIOClient::terminateCommand(void) {
	dataptr[strlen(dataptr) - 3] = 0;
}

void SocketIOClient::parser(int index) {
	String rcvdmsg = "";
	int sizemsg = databuffer[index + 1];   // 0-125 byte, index ok        Fix provide by Galilei11. Thanks
	if (databuffer[index + 1]>125)
	{
		sizemsg = databuffer[index + 2];    // 126-255 byte
		index += 1;       // index correction to start
	}
	Serial.print("Tamanho da mensagem = ");	//Can be used for debugging
	Serial.println(sizemsg);			//Can be used for debugging
	for (int i = index + 2; i < index + sizemsg + 2; i++)
		rcvdmsg += (char)databuffer[i];
	Serial.print("Mensagem recebida = ");	//Can be used for debugging
	Serial.println(rcvdmsg);				//Can be used for debugging
	switch (rcvdmsg[0])
	{
	case '2':
		Serial.println("Ping recebido - Enviando Pong");
		heartbeat(1);
		break;

	case '3':
		Serial.println("Pong recebido!");
		break;

	case '4':
		switch (rcvdmsg[1])
		{
		case '0':
			Serial.println("Upgrade para WebSocket confirmado!");
			break;
		case '2':
			RID = rcvdmsg.substring(rcvdmsg.indexOf("[\"")+2, rcvdmsg.indexOf("\","));
			Rname = rcvdmsg.substring(rcvdmsg.indexOf("\",") + 4, rcvdmsg.indexOf("\":"));
			Rcontent = rcvdmsg.substring(rcvdmsg.indexOf(":")+1, rcvdmsg.indexOf("}"));
			break;
		}
	}
}

bool SocketIOClient::monitor() {
	int index = -1;
	int index2 = -1;
	String tmp = "";
	*databuffer = 0;

	if (!client.connected()) {
		if (!client.connect(hostname, port)) return 0;
	}

	if (!client.available())
	{
		return 0;
	}
	char which;

	while (client.available()) {
		readLine();
		tmp = databuffer;
		Serial.println(databuffer);
		dataptr = databuffer;
		index = tmp.indexOf((char)129);	//129 DEC = 0x81 HEX = sent for proper communication
		index2 = tmp.indexOf((char)129, index + 1);
		if (index != -1)
		{
			parser(index);
		}
		if (index2 != -1)
		{
			parser(index2);
		}
	}
	return 1;
}

void SocketIOClient::sendHandshake(char hostname[]) {
	String request = "";
	request +=	"GET /socket.io/1/?EIO=3&transport=polling&b64=true HTTP/1.1\r\n";
	request +=	"Host: ";
	request += hostname;
	request += "\r\n";
	request +=	"Origin: Arduino\r\n";
	request +=	"Connection: keep-alive\r\n\r\n";

	client.print(request);
}

bool SocketIOClient::waitForInput(void) {
	unsigned long now = millis(); //Registra o momento atual.
	while (!client.available() && ((millis() - now) < 30000UL)) { ; } //Aguarda chegar dados do servidor
	return client.available(); 
}

void SocketIOClient::eatHeader(void) {
	while (client.available()) {	// consume the header
		readLine();
		if (strlen(databuffer) == 0) break;
	}
}

bool SocketIOClient::readHandshake() {

	if (!waitForInput()) return false;

	// check for happy "HTTP/1.1 200" response
	readLine();
	if (atoi(&databuffer[9]) != 200) {
		while (client.available()) readLine();
		client.stop();
		return false;
	}
	eatHeader();
	readLine();
	String tmp = databuffer;

	int sidindex = tmp.indexOf("sid");
	int sidendindex = tmp.indexOf("\"", sidindex + 6);
	int count = sidendindex - sidindex - 6;

	for (int i = 0; i < count; i++)
	{
		sid[i] = databuffer[i + sidindex + 6];
	}
	Serial.println(" ");
	Serial.print(F("HANDSHAKE: Conectado. SID="));
	Serial.println(sid);	// sid:transport:timeout 

	while (client.available()) readLine();
	client.stop();
	delay(200);
	if (!client.connect(hostname, port)) {
		Serial.print(F("HANDSHAKE: Websocket falhou."));
		return false;
	}
	Serial.println(F("HANDSHAKE: Conectando via Websocket."));

	String request = "";
	request += "GET /socket.io/1/websocket/?EIO=3&transport=websocket&b64=true&sid=" ;
	request += sid ;
	request += " HTTP/1.1\r\n" ;
	request += "Host: " ;
	request += hostname ;
	request += "\r\n" ;
	request += "Origin: Arduino\r\n" ;
	request += "Sec-WebSocket-Key: " ;
	request += sid ;
	request += "\r\n" ;
	request += "Sec-WebSocket-Version: 13\r\n" ;
	request += "Upgrade: websocket\r\n" ;
	request += "Connection: Upgrade\r\n" ;
	request += "\r\n" ;

	client.print(request);

	Serial.println(F("HANDSHAKE: Websocket enviado."));

	if (!waitForInput()) return false;
	readLine();
	if (atoi(&databuffer[9]) != 101) {	// check for "HTTP/1.1 101 response, means Updrage to Websocket OK
		while (client.available()) readLine();

		client.stop();
		return false;
	}
	readLine();
	readLine();
	readLine();
	for (int i = 0; i < 28; i++)
	{
		key[i] = databuffer[i + 22];	//key contains the Sec-WebSocket-Accept, could be used for verification
	}


	eatHeader();


	/*
	Generating a 32 bits mask requiered for newer version
	Client has to send "52" for the upgrade to websocket
	*/
	randomSeed(analogRead(0));
	String mask = "";
	String masked = "52";
	String message = "52";
	for (int i = 0; i < 4; i++)	//generate a random mask, 4 bytes, ASCII 0 to 9
	{
		char a = random(48, 57);
		mask += a;
	}

	for (int i = 0; i < message.length(); i++)
		masked[i] = message[i] ^ mask[i % 4];	//apply the "mask" to the message ("52")



	client.print((char)0x81);	//has to be sent for proper communication
	client.print((char)130);	//size of the message (2) + 128 because message has to be masked
	client.print(mask);
	client.print(masked);

	monitor();		// treat the response as input
	if(nsp != ""){
		sendNSP();
	}

	return true;
}

void SocketIOClient::readLine() {
	for (int i = 0; i < DATA_BUFFER_LEN; i++)
		databuffer[i] = ' ';
	dataptr = databuffer;
	while (client.available() && (dataptr < &databuffer[DATA_BUFFER_LEN - 2]))
	{
		char c = client.read();
		Serial.print(c);			//Can be used for debugging
		if (c == 0) Serial.print("");
		else if (c == 255) Serial.println("");
		else if (c == '\r') { ; }
		else if (c == '\n') break;
		else *dataptr++ = c;
	}
	*dataptr = 0;
}

void SocketIOClient::sendMessage(String message){
	String dataSend = "";
	int header[10];
	header[0] = 0x81;
	int msglength = message.length();
	randomSeed(analogRead(0));
	String mask = "";
	String masked = message;
	for (int i = 0; i < 4; i++)
	{
		char a = random(1, 10);
		mask += a;
	}
	for (int i = 0; i < msglength; i++)
		masked[i] = message[i] ^ mask[i % 4];
	
	dataSend += (char)header[0];

	if (msglength <= 125)
	{
		header[1] = msglength + 128;
		dataSend += (char)header[1];
	}
	else if (msglength >= 126 && msglength <= 65535)
	{
		header[1] = 126 + 128;
		dataSend += (char)header[1];
		header[2] = (msglength >> 8) & 255;
		dataSend += (char)header[2];
		header[3] = (msglength)& 255;
		dataSend += (char)header[3];
	}
	else
	{
		int num = 1;
		header[num] = 127 + 128;
		dataSend += (char)header[num];
		for(int i = 56 ; i > 0 ; i -= 8){
			num++;
			header[num] = (msglength >> i) & 255;
			dataSend += (char)header[num];
		}
		header[9] = (msglength)& 255;
		dataSend += (char)header[9];

	}
	dataSend += mask;
	dataSend += masked;

	client.print(dataSend);
}

void SocketIOClient::send(String RID, String Rname, String Rcontent) {

	String message = "42/" + String(nsp) + ",[\"" + RID + "\",{\"" + Rname + "\":\"" + Rcontent + "\"}]";
	
	sendMessage(message);
}

void SocketIOClient::sendNSP() {

	String message = "40/" + String(nsp);
	
	sendMessage(message);
}

void SocketIOClient::sendJSON(String RID, String JSON) {
	String message = "42/" + String(nsp) + ",[\"" + RID + "\"," + JSON + "]";
	
	sendMessage(message);
}

void SocketIOClient::heartbeat(int select) {
	randomSeed(analogRead(0));
	String mask = "";
	String masked = "";
	String message = "";
	String dataSend = "";
	if (select == 0)
	{
		masked = "2";
		message = "2";
	}
	else
	{
		masked = "3";
		message = "3";
	}
	for (int i = 0; i < 4; i++)	//generate a random mask, 4 bytes, ASCII 0 to 9
	{
		char a = random(1, 10);
		mask += a;
	}

	for (int i = 0; i < message.length(); i++)
		masked[i] = message[i] ^ mask[i % 4];	//apply the "mask" to the message ("2" : ping or "3" : pong)

	dataSend += (char)0x81;
	dataSend += (char)129;
	dataSend += mask;
	dataSend += masked;
	client.print(dataSend);
}

void SocketIOClient::clear() {
	RID = "";
	Rname = "";
	Rcontent = "";
}