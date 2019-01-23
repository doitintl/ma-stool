#include "WiFiEsp.h"
#include "WiFiEspUdp.h"
#include <PubSubClient.h>
#include "ma-stool-config.h"
#include "timeutils.h"
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(17, 18); // RX, TX
#endif
WiFiEspClient espClient;
PubSubClient client(espClient);
unsigned long lastclientloop = 0;

int status = WL_IDLE_STATUS; // the Wifi radio's status
WiFiEspUDP Udp;
unsigned int localPort = 2390; // local port to listen for UDP packets
void setupWifi()
{
		// initialize serial for debugging
	Serial.begin(9600);
	// initialize serial for ESP module
	Serial1.begin(115200);
	// initialize ESP module
	delay(1001);
	WiFi.init(&Serial1, true);
	// check for the presence of the shield
	if (WiFi.status() == WL_NO_SHIELD)
	{
		Serial.println("WiFi shield not present");
		// don't continue
		while (true)
			;
	}
	int numSsid = WiFi.scanNetworks(ssid);
	;
	if (numSsid == -1)
	{

		Serial.println("Couldn't get a wifi connection");
		while (true)
			;
	}
	while (status != WL_CONNECTED)
	{
		Serial.print("Attempting to connect to WPA SSID: ");
		// Connect to WPA/WPA2 network
		if (numSsid >1){
			status = WiFi.begin(ssid, pass, WiFi.MAC(0));
		} else {
			status = WiFi.begin(ssid, pass);
		}
		delay(2000);
	}

	// you're connected now, so print out the data
	Serial.println("You're connected to the network");
	delay(2000);
	Udp.stop();
	Udp.begin(localPort);
	Serial.println("Waiting on time sync...");
	while (GetTime(&Udp) < 1510644967)
	{
		delay(10);
	}
	Udp.stop();
}

void setupPubSub(MQTT_CALLBACK_SIGNATURE)
{
	client.setServer(mqttServer, mqttPort);
	client.setCallback(callback);
}

void reconnect()
{
	// Loop until we're reconnected
	while (!client.connected())
	{
		Serial.println("Attempting MQTT connection...");
		while (!client.connected())
		{
			Serial.println("Connecting to MQTT...");
			// Create a random client ID
			String clientId = "Mastool-0";
			clientId += String(random(0xffff), HEX);
			if (client.connect(clientId.c_str(), mqttUser, mqttPassword))
			{

				Serial.println("connected");
			}
			else
			{

				Serial.print("failed with state ");
				Serial.print(client.state());
				delay(2000);
			}
		}
		//client.publish("mastool/t1", "Hello from ESP8266");
		client.subscribe("ma-stool/bathroom");
		Serial.println("Done Setup!");
	}
}

void loopPubSub()
{
	if (millis() - lastclientloop > 1000)
	{
		bool clientconnected = client.loop();
		if (!clientconnected)
		{
			reconnect();
		}
		lastclientloop = millis();

	};
}

void publishStatus(int stat){
	char buffer [2];
	itoa(stat,buffer, 10);
	client.publish(mastoolTopic, buffer);
}

