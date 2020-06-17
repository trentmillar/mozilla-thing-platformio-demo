#define ARDUINOJSON_USE_LONG_LONG 1

#include <ArduinoJson.h>
#include <Arduino.h>
#include <Thing.h>
#include <WebThingAdapter.h>

#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif


#include "secrets.h"

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(20000);
}