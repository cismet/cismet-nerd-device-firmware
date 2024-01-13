// presence.cpp
#include "presence.h"
#include <WiFi.h>
#include <TFT_eSPI.h>
#include <ArduinoJson.h>
#include <vector>
#include <string>

extern WiFiClient espClient;
extern PubSubClient mqttClient;
extern TFT_eSPI tft;
extern bool isPortrait;

const int portraitWidth = 135;
const int landscapeWidth = 240;

bool allAway = true;
unsigned long lastStatusChangeTime = 0;
const unsigned long screensaverTimeout = 5 * 60 * 1000; // 5 minutes
bool screensaverActive = false;

// Global JSON document
DynamicJsonDocument presenceData(1024);
std::vector<std::string> employeeNames;
unsigned long lastPresenceDataHash = 0;
unsigned long lastDisplayedDataHash = 0;

unsigned long hashString(const char *str)
{
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
    {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }

    return hash;
}
void wakeUpFromScreensaver()
{
    if (screensaverActive)
    {
        screensaverActive = false;
        lastDisplayedDataHash = 0;       // Force redraw
        lastStatusChangeTime = millis(); // Reset last status change time
    }
}
void connectToMqtt(PubSubClient &mqttClient)
{
    Serial.println("connectToMqtt called");

    while (!mqttClient.connected())
    {
        if (mqttClient.connect("arduinoClient", mqtt_user.c_str(), mqtt_password.c_str()))
        {
            mqttClient.subscribe(mqtt_topic.c_str());
        }
        else
        {
            delay(5000);
        }
    }
}

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
    // Convert payload to a String
    String message;
    for (unsigned int i = 0; i < length; i++)
    {
        message += (char)payload[i];
    }

    // Calculate hash
    unsigned long currentHash = hashString(message.c_str());

    // Compare with the last hash
    if (currentHash != lastPresenceDataHash)
    {
        lastPresenceDataHash = currentHash;

        // Clear the existing names
        employeeNames.clear();

        // Parse JSON
        DeserializationError error = deserializeJson(presenceData, message);

        if (!error)
        {

            // The JSON is now stored in presenceData
            // You can access any value by key, e.g., presenceData["helllth"]

            // Example: Print all keys and values (employee names and statuses)
            Serial.println("---new mqtt message-----");

            for (JsonPair kv : presenceData.as<JsonObject>())
            {
                Serial.print(kv.key().c_str());
                Serial.print(": ");
                Serial.println(kv.value().as<String>());

                employeeNames.push_back(kv.key().c_str());
            }
            Serial.println("------------------------");

            // Check presence status
            bool previouslyAllAway = allAway;
            allAway = true;
            for (const auto &name : employeeNames)
            {
                std::string status = presenceData[name.c_str()].as<String>().c_str();
                if (status != "away")
                {
                    allAway = false;
                    break;
                }
            }
            // If previously all were away and now at least one is available, wake up from screensaver
            if (previouslyAllAway && !allAway)
            {
                wakeUpFromScreensaver();
            }
            // Update last status change time
            lastStatusChangeTime = millis();
        }
        else
        {
            // Handle the error (e.g., print to serial)
            Serial.println("JSON Deserialization failed");
        }
    }
}

const uint16_t *getImageForEmployee(const std::string &name, const std::string &status)
{
    if (name == "david")
    {
        return (status == "away") ? david_away : david;
    }
    else if (name == "helllth")
    {
        return (status == "away") ? helllth_away : helllth;
    }
    else if (name == "jruiz")
    {
        return (status == "away") ? jruiz_away : jruiz;
    }
    else if (name == "pavel")
    {
        return (status == "away") ? pavel_away : pavel;
    }
    else if (name == "sabine")
    {
        return (status == "away") ? sabine_away : sabine;
    }
    else if (name == "therter")
    {
        return (status == "away") ? therter_away : therter;
    }

    return nullptr; // Return nullptr if no matching image is found
}

void setupPresence(PubSubClient &mqttClient)
{

    if (mqttClient.connected())
    {
        Serial.println("Will disconnect mqttClient");
        mqttClient.unsubscribe(mqtt_topic.c_str());
        mqttClient.disconnect();
        delay(500);
    }

    Serial.println("mqttClient connected");
    Serial.print(mqttClient.connected());

    Serial.println("Will setup mqttClient");
    Serial.print("Presence enabled:");
    Serial.println(presenceEnabled);

    Serial.print("mqtt_server:");
    Serial.println(mqtt_server);
    Serial.print("mqtt_port:");
    Serial.println(mqtt_port);
    Serial.print("mqtt_user:");
    Serial.println(mqtt_user);
    Serial.print("mqtt_password:");
    Serial.println("***");
    // Serial.println(mqtt_password);
    Serial.print("mqtt_topic:");
    Serial.println(mqtt_topic);

    // Clear existing data
    employeeNames.clear();
    presenceData.clear();
    lastPresenceDataHash = 0;
    lastDisplayedDataHash = 0;

    // MQTT setup code
    mqttClient.setServer(mqtt_server.c_str(), mqtt_port);
    mqttClient.setCallback(mqttCallback);
}

void loopPresence(PubSubClient &mqttClient)
{
    if (!mqttClient.connected())
    {
        connectToMqtt(mqttClient);
    }
    mqttClient.loop();

    if (!screensaverActive && lastPresenceDataHash != lastDisplayedDataHash)
    {
        lastDisplayedDataHash = lastPresenceDataHash;

        // Clear screen
        tft.fillScreen(TFT_WHITE);

        // Initial positions based on orientation
        int initialX = isPortrait ? 5 : 20;
        int initialY = isPortrait ? 20 : 5;
        int x = initialX;
        int y = initialY;
        int gapX = 5; // Horizontal gap between images and borders
        int gapY = 5; // Vertical gap between images and borders

        // Determine the number of columns based on orientation
        int numColumns = isPortrait ? 2 : 3;
        int imageWidth = 60;
        int totalGapX = (numColumns + 1) * gapX;
        int availableWidth = (isPortrait ? portraitWidth : landscapeWidth) - totalGapX;
        int columnWidth = availableWidth / numColumns;

        for (const std::string &name : employeeNames)
        {
            std::string status = presenceData[name.c_str()].as<String>().c_str();
            const uint16_t *image = getImageForEmployee(name, status);

            if (image != nullptr)
            {
                tft.pushImage(x, y, 60, 60, image);
            }

            // Update position
            x += columnWidth;
            if (x + imageWidth > (isPortrait ? portraitWidth : landscapeWidth))
            {
                x = initialX;   // Reset x to initial value
                y += 60 + gapY; // Move to next row
            }
        }
    }
    // Screensaver logic
    // Serial.print("----screensaverActive:");
    // Serial.println(screensaverActive);

    // Serial.print("----allAway:");
    // Serial.println(allAway);
    // Serial.println(millis() - lastStatusChangeTime);
    // Serial.println(millis() - lastStatusChangeTime > screensaverTimeout);
    // Serial.println(allAway && (millis() - lastStatusChangeTime > screensaverTimeout) && !screensaverActive);
    if (screensaverActive)
    {
        if (digitalRead(rightButton) == LOW)
        {
            delay(200); // Simple debounce
            wakeUpFromScreensaver();
        }
    }
    else if (allAway && (millis() - lastStatusChangeTime > screensaverTimeout) && !screensaverActive)
    {
        tft.fillScreen(TFT_BLACK);
        screensaverActive = true;
    }
    else if (!allAway && screensaverActive)
    {
        wakeUpFromScreensaver();
    }
}
