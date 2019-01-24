#ifndef MASTOOL_COMMON_H
#define MASTOOL_COMMON_H
#include <PubSubClient.h>
void setupWifi();
void setupPubSub(MQTT_CALLBACK_SIGNATURE );
void reconnect(bool subscribe);
void loopPubSub(bool subscribe);
void publishStatus(int stat);
#endif
