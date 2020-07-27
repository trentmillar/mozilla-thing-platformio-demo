#define ARDUINOJSON_USE_LONG_LONG 1

#include <ArduinoJson.h>
#include <Arduino.h>
#include <Thing.h>
#include <WebThingAdapter.h>
#include "SPIFFS.h"

#include "OSClient.h"

#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif

#define LARGE_JSON_BUFFERS 1

#include "secrets.h"

WebThingAdapter *adapter;

const char *ledTypes[] = {"OnOffSwitch", "Light", nullptr};
ThingDevice led("led", "Built-in LED", ledTypes);
ThingProperty ledOn("on", "", BOOLEAN, "OnOffProperty");

bool lastOn = false;

void start()
{
  WiFi.disconnect(true);
  WiFi.mode(WIFI_MODE_AP);
  String apname = "Aquabotics-" + WiFi.macAddress();
  String password = "password";

  WiFi.softAP(apname.c_str(), password.c_str());
  delay(100000000);
}

void setup(void)
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(115200);
  Serial.println("");
  Serial.print("Connecting to \"");
  Serial.print(SECRET_SSID);
  Serial.println("\"");

  OSClient client;
  client.start_ap();

  while(true) {

  }
  
#if defined(ESP8266) || defined(ESP32)
  WiFi.mode(WIFI_STA);
#endif
  WiFi.begin(SECRET_SSID, SECRET_PASSWORD);
  Serial.println("");

  // Wait for connection
  bool blink = true;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    digitalWrite(LED_BUILTIN, blink ? LOW : HIGH); // active low led
    blink = !blink;
  }
  digitalWrite(LED_BUILTIN, HIGH); // active low led

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(SECRET_SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  adapter = new WebThingAdapter("w25", WiFi.localIP());

  led.addProperty(&ledOn);
  adapter->addDevice(&led);
  adapter->begin();
  Serial.println("HTTP server started");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.print("/things/");
  Serial.println(led.id);

  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  File file = SPIFFS.open("/test.txt", FILE_WRITE);

  if (!file)
  {
    Serial.println("There was an error opening the file for writing");
    return;
  }

  if (file.print("TEST"))
  {
    Serial.println("File was written");
  }
  else
  {
    Serial.println("File write failed");
  }

  file.close();

  file = SPIFFS.open("/test.txt", FILE_READ);

  while (file.available())
  {
    Serial.write(file.read());
  }
  file.close();
}

void loop(void)
{
  adapter->update();
  bool on = ledOn.getValue().boolean;
  digitalWrite(LED_BUILTIN, on ? LOW : HIGH); // active low led
  if (on != lastOn)
  {
    Serial.print(led.id);
    Serial.print(": ");
    Serial.println(on);
  }
  lastOn = on;
}
