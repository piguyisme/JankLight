#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <FastLED.h>
#include <ESPAsyncWebServer.h>
#include "index.h"
#include "env.h"

#define DATA_PIN 13
#define NUM_LEDS 16

CRGB leds[NUM_LEDS];
int circlePatternEdges[8] = {1, 2, 4, 11, 13, 14, 8, 7};
int circlePattern[12] = {0, 1, 2, 3, 4, 11, 12, 13, 14, 15, 8, 7};

int i = 0;
int mode = 1;
int brightness = 255;
int speed = 10;

AsyncWebServer server(80);

/**
Light indexes:
+ -- -- -- -- +
| 00 01 02 03 |
| 07 06 05 04 |
| 08 09 10 11 |
| 15 14 13 12 |
+ -- -- -- -- +
**/

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASS;

String getHTML() {
  String html = webpage;
  html.replace("%MODE%", String(mode));
  html.replace("%BRIGHTNESS%", String(brightness));
  html.replace("%SPEED%", String(speed));

  return html;
}

void setup() {
  if (true) { //OTA stuff
    Serial.begin(115200);
    Serial.println("Booting");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.println("Connection Failed! Rebooting...");
      delay(5000);
      ESP.restart();
    }

    // Port defaults to 3232
    // ArduinoOTA.setPort(3232);

    // Hostname defaults to esp3232-[MAC]
    // ArduinoOTA.setHostname("myesp32");

    // No authentication by default
    // ArduinoOTA.setPassword("admin");

    // Password can be set with it's md5 value as well
    // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
    // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

    ArduinoOTA
      .onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
          type = "sketch";
        else // U_SPIFFS
          type = "filesystem";

        // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
        Serial.println("Start updating " + type);
      })
      .onEnd([]() {
        Serial.println("\nEnd");
      })
      .onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
      })
      .onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR) Serial.println("End Failed");
      });

    ArduinoOTA.begin();

    Serial.println("Ready");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  // ========== Big Boy Hours ==========
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    Serial.println("ESP32 Web Server: New request received:");  // for debugging
    Serial.println("GET /");        // for debugging
    request->send(200, "text/html", getHTML());
  });
  server.on("/setState/", HTTP_GET, [](AsyncWebServerRequest* request) {
    Serial.println("ESP32 Web Server: New request received:");  // for debugging
    Serial.println("GET /setState");        // for debugging

    if (request->hasArg("mode")) {
      Serial.println(request->arg("mode"));

      mode = request->arg("mode").toInt();
    }
    if (request->hasArg("brightness")) {
      Serial.println(request->arg("brightness"));

      brightness = request->arg("brightness").toInt();
      FastLED.setBrightness(brightness);
    }
    if (request->hasArg("speed")) {
      Serial.println(request->arg("speed"));

      speed = request->arg("speed").toInt();
    }
    // int args = request->args();
    // for(int i=0;i<args;i++){
    //   Serial.printf("ARG[%s]: %s\n", request->argName(i).c_str(), request->arg(i).c_str());
    // }

    request->send(200);
  });

  // Start the server
  server.begin();

}


void loop() {
  ArduinoOTA.handle();
  FastLED.clear();
  if (mode == 0) { //Rainbow
    i = i % 255;
    fill_solid(leds, NUM_LEDS, CHSV(i, 255, 255));
  } else if (mode == 1) { //Circle
    i = i % 255;
    for (int j = 0; j < 12; j++) {
      leds[j] = CHSV(((i + (j*21)) % 255), 255, 255);
    }
  }
  else if (mode == 3) { //Alternating colors
    i = i % 16;
    for (int j = 0; j <= 15; j++) {
      leds[j] = (random(0,1) > 0.5) ? CRGB::Red : CRGB::Orange;
    }
  } else if (mode == 4) { //Snow
    i = i % 16;
    leds[i] = CRGB::White;
  } else if (mode == 5) { //Color Sections
    leds[1] = CRGB::Red;
    leds[2] = CRGB::Red;
    leds[4] = CRGB::Green;
    leds[11] = CRGB::Green;
    leds[7] = CRGB::Yellow;
    leds[8] = CRGB::Yellow;
    leds[14] = CRGB::Blue;
    leds[13] = CRGB::Blue;
  } else if (mode == 6) {
    i = i % 8;
    int redLed = circlePatternEdges[i];
    int blueLed = circlePatternEdges[(i + 4) % 8];
    Serial.print(redLed); Serial.print(" : "); Serial.print(i); Serial.print("\n");
    leds[redLed] = CRGB::Blue;
    leds[blueLed] = CRGB::Red;
  }

  FastLED.show();
  ++i;
  delay(speed);
}