#define ESP32 

#ifdef ESP32
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#else
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#endif

#include <SimpleTimer.h>

char auth[] = "4c34e9c7f73d4fd684dbef6e57af127c";
char ssid[] = "PrettyFlyForAWiFi";
char pass[] = "custom2015";

SimpleTimer timer;

long StopwatchTimeMasterMilliSec;
long StopwatchTimeLapMilliSec, StopwatchTimeLapBest;
int StopwatchTimer, StopwatchTimerMS, StopwatchTimerLapMS;
int rowIndex = 0, shortFormat = 1;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  Blynk.begin(auth, ssid, pass, IPAddress(192, 168, 1, 2));
  while (Blynk.connect() == false) {}
#ifndef ESP32
  ArduinoOTA.setHostname("Stopwatcher"); // OPTIONAL
  ArduinoOTA.begin();
#endif
  StopwatchTimerMS = timer.setInterval(1, StopwatchTimerMSFunction);
  StopwatchTimerLapMS = timer.setInterval(1, StopwatchTimerLapMSFunction);
  StopwatchTimer = timer.setInterval(223, StopwatchTimerFunction);
  timer.disable(StopwatchTimerMS);
  timer.disable(StopwatchTimerLapMS);
  Blynk.virtualWrite(6, "clr");
  Blynk.virtualWrite(4, 0);
}

void StopwatchTimerMSFunction() {
  StopwatchTimeMasterMilliSec++;
}

void StopwatchTimerLapMSFunction() {
  StopwatchTimeLapMilliSec++;
}

void StopwatchTimerFunction() {
  Blynk.virtualWrite(1, StopwatchGetFormatted(StopwatchTimeMasterMilliSec));
  Blynk.virtualWrite(2, StopwatchGetFormattedShort(StopwatchTimeLapMilliSec));
}

// Button Widget (Switch Type): Start/Pause Timer
BLYNK_WRITE(4) {
  if (param.asInt()) {
    timer.enable(StopwatchTimerMS);
    timer.enable(StopwatchTimerLapMS);
  } else {
    timer.disable(StopwatchTimerMS);
    timer.disable(StopwatchTimerLapMS);
    Blynk.virtualWrite(1, StopwatchGetFormatted(StopwatchTimeMasterMilliSec));
    Blynk.virtualWrite(2, StopwatchGetFormattedShort(StopwatchTimeLapMilliSec));
  }
}

// Button Widget (Momentary): Lap
BLYNK_WRITE(3) {
  if (param.asInt()) {
    StopwatchNewLap();
  }
}

void StopwatchNewLap() {
  if (timer.isEnabled(StopwatchTimerMS)) {
    if (!StopwatchTimeLapBest || StopwatchTimeLapMilliSec < StopwatchTimeLapBest) {
      StopwatchTimeLapBest = StopwatchTimeLapMilliSec;
      Blynk.virtualWrite(7, StopwatchGetFormattedShort(StopwatchTimeLapMilliSec));
    }

    rowIndex++;
    Blynk.virtualWrite(6, "add", rowIndex, String("Lap ") + rowIndex, StopwatchGetFormattedShort(StopwatchTimeLapMilliSec));
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
    Blynk.virtualWrite(1, StopwatchGetFormatted(StopwatchTimeMasterMilliSec));
    Blynk.virtualWrite(1, StopwatchGetFormattedShort(StopwatchTimeLapMilliSec));
    Blynk.virtualWrite(6, "clr");
    Blynk.virtualWrite(4, 0);
    Blynk.virtualWrite(7, StopwatchGetFormattedShort(StopwatchTimeLapBest));
  }
}

String StopwatchGetFormattedShort(long milliSeconds) {
  long days = 0;
  long hours = 0;
  long mins = 0;
  long secs = 0;
  long ms = 0;

  String returned;

  String ms_o;
  String secs_o;
  String mins_o;
  String hours_o;

  ms = milliSeconds;
  secs = milliSeconds / 1000; //convert milliseconds to secs
  mins = secs / 60; //convert seconds to minutes
  hours = mins / 60; //convert minutes to hours
  days = hours / 24; //convert hours to days

  ms = milliSeconds - (secs * 1000); //subtract the coverted seconds to minutes in order to display 999 ms max
  secs = secs - (mins * 60); //subtract the coverted seconds to minutes in order to display 59 secs max
  mins = mins - (hours * 60); //subtract the coverted minutes to hours in order to display 59 minutes max
  hours = hours - (days * 24); //subtract the coverted hours to days in order to display 23 hours max

  returned = "";
  if (ms == 0) {
    ms_o = "00";
  } else if (ms < 100) {
    ms_o = "0";
  } else if (ms < 10) {
    ms_o = "00";
  }
  if (secs < 10) {
    secs_o = "0";
  }
  if (mins < 10) {
    mins_o = "0";
  }
  if (hours < 10) {
    hours_o = "0";
  }
  if (days) {
    returned += days + String(":");
  }
  if (hours) {
    returned += hours_o + hours + String(":");
  }
  if (mins) {
    returned += mins_o + mins + String(":");
  }
  returned += secs_o + secs + String(".");
  returned += ms_o + ms;
  return returned;
}

String StopwatchGetFormatted(long milliSeconds) {
  long days = 0;
  long hours = 0;
  long mins = 0;
  long secs = 0;
  long ms = 0;

  String returned;

  String ms_o = ".";
  String secs_o = ":";
  String mins_o = ":";
  String hours_o = ":";

  ms = milliSeconds;
  secs = milliSeconds / 1000; //convert milliseconds to secs
  mins = secs / 60; //convert seconds to minutes
  hours = mins / 60; //convert minutes to hours
  days = hours / 24; //convert hours to days

  ms = milliSeconds - (secs * 1000); //subtract the coverted seconds to minutes in order to display 999 ms max
  secs = secs - (mins * 60); //subtract the coverted seconds to minutes in order to display 59 secs max
  mins = mins - (hours * 60); //subtract the coverted minutes to hours in order to display 59 minutes max
  hours = hours - (days * 24); //subtract the coverted hours to days in order to display 23 hours max

  if (ms == 0) {
    ms_o = ".00";
  } else if (ms < 100) {
    ms_o = ".0";
  } else if (ms < 10) {
    ms_o = ".00";
  }
  if (secs < 10) {
    secs_o = ":0";
  }
  if (mins < 10) {
    mins_o = ":0";
  }
  if (hours < 10) {
    hours_o = ":0";
  }
  return days + hours_o + hours + mins_o + mins + secs_o + secs + ms_o + ms;

}

void loop() {
  Blynk.run();
#ifndef ESP32
  ArduinoOTA.handle();
#endif
  timer.run();
}
