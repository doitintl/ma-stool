#include <Arduino.h>
#include "common.h"
#include "ma-stool-config.h"

int redPin = 6;
int greenPin = 7;

void indicator(char occupied)
{
	Serial.print("occupied = ");
	Serial.println(occupied);
	if (occupied == '0')
	{
		digitalWrite(redPin, HIGH);
		digitalWrite(greenPin, LOW);
	}
	else
	{
		digitalWrite(redPin, LOW);
		digitalWrite(greenPin, HIGH);
	}
}
void callback(char *topic, byte *payload, unsigned int length)
{

	Serial.print("Message arrived in topic: ");
	Serial.println(topic);
	indicator(char(payload[0]));
	Serial.print("Message: ");
	for (int i = 0; i < length; i++)
	{
		Serial.print((char)payload[i]);
	}

	Serial.println();
	Serial.println("-----------------------");
}

void setup()
{
	setupWifi();
	setupPubSub(callback);
}

void loop()
{
	loopPubSub(true);
}
