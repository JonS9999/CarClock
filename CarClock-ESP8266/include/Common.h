#ifndef _COMMON_H_
#define _COMMON_H_

//
//  System include files :
//
#include <Arduino.h>
#include <ezTime.h>


//--------------------------------------------------------------
//
//  Use these things to enable/disable various features :
//
//--------------------------------------------------------------

//
//  Type of display we are using :
//
#define USE_DISPLAY_LCD2004


// #define DO_WEATHER


//--------------------------------------------------------------
//
//  A few project specific constants :
//
//--------------------------------------------------------------

#define     PROJECT_NAME    "CarClock"
//#define     PROJECT_NAME    "CarClock-ESP8266"


//--------------------------------------------------------------
//
//  Simple conversion routines :
//
//--------------------------------------------------------------

#define SECS_TO_MSECS(x)    ( (x) * 1000 )          // Convert x seconds into milliseconds.

#define MINS_TO_SECS(x)     ( (x) * 60 )            // Convert x mintues to seconds.
#define MINS_TO_MSECS(x)    ( SECS_TO_MSECS ( MINS_TO_SECS(x) ) )

#define HOURS_TO_MINS(x)    ( (x) * 60 )            // Convert x hours into minutes.
#define HOURS_TO_SECS(x)    ( MINS_TO_SECS ( HOURS_TO_MINS(x) ) )
#define HOURS_TO_MSECS(x)   ( SECS_TO_MSECS ( MINS_TO_SECS ( HOURS_TO_MINS(x) ) ) )


//--------------------------------------------------------------
//
//  Constants :
//
//--------------------------------------------------------------

#define MAX_WIFI_NETWORKS   (4)
#define BUTTON_AP_MODE      (0)     // Set this to your actual GPIO pin (e.g., 0 for Boot button)
#define IP_ADDR_SIZE        (17)
#define MAC_ADDR_SIZE       (18)


//--------------------------------------------------------------
//
//  Structures :
//
//--------------------------------------------------------------

typedef struct
{
    char ssid[32];
    char pass[32];
} tWiFiCredentials;

#ifdef  DO_WEATHER
typedef struct
{
    float temp;
    int humidity;
    char description[32];
    char icon[4];
    bool isValid;
} tWeatherData;
#endif  // DO_WEATHER

typedef struct
{
    tWiFiCredentials wifiSlots[MAX_WIFI_NETWORKS];
    bool isConnected;       // Are we connected to a WiFi network?
    char owmApiKey[33];     // OpenWeatherMap API Key
    char cityQuery[32];     // e.g., "Cedar Rapids,US"
    char timezone[32];      // Added: Stores Olson name or Posix string
    bool isAnalog;          // Are we displaying time as an analog clock or digital clock?
} tSystemConfig;


//--------------------------------------------------------------
//
//  Function prototypes contained in Common.cpp :
//
//--------------------------------------------------------------

extern void MyPrintf ( const char* format, ... );                   // Common.cpp
extern void MyPrintf ( const __FlashStringHelper* format, ... );    // Common.cpp

extern void SecretsSetup ( void );                                  // Secrets.cpp


//extern void saveConfigToFlash ( void );                             // ConfigStorage.cpp
//extern void loadConfigFromFlash ( void );                           // ConfigStorage.cpp

// Function to print time at the top of the minute :
extern void MyDebug_MinuteReport ( bool ForceUpdate );

// Function to simulate an "AP button" press after a specific delay :
extern bool MyDebug_IsApButtonPressed(void);

// Function to check the serial port for debugging commands :
extern void MyDebug_SerialCheck(void);


//--------------------------------------------------------------
//
//  Global files defined elsewhere :
//
//--------------------------------------------------------------

extern bool             g_FirstTime;               // Used to indicate the first time we're going through the main loop.
extern tSystemConfig    config;
extern Timezone         myTZ;

#ifdef  DO_WEATHER
extern tWeatherData     currentWeather;
#endif  // DO_WEATHER


#endif // !_COMMON_H_
