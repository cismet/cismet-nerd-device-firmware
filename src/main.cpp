#include <Arduino.h>
#include <QRCode.h>
#include <TFT_eSPI.h>
#include "startup/c_osa.h"
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include "HtmlGenerator.h"
#include "tetris/tetris.h"
#include "breakout/breakout.h"
// #include "alienAttack/alienAttack.h"
#include "presence/presence.h"

String mqtt_server;
int mqtt_port;
String mqtt_user;
String mqtt_password;
String mqtt_topic;
bool presenceEnabled;
QRCode qrcode;
WebServer server(80);
Preferences preferences;

TFT_eSPI tft = TFT_eSPI();
const char *orientationKey = "orientation";
bool isPortrait = true; // Default orientation

const int leftButton = 0;
const int rightButton = 35;

String currentMode = "Startup"; // Default mode
String ip = "???";
WiFiManager wm;

WiFiClient espClient;
PubSubClient mqttClient(espClient);
void setDisplayOrientation(bool portrait)
{
  if (portrait)
  {
    tft.setRotation(0); // or whatever rotation is portrait
  }
  else
  {
    tft.setRotation(1); // or whatever rotation is landscape
  }
}
void drawQRCode(QRCode &qrcode, int offsetX, int offsetY)
{
  for (int y = 0; y < qrcode.size; y++)
  {
    for (int x = 0; x < qrcode.size; x++)
    {
      if (qrcode_getModule(&qrcode, x, y))
      {
        tft.fillRect(x * 2 + offsetX, y * 2 + offsetY, 2, 2, TFT_BLACK);
      }
    }
  }
}

void setMode(const String &mode)
{
  currentMode = mode;

  // Add additional logic for each mode if needed
  if (mode == "Startup")
  {
    String url = "http://" + ip;
    // Create a buffer for the QR code
    uint8_t qrcodeData[qrcode_getBufferSize(3)];
    qrcode_initText(&qrcode, qrcodeData, 3, 0, url.c_str());

    // Set the display to white background
    tft.fillScreen(TFT_WHITE);

    if (isPortrait)
    {
      tft.fillScreen(TFT_WHITE);
      tft.pushImage(0, 0, 135, 240, c_osa);
      tft.setTextColor(TFT_BLACK, TFT_WHITE);
      tft.drawString(currentMode, 10, 120, 2);
      tft.drawString("IP: " + ip, 10, 220, 2);

      // Calculate top-left corner position to center the QR code
      int offsetX = (tft.width() - qrcode.size * 2) / 2;
      int offsetY = 150;
      // Draw the QR code for (int y = 0; y < qrcode.size; y++)
      drawQRCode(qrcode, offsetX, offsetY);
    }
    else
    {
      tft.fillScreen(TFT_WHITE);
      tft.pushImage(0, 0, 135, 240, c_osa);
      tft.setTextColor(TFT_BLACK, TFT_WHITE);
      tft.drawString(currentMode, 180, 10, 2);
      tft.drawString("IP: " + ip, 115, 115, 2);

      int offsetX = 150;
      int offsetY = (tft.height() - qrcode.size * 2) / 2;
      drawQRCode(qrcode, offsetX, offsetY);
    }
  }
  else if (mode == "Presence")
  {
    if (!presenceEnabled)
    {
      Serial.println("Presence not enabled");
      setMode("Startup");
      return;
    }
    tft.fillScreen(TFT_WHITE);
    setupPresence(mqttClient);
  }
  else if (mode == "Tetris")
  {
    setDisplayOrientation(true); // Tetris only works in portrait mode
    preferences.putBool(orientationKey, true);
    tetrisSetup();
  }
  else if (mode == "Breakout")
  {
    setDisplayOrientation(true); // Breakout only works in portrait mode
    preferences.putBool(orientationKey, true);
    breakoutSetup();
  }
  // else if (mode == "AlienAttack")
  // {
  //   alienAttackSetup();
  // }
}
void getMqttConfig()
{
  mqtt_server = preferences.getString("mqtt_server", "mqtt.cismet.de");
  mqtt_port = preferences.getInt("mqtt_port", 1883);
  mqtt_user = preferences.getString("mqtt_user", "");
  mqtt_password = preferences.getString("mqtt_password", "");
  mqtt_topic = preferences.getString("mqtt_topic", "slackpresence");
  presenceEnabled = preferences.getBool("pres_enab");
}
void setup()
{
  Serial.begin(115200);

  preferences.begin("settings", false);

  pinMode(leftButton, INPUT_PULLUP);
  pinMode(rightButton, INPUT_PULLUP);
  String mode;
  // Check if the left button is pressed on startup
  if (digitalRead(rightButton) == LOW)
  {
    // If left button is pressed, immediately go to Startup mode
    mode = "Startup";
    preferences.putString("mode", mode);
  }
  else
  {
    // Otherwise, load the mode from preferences
    preferences.begin("settings", false);
    mode = preferences.getString("mode", "Startup");
  }

  getMqttConfig();

  isPortrait = preferences.getBool(orientationKey, true); // Default to portrait
  setDisplayOrientation(isPortrait);

  tft.init();
  tft.fillScreen(TFT_WHITE);
  tft.setSwapBytes(true);
  tft.pushImage(0, 0, 135, 240, c_osa);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);

  if (isPortrait)
  {
    Serial.println("Failed to connect, started AP mode");
    tft.drawString("Searching for a", 10, 200, 2);
    tft.drawString("Network .....", 10, 220, 2);
  }
  else
  {
    tft.drawString("Searching for a", 10, 100, 2);
    tft.drawString("Network .....", 10, 120, 2);
  }

  WiFi.begin(); // Start WiFi connection with stored credentials
  unsigned long startTime = millis();
  bool initialConnect = false;

  // Attempt to connect for a short period (e.g., 5 seconds)
  while (millis() - startTime < 5000)
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      initialConnect = true;
      break;
    }
    delay(100);
  }

  if (!initialConnect)
  {
    // Update display with hints before starting WiFiManager
    tft.fillScreen(TFT_WHITE);
    tft.pushImage(0, 0, 135, 240, c_osa);
    if (isPortrait)
    {
      Serial.println("Failed to connect, started AP mode");
      tft.drawString("Connect to", 10, 200, 2);
      tft.drawString("cismetFirstConnect", 10, 220, 2);
    }
    else
    {
      tft.drawString("Connect to", 10, 100, 2);
      tft.drawString("cismetFirstConnect", 10, 120, 2);
    }
    // Now use WiFiManager for a blocking connection attempt
    Serial.println("before wm.autoConnect");
    bool res = wm.autoConnect("cismetFirstConnect"); // password protected ap
    Serial.println("after wm.autoConnect");
    Serial.println(res);

    if (!res)
    {
      Serial.println("Failed to connect, started AP mode");
      tft.drawString("Connect to", 10, 200, 2);
      tft.drawString("cismetFirstConnect", 10, 220, 2);
    }
    else
    {

      Serial.println("Connected...yeey :)");
      ip = WiFi.localIP().toString();
      // Update display accordingly
    }
  }
  else
  {

    Serial.println("Connected to WiFi");

    ip = WiFi.localIP().toString();
    // Update display accordingly
  }

  setMode(mode);

  server.on("/", HTTP_GET, []()
            { server.send(200, "text/html", generateHtmlPage(currentMode, presenceEnabled)); });

  server.on("/setMode", HTTP_GET, []()
            {
    if (server.hasArg("mode")) {
        String mode = server.arg("mode");
        preferences.putString("mode", mode);
        setMode(mode);
        server.send(200, "text/plain", "Mode set to " + mode);
    } else {
        server.send(400, "text/plain", "Mode not specified");
    } });
  server.on("/saveConfig", HTTP_POST, []()
            {
                  Serial.println("saveConfig called");

      // Extract form data from POST request
      mqtt_server = server.arg("mqtt_server");
      mqtt_port = server.arg("mqtt_port").toInt();
      mqtt_user = server.arg("mqtt_user");
      mqtt_password = server.arg("mqtt_password");
      mqtt_topic = server.arg("mqtt_topic");
      presenceEnabled = server.arg("pres_enab") == "on";

      // Save data to preferences
      preferences.putString("mqtt_server", mqtt_server);
      preferences.putInt("mqtt_port", mqtt_port);
      preferences.putString("mqtt_user", mqtt_user);
      preferences.putString("mqtt_password", mqtt_password);
      preferences.putString("mqtt_topic", mqtt_topic);
      preferences.putBool("pres_enab", presenceEnabled);
      Serial.print("Presence enabled:");
      Serial.println(presenceEnabled);
      getMqttConfig();

      setMode(currentMode);

      // Redirect back to config page or main page
      server.sendHeader("Location", "/");
      server.send(303); });
  server.on("/config", HTTP_GET, []()
            {
        // Retrieve current settings from preferences

        String page = generateConfigPage(mqtt_server, mqtt_port, mqtt_user, mqtt_password, mqtt_topic, presenceEnabled);
        server.send(200, "text/html", page); });
  server.begin();
}

void loopOrientation()
{
  if (digitalRead(leftButton) == LOW)
  {
    // Debounce and toggle orientation
    delay(200); // Simple debounce
    isPortrait = !isPortrait;
    setDisplayOrientation(isPortrait);
    preferences.putBool(orientationKey, isPortrait);
    Serial.println("Setting orientation");

    setMode(currentMode);
  }
}

void loop()
{
  server.handleClient();

  if (currentMode == "Startup")
  {
    if (digitalRead(leftButton) == LOW && digitalRead(rightButton) == LOW)
    {
      // Both buttons are pressed
      unsigned long startTime = millis();
      while (digitalRead(leftButton) == LOW && digitalRead(rightButton) == LOW)
      {
        // Check if buttons are still being pressed
        if (millis() - startTime > 4000)
        {
          // Buttons have been pressed for more than 4 seconds
          Serial.println("Resetting WiFi credentials...");
          wm.resetSettings();
          ESP.restart(); // Restart the ESP
        }
      }
    }

    loopOrientation();
  }
  else if (currentMode == "Presence")
  {
    loopPresence(mqttClient);
    loopOrientation();
  }

  if (currentMode == "Tetris")
  {
    tetrisLoop();
  }

  if (currentMode == "Breakout")
  {
    breakoutLoop();
  }

  // if (currentMode == "AlienAttack")
  // {
  //   alienAttackLoop();
  // }
}