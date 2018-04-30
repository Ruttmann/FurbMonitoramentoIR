#include <Arduino.h>

#include <Ethernet.h>
#include <SPI.h>

/*
 * Tamanho dos data buffers estáticos
 */
#define DATA_BUFFER_LEN 120
#define SID_LEN 24

class SocketIOClient {
public:
	bool connect(char hostname[], int port = 80, char nsp[] = "");
	bool connected();
	void disconnect();
	bool reconnect(char hostname[], int port = 80, char nsp[] = "");
	bool monitor();
	void sendMessage(String message = "");
	void send(String RID, String Rname, String Rcontent);
	void sendNSP();
	void sendJSON(String RID, String JSON);
	void heartbeat(int select);
	void clear();
private:
	void parser(int index);
	void sendHandshake(char hostname[]);
	EthernetClient client;
	bool readHandshake();
	void readLine();
	char *dataptr;
	char databuffer[DATA_BUFFER_LEN];
	char sid[SID_LEN];
	char key[28];
	char *hostname;
	char *nsp;
	int port;

	void findColon(char which);
	void terminateCommand(void);
	bool waitForInput(void);
	void eatHeader(void);
};