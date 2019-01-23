#ifndef __TIMEUTILS_H__
#define __TIMEUTILS_H__
#include <Arduino.h>
#include <TimeLib.h>
#include <WiFiEspUdp.h>

void SetTime(unsigned long epoch, WiFiEspUDP *Udp);
unsigned long GetTime(WiFiEspUDP *Udp);

#endif