#define W5100
#include "SocketIOClient.h"
#include "Ethernet.h"
#include "SPI.h"

SocketIOClient client;

byte mac[] = { 0xAA, 0x00, 0xBE, 0xEF, 0xFE, 0xEE };
//char hostname[] = "192.168.1.5";
char hostname[] = "intense-eyrie-51108.herokuapp.com";
int port = 3484;

extern String RID;
extern String Rname;
extern String Rcontent;

unsigned long previousMillis = 0; 
long interval = 10000; 
void setup() {
	pinMode(10, OUTPUT);    //for some ethernet shield on the mega : disables the SD and enables the W5100
	digitalWrite(10, LOW);
	pinMode(4, OUTPUT);
	digitalWrite(4, HIGH);
	Serial.begin(9600);

	Ethernet.begin(mac);

//	if (!client.connect(hostname, port))
  if (!client.connect(hostname))
		Serial.println(F("Not connected."));
	if (client.connected())
	{
		client.send("connection", "message", "Connected !!!!");
	}
	else
	{
		Serial.println("Connection Error");
		while(1);
	}
	delay(1000);
}

void loop()
{
	unsigned long currentMillis = millis();
	if(currentMillis - previousMillis > interval)
	{
	  previousMillis = currentMillis; 
	  //client.heartbeat(0);
	  client.send("atime", "message", "Time please?");
	}
	if (client.monitor())
	{
		Serial.println(RID);
		if (RID == "atime" && Rname == "time")
		{
			Serial.print("Time is ");
			Serial.println(Rcontent);
		}
	}
}
