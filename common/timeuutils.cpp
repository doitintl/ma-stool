#include "timeutils.h"


char timeServer[] = "pool.ntp.org"; // NTP server

const int NTP_PACKET_SIZE = 48; // NTP timestamp is in the first 48 bytes of the message
const int UDP_TIMEOUT = 2000;   // timeout in miliseconds to wait for an UDP packet to arrive
byte packetBuffer[NTP_PACKET_SIZE]; // buffer to hold incoming and outgoing packets
void SetTime(unsigned long epoch)
{
  time_t t = (time_t)epoch;
  setTime(t);
}



// send an NTP request to the time server at the given address
void sendNTPpacket(char *ntpSrv, WiFiEspUDP *Udp)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)

  packetBuffer[0] = 0b11100011; // LI, Version, Mode
  packetBuffer[1] = 0;          // Stratum, or type of clock
  packetBuffer[2] = 6;          // Polling Interval
  packetBuffer[3] = 0xEC;       // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp->beginPacket(ntpSrv, 123); //NTP requests are to port 123

  Udp->write(packetBuffer, NTP_PACKET_SIZE);

  Udp->endPacket();
}

unsigned long GetTime(WiFiEspUDP *Udp)
{
  sendNTPpacket(timeServer,Udp); // send an NTP packet to a time server
  unsigned long startMs = millis();
  while (!Udp->available() && (millis() - startMs) < UDP_TIMEOUT)
  {
  }
  if (Udp->parsePacket())
  {
    Udp->read(packetBuffer, NTP_PACKET_SIZE);

    // the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    unsigned long epoch = secsSince1900 - seventyYears;
    return epoch;
  }
  return 0;
}

