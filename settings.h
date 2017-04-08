/**************************************************************

                            Settings

 **************************************************************/
/*
  ESP32 Board - Comment out for ESP8266
*/
//#define ESP32
 
#include "wifi_credentials.h" // see ReadMe
/*
     Blynk Auth Code
*/
#define AUTH                      "4c34e9c7f73d4fd684dbef6e57af127c"
/*
   Over The Air Hostname
*/
#define OTA_UPDATES
#define OTA_HOSTNAME              "STOPWATCHER"
/*
   Local Server Settings (uncomment to use local server)
*/
#define USE_LOCAL_SERVER
#define SERVER                      IPAddress(192, 168, 1, 2)
/*
   Virtual Pins - Base
*/
#define vPIN_VOLTAGE_REAL           V1
/*
  Debugging
*/
#define DEBUG


