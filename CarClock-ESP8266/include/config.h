#ifndef _CONFIG_H_
#define _CONFIG_H_

//
//  System include files :
//
#include <Arduino.h>
#include <ezTime.h>


//
//  Use these things to enable/disable various features :
//
// #define DO_WEATHER


//
//  Simple conversion routines :
//
#define SECS_TO_MSECS(x)    ( (x) * 1000 )          // Convert x seconds into milliseconds.

#define MINS_TO_SECS(x)     ( (x) * 60 )            // Convert x mintues to seconds.
#define MINS_TO_MSECS(x)    ( SECS_TO_MSECS ( MINS_TO_SECS(x) ) )

#define HOURS_TO_MINS(x)    ( (x) * 60 )            // Convert x hours into minutes.
#define HOURS_TO_SECS(x)    ( MINS_TO_SECS ( HOURS_TO_MINS(x) ) )
#define HOURS_TO_MSECS(x)   ( SECS_TO_MSECS ( MINS_TO_SECS ( HOURS_TO_MINS(x) ) ) )


//
//  Constants :
//
#define MAX_WIFI_NETWORKS 4
#define BUTTON_AP_MODE 0  // Set this to your actual GPIO pin (e.g., 0 for Boot button)


//
//  Structures :
//
typedef struct
{
    char ssid[32];
    char pass[32];
} tWiFiCredentials;

typedef struct
{
    float temp;
    int humidity;
    char description[32];
    char icon[4];
    bool isValid;
} tWeatherData;

typedef struct
{
    tWiFiCredentials wifiSlots[MAX_WIFI_NETWORKS];
    bool isConnected;       // Are we connected to a WiFi network?
    char owmApiKey[33];     // OpenWeatherMap API Key
    char cityQuery[32];     // e.g., "Cedar Rapids,US"
    char timezone[32];      // Added: Stores Olson name or Posix string
    bool isAnalog;          // Are we displaying time as an analog clock or digital clock?
} tSystemConfig;


//
//  Function prototypes for storage :
//
extern void MyPrintf(const char* format, ...);          // main.cpp
extern void saveConfigToFlash(void);                    // ConfigStorage.cpp
extern void loadConfigFromFlash(void);                  // ConfigStorage.cpp
extern void secretsSetup ( void );                      // secrets.cpp


//
//  Global files defined elsewhere :
//
extern tSystemConfig    config;
extern tWeatherData     currentWeather;
extern Timezone         myTZ;

#endif // !_CONFIG_H_
