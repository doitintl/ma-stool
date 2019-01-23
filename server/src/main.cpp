#include <Arduino.h>
#include "common.h"
#include "ma-stool-config.h"

int doorPin = 2;
int prevDoorStatus = -1;



const int DOOR_OPEN = HIGH;
const int DOOR_CLOSED = LOW;

void setup()
{

	setupWifi();
	setupPubSub(NULL);
	pinMode(doorPin, INPUT_PULLUP);
	prevDoorStatus = digitalRead(doorPin);
	publishStatus(prevDoorStatus);
}

void switchReader() {
	int dorStatus = digitalRead(doorPin);
	if (dorStatus != prevDoorStatus)
	{
		publishStatus(dorStatus);
		prevDoorStatus = dorStatus;
		Serial.print("Pulished status ");
		Serial.println(dorStatus);
	}
}

void loop()
{
	loopPubSub();
	switchReader();
	delay(500);
}
