// presence.h
#ifndef PRESENCE_H
#define PRESENCE_H

#include <PubSubClient.h>

#include "avatars/david.h"
#include "avatars/helllth.h"
#include "avatars/jruiz.h"
#include "avatars/pavel.h"
#include "avatars/sabine.h"
#include "avatars/therter.h"

#include "avatars/david_away.h"
#include "avatars/helllth_away.h"
#include "avatars/jruiz_away.h"
#include "avatars/pavel_away.h"
#include "avatars/therter_away.h"
#include "avatars/sabine_away.h"

// MQTT Broker settings
// Declare as extern
extern String mqtt_server;
extern int mqtt_port;
extern String mqtt_user;
extern String mqtt_password;
extern String mqtt_topic;
extern bool presenceEnabled;

extern const int leftButton;
extern const int rightButton;

// Function declarations
void setupPresence(PubSubClient &mqttClient);
void loopPresence(PubSubClient &mqttClient);

#endif // PRESENCE_H
