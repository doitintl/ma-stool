#ifndef __MASTOOL_CONFIG__
#define __MASTOOL_CONFIG__

/* In order to build the project create a ma-stool-config.cpp in the common
directory and assign values to the variables

const char *ssid = "";
const char *pass = "";

const char *mqttServer = "";
int mqttPort = 0;
const char *mqttUser = "";
const char *mqttPassword = "";
const char *mastoolTopic = "";
*/
extern const char *ssid; //Wifi SSID
extern const char *pass; //Wifi passowrd

extern const char *mqttServer;
extern int mqttPort;
extern const char *mqttUser;
extern const char *mqttPassword;
extern const char *mastoolTopic;

#endif
