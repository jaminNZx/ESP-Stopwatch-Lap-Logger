#ifdef ESP32
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#else
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#endif

#include <wifi_credentials.h>
#include <SimpleTimer.h>
#include "settings.h"

SimpleTimer timer;

long StopwatchTimeMasterMilliSec, StopwatchTimeLapMilliSec, StopwatchTimeLapBest;
int StopwatchTimer, StopwatchTimerMS, StopwatchTimerLapMS, rowIndex = 0;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
#ifdef USE_LOCAL_SERVER
  Blynk.begin(AUTH, WIFI_SSID, WIFI_PASS, SERVER);
#else
  Blynk.begin(AUTH, WIFI_SSID, WIFI_PASS);
#endif
  while (Blynk.connect() == false) {}
#ifdef OTA_UPDATES
#ifndef ESP32
  ArduinoOTA.setHostname(OTA_HOSTNAME); // OPTIONAL
  ArduinoOTA.begin();
#endif
#endif
  StopwatchTimerMS = timer.setInterval(1, []() {
    StopwatchTimeMasterMilliSec++;
  });
  StopwatchTimerLapMS = timer.setInterval(1, []() {
    StopwatchTimeLapMilliSec++;
  });
  StopwatchTimer = timer.setInterval(123, []() {
    Blynk.virtualWrite(1, StopwatchGetFormatted(StopwatchTimeMasterMilliSec, 0));
    Blynk.virtualWrite(2, StopwatchGetFormatted(StopwatchTimeLapMilliSec, 1));
  });
  timer.disable(StopwatchTimerMS);
  timer.disable(StopwatchTimerLapMS);
  Blynk.virtualWrite(6, "clr");
  Blynk.virtualWrite(4, 0);
}

// Button Widget (Switch Type): Start/Pause Timer
BLYNK_WRITE(4) {
  if (param.asInt()) {
    timer.enable(StopwatchTimerMS);
    timer.enable(StopwatchTimerLapMS);
  } else {
    timer.disable(StopwatchTimerMS);
    timer.disable(StopwatchTimerLapMS);
    Blynk.virtualWrite(1, StopwatchGetFormatted(StopwatchTimeMasterMilliSec, 0));
    Blynk.virtualWrite(2, StopwatchGetFormatted(StopwatchTimeLapMilliSec, 1));
  }
}

// Button Widget (Momentary): Lap
BLYNK_WRITE(3) {
  if (param.asInt()) StopwatchNewLap();
}

void StopwatchNewLap() {
  if (timer.isEnabled(StopwatchTimerMS)) {
    if (!StopwatchTimeLapBest || StopwatchTimeLapMilliSec < StopwatchTimeLapBest, 0) {
      StopwatchTimeLapBest = StopwatchTimeLapMilliSec;
      Blynk.virtualWrite(7, StopwatchGetFormatted(StopwatchTimeLapMilliSec, 1));
    }
    rowIndex++;
    Blynk.virtualWrite(6, "add", rowIndex, String("Lap ") + rowIndex, StopwatchGetFormatted(StopwatchTimeLapMilliSec, 1));
    Blynk.virtualWrite(6, "pick", rowIndex);
    StopwatchTimeLapMilliSec = 0;
  }
}

// Button Widget (Momentary): Reset Stopwatch
BLYNK_WRITE(5) {
  if (param.asInt()) {
    timer.disable(StopwatchTimerMS);
    timer.disable(StopwatchTimerLapMS);
    StopwatchTimeMasterMilliSec = 0;
    StopwatchTimeLapMilliSec = 0;
    StopwatchTimeLapBest = 0;
    rowIndex = 0;
    Blynk.virtualWrite(1, StopwatchGetFormatted(StopwatchTimeMasterMilliSec, 0));
    Blynk.virtualWrite(1, StopwatchGetFormatted(StopwatchTimeLapMilliSec, 1));
    Blynk.virtualWrite(6, "clr");
    Blynk.virtualWrite(4, 0);
    Blynk.virtualWrite(7, StopwatchGetFormatted(StopwatchTimeLapBest, 1));
  }
}

String StopwatchGetFormatted(long milliSeconds, int Short) {
  long days, hours, mins, secs, ms;
  String returned, ms_o, secs_o, mins_o, hours_o;
  ms = milliSeconds;
  secs = milliSeconds / 1000;
  mins = secs / 60;
  hours = mins / 60;
  days = hours / 24;
  ms = milliSeconds - (secs * 1000);
  secs = secs - (mins * 60);
  mins = mins - (hours * 60);
  hours = hours - (days * 24);
  if (Short) {
    returned = "";
    if (ms == 0) ms_o = "00";
    if (ms < 100) ms_o = "0";
    if (ms < 10) ms_o = "00";
    if (secs < 10) secs_o = "0";
    if (mins < 10) mins_o = "0";
    if (hours < 10) hours_o = "0";
    if (days) returned += days + String(":");
    if (hours) returned += hours_o + hours + String(":");
    if (mins) returned += mins_o + mins + String(":");
    returned += secs_o + secs + String(".");
    returned += ms_o + ms;
    return returned;
  } else {
    if (ms == 0)  ms_o = ".00";
    if (ms < 100) ms_o = ".0";
    if (ms < 10)  ms_o = ".00";
    if (secs < 10) secs_o = ":0";
    if (mins < 10) mins_o = ":0";
    if (hours < 10) hours_o = ":0";
    return days + hours_o + hours + mins_o + mins + secs_o + secs + ms_o + ms;
  }
}


void loop() {
  Blynk.run();
#ifndef ESP32
  ArduinoOTA.handle();
#endif
  timer.run();
}
