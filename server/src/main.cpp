#include <Arduino.h>
#include "common.h"
#include "ma-stool-config.h"

int doorPin = 2;
int prevDoorStatus = LOW;
int doorState; // the current reading from the input pin

const int DOOR_OPEN = HIGH;
const int DOOR_CLOSED = LOW;

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0; // the last time the output pin was toggled
unsigned long debounceDelay = 100;  // the debounce time; increase if the output flickers

void setup()
{
	Serial.begin(9600);
	setupWifi();
	setupPubSub(NULL);
	pinMode(doorPin, INPUT_PULLUP);
	delay(500);
	Serial.println("Reading switch");
	doorState = digitalRead(doorPin);
	prevDoorStatus = doorState;
}

void switchReader()
{
	int reading = digitalRead(doorPin);

	if (reading != prevDoorStatus)
	{
		lastDebounceTime = millis();
	}
	if ((millis() - lastDebounceTime) > debounceDelay)
	{

		// whatever the reading is at, it's been there for longer than the debounce
		// delay, so take it as the actual current state:

		// If the switch changed, due to noise or pressing:
		if (reading != doorState)
		{
			doorState = reading;
			publishStatus(doorState);
			Serial.print("Pulished status ");
			Serial.println(doorState);
		}
	}
	prevDoorStatus = reading;
}



void loop()
{
	loopPubSub(false);
	switchReader();
	delay(500);
}
