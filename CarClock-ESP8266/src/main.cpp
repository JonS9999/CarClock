//----------------------------------------------------------------------------
//
//  main.cpp
//
//      Version 1.2
//
//----------------------------------------------------------------------------
//
//  History :
//
//      Ver 1.3 : 2026/03/01
//          - Added code to search open WiFi networks.
//
//      Ver 1.2 : 2026/02/20
//          - Added SerialCommand library to be able to drive the clock
//            via the serial (USB) port -- Not doing anything with this yet.
//          - Changed some of the messages that are displayed on the LCD.
//          - Added an ASCII "screen" to MyDisplay_LCD2004 so we keep a copy
//            of what is displayed on the LCD.  This way we can test our code
//            without needing an actual LCD connected to the ESP8266.
//            Note that the ASCII "screen" can be displayed using normal
//            ASCII characters, or it can be displayed using VT100 graphics
//            characters -- the is controlled by the m_VT100_Emulation
//            attribute in the MyDisplay_LCD2004 object.
//
//      Ver 1.1 : 2026/02/15
//          - Added MyDisplay + MyDisplay_LCD support.
//          - Added MyRTC support for I2C RTC.
//          - Added OTA (Over The Air) programming support.
//          - Minor changes like renaming "secrets.cpp" to "Secrets.cpp",
//            displaying our IP address when we connect to a WiFi network,
//            etc...
//          - Still needs a code cleanup and some refactoring (but I wanted
//            to get this code checked in as a snapshot of a working clock).
//
//      Ver 1.0 : 2026/02/01
//          - Initial version.
//          - Simply clock with WiFi + NTP support.
//
//----------------------------------------------------------------------------
//
//      Hardware
//
//        - Wemos D1 Mini (https://www.wemos.cc/en/latest/d1/d1_mini.html)
//
//        - LCD2004 20x4 Hitachi-type LCD display.
//
//        - I2C devices on our I2C bus :
//              0x27 : LCD Backpack.
//              0x57 : AT24Cxxx EEPROM series (on RTC board).
//              0x68 : DS3231 RTC.
//
//----------------------------------------------------------------------------
//
//      Future :
//
//        - Weather current + forecast information (e.g., pull down weather
//          info when we connect to a WiFi network).
//
//        - MQTT client to display messages.  This could be useful if I use
//          the clock as a stationary clock in my house -- for example, it
//          could display 3D printer status messages (M117) when I'm running
//          my 3D printer.  Or when someone comes to the door and rings the
//          doorbell (i.e, integrate with my home automation setup).
//
//        - Display OTA updating progress on the LCD so we can see what is
//          happening.
//
//        - Maybe a speaker or piezo electric element so it can alert when
//          the weather turns bad or someone rings the doorbell or when we
//          successfully connect to a WiFi network and get the time or...
//
//        - Possibly configure networks (that we search for) at runtime
//          instead of at compile time (e.g., get rid of Secrets.cpp).
//          Maybe use a web interface (WebIotConf perhaps?).
//
//        - Possibly attempt to connect to an open network if we cannot
//          connect to any network in our list of networks (this might
//          be useful when pulling into a truck stop or some place where
//          they have free WiFi).
//
//        - Push button for control, for example :
//              - Cycling through screens.
//              - Display weather current conditions + forecast.
//              - WiFi information (IP address, subnet mask, etc).
//              - Force rebooting (if button is held for a long time).
//              - ...many more things...
//
//----------------------------------------------------------------------------
//
//      Notes for me :
//
//          - Code cleanup.
//
//          - Remove unnecessary MyPrintf() lines.
//
//          - Refine/Cleanup the m_Display.DisplayMessage3() call.  Try to
//            make this into a more generic routine.
//
//          - Use callbacks (cb_*) for various things.
//
//          - Register c_WiFi object with other objects (e.g., pass a
//            pointer or reference to the object into the setup() routine
//            of objects that need access to WiFi stuff).  This makes things
//            less tied-together than they are now (e.g., I could remove the
//            MyWiFi.* stuff, and everything else would still work).
//
//          - Be more creative with the colon?  For example:
//                  - o : (Big dot) Normal operation / On WiFi.
//                  - ! : Normal operation / Not on WiFi.
//                  - * : (Snowflake) Weather warning.
//
//          - Implement the things listed in the section above.
//
//      Stuff dealing with the 3D printed case :
//          - Include case STLs in the github repo.
//          - Modify case to have a cable-hole in the back part of the case.
//          - Maybe put in one or more holes for push buttons.
//
//----------------------------------------------------------------------------


//
//  System include files :
//
#include <Arduino.h>
#include <stdarg.h>         // Required for variable arguments
#include <ESP8266WiFi.h>    // Wifi for ESP8266.
#include <ezTime.h>         // Timezone stuff.      *** Note that the ezTime library generates some warnings ***
#include <SerialCommand.h>  // For reading commands from the serial/USB port.


//
//  Local include files :
//
#include "Common.h"
#include "MyDisplay.h"              // Display driver (base class).
#include "MyRTC.h"                  // Real Time Clock (RTC) stuff.
#include "MyWiFi.h"                 // WiFi/Network stuff.
#include "MyWiFi_OTA.h"             // WiFi OTA (Over The Air) programming support.
#include "Secrets.h"                // Where our personal (secret) stuff is defined.


//
//  Things we may want to implement some day :
//
#if 0
#include "MyDisplay_TFT"            // Do we want to support a TFT display?
#include "MyWeather.h"              // Stuff to pull current and future weather information.
#include "MyWebManager.h"           // Maybe someday implement this...
#include "MyWiFi_MQTT"              // MQTT client support.
#endif


//
//  Single instances of some objects :
//
//SimpleTimer     g_Timer;        // Timer for sending messages to the MQTT broker.
cMyDisplay      g_Display;      // One instance of the display object.
cMyRTC          g_RTC;          // One instance of the Real Time Clock (RTC) object.
cMyWiFi         g_WiFi;         // One instance of the WiFi object.
SerialCommand   g_SerialCmd;    // Serial port command line interface.


//
//  WiFi networks information :
//
tSystemConfig    config;


//
//  Used to determine timezone and thus calculate the correct local time :
//
Timezone myTZ;


//----------------------------------------------------------------------------
//
//  cb_TimeSyncEvent () -- Callback which is called by ezTime library.
//
//      *** NOT CURRENTLY USED ***
//
//----------------------------------------------------------------------------

void cb_TimeSyncEvent ()
{
    //
    //  Local variables :
    //
    timeStatus_t status = timeStatus();     // timeStatus() returns 0, 1, or 2 (timeNotSet, timeNeedsSync, timeSet)


    switch ( status )
    {
        case timeSet :          // Value: 2
            MyPrintf ( "[cb_TimeSyncEvent]  NTP Sync Successful!\n" );
            break;

        case timeNeedsSync :    // Value: 1
            MyPrintf ( "[cb_TimeSyncEvent]  NTP Sync failed (using old/expired time).\n" );
            break;

        case timeNotSet :       // Value: 0
            MyPrintf ( "[cb_TimeSyncEvent]  NTP Sync failed (time never set).\n" );
            break;

        default:
            MyPrintf ( "[cb_TimeSyncEvent]  NTP Sync status: %d.\n", status );
            break;
    } // switch
}


//----------------------------------------------------------------------------
//
//  cb_WiFi_Connecting () -- WiFi callback which is called when the WiFi
//                           module attempts to connect to an access point.
//
//----------------------------------------------------------------------------

void cb_WiFi_Connecting (   const int   index,              // Index into WiFi.RSSI(index).
                            const char* ssid )
{
    //
    //  Local variables :
    //
    char    buf[80];                                // Temporary text buffer.


    //
    //  Create the string to display :
    //
    //                          "--------------------"
    //                          "Trying open WiFi :",
    //                          "Trying secure WiFi :",
    snprintf ( buf, sizeof(buf), "Trying %s WiFi :",
        ( (WiFi.encryptionType(index) == ENC_TYPE_NONE) ? "open" : "secure") );

    MyPrintf ( "[cb_WiFi_Connecting]  ------------------------------------------\n" );
    MyPrintf ( "[cb_WiFi_Connecting]  Called : Idx # %d / AP = [%s].\n", index, ssid );


    //                          "--------------------"
    g_Display.DisplayMessage3 ( buf,                        // Row 0 (see above).
                                "",                         // Row 1.
                                ssid,                       // Row 2 (SSID).
                                false );                    // Do not force SSID to the bottom row.
}


//----------------------------------------------------------------------------
//
//  cb_WiFi_NoUsableNetwork () -- WiFi callback which is called when
//              the WiFi module is not able to connect to any network.
//
//      Note that we only display something the first time we are called.
//      This is so that once we're running and searching for networks, when
//      we don't find one, we just return as quickly as possible (to get
//      back to displaying the time on our display).
//
//----------------------------------------------------------------------------

void cb_WiFi_NoUsableNetwork ( void )
{
    //
    //  Local variables :
    //
    static bool     s_UpdateDisplay     = true;         // Used to track if we should update the display.


    //
    //  Always display the following :
    //
    MyPrintf ( "[cb_WiFi_NoUsableNetwork]  ------------------------------------------\n" );
    MyPrintf ( "[cb_WiFi_NoUsableNetwork]  Called : Update display = %s.\n", ( (s_UpdateDisplay == true) ? "TRUE" : "FALSE") );


    //
    //  Should we display a message on our display ?
    //
    if ( s_UpdateDisplay == true )
    {
        MyPrintf ( "[cb_WiFi_NoUsableNetwork]  Displaying stuff on the display just this one time.\n" );

        //                          "--------------------"
        g_Display.DisplayMessage3 ( "No usable networks",
                                    "found.",
                                    "" );

        delay ( 2000 );

        s_UpdateDisplay = false;
    }
}


//----------------------------------------------------------------------------
//
//  cb_WiFi_ScanningForNetworks () -- WiFi callback which is called
//              when the WiFi module is about to scan for networks.
//
//----------------------------------------------------------------------------

void cb_WiFi_ScanningForNetworks ( void )
{
    MyPrintf ( "[cb_WiFi_ScanningForNetworks]  ------------------------------------------\n" );
    MyPrintf ( "[cb_WiFi_ScanningForNetworks]  Called.\n" );

    //                          "--------------------"
    g_Display.DisplayMessage3 ( "Scanning for WiFi",
                                "networks.",
                                "Please wait..." );

    delay ( 2000 );
}


//----------------------------------------------------------------------------
//
//  cb_WiFi_SearchingForOpenNetwork () -- WiFi callback which is called
//              when the WiFi module searches for an open network.
//
//----------------------------------------------------------------------------

void cb_WiFi_SearchingForOpenNetwork ( void )
{
    MyPrintf ( "[cb_WiFi_SearchingForOpen]  ------------------------------------------\n" );
    MyPrintf ( "[cb_WiFi_SearchingForOpen]  Called.\n" );

    //                          "--------------------"
    g_Display.DisplayMessage3 ( "Searching for an",
                                "open network.",
                                "Please wait..." );

    delay ( 2000 );
}


//----------------------------------------------------------------------------
//
//  cb_WiFi_SearchingForPreferredNetwork () -- WiFi callback which is called
//              when the WiFi module searches for a preferred network.
//
//----------------------------------------------------------------------------

void cb_WiFi_SearchingForPreferredNetwork ( void )
{
    MyPrintf ( "[cb_WiFi_SearchingForPreferred]  ------------------------------------------\n" );
    MyPrintf ( "[cb_WiFi_SearchingForPreferred]  Called.\n" );

    //                          "--------------------"
    g_Display.DisplayMessage3 ( "Searching for a",
                                "preferred network.",
                                "Please wait..." );

    delay ( 2000 );
}


//----------------------------------------------------------------------------
//
//  setup ()
//
//----------------------------------------------------------------------------

void setup ( void )
{
    //
    //  Local variables :
    //
    //char    buf[30];


    Serial.begin ( 115200 );
    delay ( 2000 );

    Serial.println ( "\n\n\n" );
    MyPrintf ( "=== %s ===\n", __FILE__ );
    MyPrintf ( "Built %s at %s.\n", __DATE__, __TIME__ );


    //
    //  Status (heartbeat) LED :
    //
    pinMode ( LED_BUILTIN, OUTPUT );

#if 1   // DEBUG HACK
    //
    //  Let's blink the built-in LED a few times to show that
    //  we are alive :
    //
    for ( int i = 0 ; i < 3 ; i++ )
    {
        digitalWrite ( LED_BUILTIN, HIGH );
        delay ( 100 );
        digitalWrite ( LED_BUILTIN, LOW );
        delay ( 100 );
    }
#endif


    //
    //  Initialize our display object :
    //
    g_Display.setup ();
    g_Display.SetObjectSerialCmd ( &g_SerialCmd );
    g_Display.DisplaySplashScreen ();


    //
    //  Initialize our RTC (Real Time Clock) object :
    //
    g_RTC.setup ();


    //
    //  Were we successful in initializing the RTC ?
    //
    if ( g_RTC.IsInitialized() == true )
    {
        //
        //  Set our system time from the time stored in the RTC :
        //
        g_RTC.SetSystemTimeFromRTC ();
    }
    else
    {
        MyPrintf ( "[setup]  *** Unable to initialize the RTC -- Ignoring future RTC operations ***\n" );

        //                          "--------------------"
        g_Display.DisplayMessage3 ( g_WiFi.MyHostname(),
                                    "",
                                    "* Cannot find RTC *",
                                    false );

        delay ( 2000 );
    }


    //
    //  Initialize our system configuration :
    //
    memset ( &config, 0, sizeof(config) );


    //
    //  Initialize our WiFi object :
    //
    g_WiFi.setup ();
    g_WiFi.SetCallback_Connecting                   ( cb_WiFi_Connecting );
    g_WiFi.SetCallback_NoUsableNetwork              ( cb_WiFi_NoUsableNetwork );
    g_WiFi.SetCallback_ScanningForNetworks          ( cb_WiFi_ScanningForNetworks );
    //g_WiFi.SetCallback_SearchingForOpenNetwork      ( cb_WiFi_SearchingForOpenNetwork );
    //g_WiFi.SetCallback_SearchingForPreferredNetwork ( cb_WiFi_SearchingForPreferredNetwork );


    //
    //  Load our personal info (WiFI SSIDs and passwords) from Secrets.cpp
    //  and also setup our default timezone :
    //
    SecretsSetup ();

    MyPrintf ( "[setup]  Defaulting to timezone '%s'.\n", MY_TIMEZONE );
    myTZ.setPosix ( MY_TIMEZONE );              // Defined in Secrets.cpp


    //
    //  Clear the display :
    //
    g_Display.ClearScreen ();


#if 0   // RTC stuff -- DEBUG HACK
    g_RTC.Debug_Test_001 ();
#endif


    MyPrintf ( "Running...\n" );
    MyPrintf ( "-----------------------------\n" );
}

//----------------------------------------------------------------------------

void loop ( void )
{
    //
    //  Local variables :
    //
    bool            success;
    bool            forceDisplayUpdate  = false;
    bool            displayWiFiInfo     = false;            // Set to 'true' if we connect to a new WiFi network.
    int             h;
    int             m;
    int             curMin              = minute();
    int             curTzHour           = 0;
    int             curTzMin            = 0;
    int             curTzSec            = 0;
    uint32_t        curMillis           = millis();
    time_t          newNtpUpdateTime;
    time_t          oldNtpUpdateTime;
    static int      s_NumGetTimeRetries = 0;
    static int      s_NumWiFiRetries    = 0;
    static int      s_LastMin           = -1;
    static uint32_t s_LastMillis        = 0;
    static uint32_t s_NextWiFiTry       = 0;
    static uint32_t s_NextTimeFetch     = 0;
    static uint32_t s_NextRtcSetTime    = 0;
    static String   s_LastWiFiIPAddr    = "";
    static String   s_LastWiFiSSID      = "";


    //
    //  Is this the first time we're being called ?
    //
    if ( g_FirstTime == true )
    {
        MyPrintf ( "[Loop]  =========================================\n" );
        MyPrintf ( "[Loop]  Main loop is running.\n" );
    }


    //
    //  Handle events and such :
    //
    events();                       // ezTime.cpp -- ezTime background tasks
    g_Display.handle ();            // Display driver.
    g_RTC.handle ();                // RTC (Real Time Clock) driver.
    g_WiFi.handle ();               // WiFi driver.


    //---------------------------------------------------------------
    //
    //  Are we supposed to try to connect to a WiFi network ?
    //
    //---------------------------------------------------------------

    if ( (WiFi.status() != WL_CONNECTED) && (curMillis > s_NextWiFiTry) )
    {
        MyPrintf ( "[Loop]  Trying to connect to a WiFi network...\n" );

        //
        //  Use our own semi-fancy WiFi manager :
        //
        success = g_WiFi.Connect ();

        g_Display.ClearScreen ();

        //
        //  If successful, then force us to get the time now :
        //
        if ( success == true )
        {
            //
            //  Determine if we've connected to a new WiFi network :
            //
            if ( (s_LastWiFiIPAddr != WiFi.localIP().toString() ) ||
                 (s_LastWiFiSSID   != WiFi.SSID()) )
            {
                s_LastWiFiIPAddr = WiFi.localIP().toString();
                s_LastWiFiSSID   = WiFi.SSID();

                MyPrintf ( "[Loop]  Connected to a new WiFi network [%s] (%s).\n",
                    s_LastWiFiSSID.c_str(),
                    s_LastWiFiIPAddr.c_str() );

                //
                //  Set the flag to show the extended WiFi info :
                //
                displayWiFiInfo = true;
            }

            MyPrintf ( "[Loop]  Connected.  We need to get the time...\n" );

            //
            //  Reset our count of the number of WiFi retries :
            //
            s_NumWiFiRetries = 0;

            //
            //  Should we show the extended WiFi information ?
            //
            if ( displayWiFiInfo == true )
            {
                //
                //  Display some WiFi information on the LCD display :
                //
                //                          "--------------------"
                g_Display.DisplayMessage3 ( "Connected to WiFi :",
                                            WiFi.SSID().c_str(),
                                            WiFi.localIP().toString().c_str() );

                delay ( 4000 );
            }

            //
            //  We can set s_NextWiFiTry to 0 so if our current WiFi
            //  connection drops, we immediately try to reconnect :
            //
            s_NextWiFiTry = 0;

            //
            //  Force us to fetch the time right away :
            //
            s_NextTimeFetch = 0;
        }
        else
        {
            MyPrintf ( "[Loop]  WiFi connection failed.\n" );

            //
            //  Clear the "last WiFi connectd to" info :
            //
            s_LastWiFiIPAddr = "";
            s_LastWiFiSSID   = "";

            //
            //  Increment the number of WiFi connect retries :
            //
            s_NumWiFiRetries++;

            //
            //  Try to connect again based on how many time we've
            //  tried to connect :
            //
            if ( s_NumWiFiRetries < 10 )
            {
                s_NextWiFiTry = ( curMillis + MINS_TO_MSECS(1) );       // Try in 1 minute.
            }
            else
            {
                s_NextWiFiTry = ( curMillis + MINS_TO_MSECS(30) );      // Try in 30 minutes.
            }
        }

        //
        //  Make sure we update the LCD :
        //
        forceDisplayUpdate = true;
    }


    //---------------------------------------------------------------
    //
    //  Is it time to get the time ?
    //
    //---------------------------------------------------------------

    if ( (WiFi.status() == WL_CONNECTED) && (curMillis > s_NextTimeFetch) )
    {
        MyPrintf ( "[Loop]  Trying to get the time...\n" );

        //                          "--------------------"
        g_Display.DisplayMessage3 ( "Connected to WiFi :",
                                     WiFi.SSID().c_str(),
                                     "Getting the time..." );


        // waitForSync();                              // ezTime.cpp
//        waitForSync ( 15 );     // ezTime.cpp : Increase timeout to 15 seconds for cellular
        waitForSync ( 8 );     // ezTime.cpp : Increase timeout to 8 seconds for cellular

        if ( timeStatus() != timeSet )
        {
            MyPrintf ( "[Loop]  NTP Sync Failed - trying using updateNTP()...\n" );
            oldNtpUpdateTime = lastNtpUpdateTime();
            MyPrintf ( "[Loop]  Last NTP date was at %d.\n", (int)oldNtpUpdateTime );

            updateNTP();

            newNtpUpdateTime = lastNtpUpdateTime();
            MyPrintf ( "[Loop]  New  NTP date was at %d.\n", (int)newNtpUpdateTime );

            if ( oldNtpUpdateTime == newNtpUpdateTime )
            {
                MyPrintf ( "[Loop]  Oops, it looks like our NTP request failed.\n" );
                s_NumGetTimeRetries++;
                MyPrintf ( "[Loop]  Number of NTP failed tries = %d.\n", s_NumGetTimeRetries );
            }
            else
            {
                MyPrintf ( "[Loop]  Hey, it looks like our NTP request succeeded!\n" );
                s_NumGetTimeRetries = 0;

                //
                //  Wait a few seconds before we try to set the time in the
                //  RTC (we need to give NTP time to set our system clock) :
                //
                s_NextRtcSetTime = ( curMillis + 5000 );
            }
        }
        else
        {
            MyPrintf ( "[Loop]  waitForSync() time request succeeded.\n" );
            s_NumGetTimeRetries = 0;

            //
            //  Wait a few seconds before we try to set the time in the
            //  RTC (we need to give NTP time to set our system clock) :
            //
            s_NextRtcSetTime = ( curMillis + 5000 );
        }

        MyPrintf ( "[Loop]  Time synced: [%s]\n", myTZ.dateTime().c_str() );

        MyPrintf ( "[Loop]  Continuing...\n" );

        g_Display.ClearScreen ();

        //
        //  Try to get the time again based on how many time we've
        //  tried to get the time :
        //
        //  Get the time in 10 seconds if we failed to get the time
        //  1 to 9 times :
        //
        if ( (s_NumGetTimeRetries > 0) && (s_NumGetTimeRetries < 10) )
        {
            MyPrintf ( "[Loop]  Will try to get the time again in %d seconds.\n", 30 );
            s_NextTimeFetch = ( curMillis + SECS_TO_MSECS(30) );      // Try to get the time every 30 seconds.
        }
        else
        {
            MyPrintf ( "[Loop]  Will try to get the time again in %d minutes.\n", 65 );
            s_NextTimeFetch = ( curMillis + MINS_TO_MSECS(65) );      // Try to get the time every 65 minutes.
        }

        //
        //  Make sure we update the LCD :
        //
        forceDisplayUpdate = true;
    }


    //---------------------------------------------------------------
    //
    //  We only redraw the digits if the minute changes to save
    //  I2C bandwidth :
    //
    //---------------------------------------------------------------

    curTzHour   = myTZ.hour();
    curTzMin    = myTZ.minute();
    curTzSec    = myTZ.second();

    if ( (forceDisplayUpdate == true) || (curMin != s_LastMin) )
    {
        s_LastMin = curMin;

        //
        //  Do some sanity checking on the time fields -- we need to
        //  do this if we don't have an RTC connected to us :
        //
        curTzHour = ( curTzHour % 24 );         // 0..23.
        curTzMin  = ( curTzMin  % 60 );         // 0..59.
        curTzSec  = ( curTzSec  % 60 );         // 0..59

        h = curTzHour;
        m = curTzMin;

        MyPrintf ( "[Loop]  Time = %s.\n", myTZ.dateTime("D, M j Y  g:i a").c_str() );

        // 12-hour format conversion
        if ( h > 12 ) h -= 12;
        if ( h == 0 ) h = 12;

        //
        //  Now display the time on our display :
        //
        g_Display.DisplayTime ( h, m, curTzSec );

    } // if


    //---------------------------------------------------------------
    //
    //  Should we update the time in the RTC ?
    //
    //---------------------------------------------------------------

    if ( g_RTC.IsInitialized() == true )
    {
        if ( (s_NextRtcSetTime != 0) && (curMillis >= s_NextRtcSetTime) )
        {
            MyPrintf ( "[Loop]  Time to set the time in the RTC...\n" );

            //
            //  Try to update the time in the RTC.  Note that if our system
            //  time is not valid (e.g., we haven't yet been able to get the
            //  current time from an NTP server), the SetRTCFromSystemTime()
            //  call will return 'false'.  In that event, we will try to set
            //  the RTC time again in 5 seconds :
            //
            if ( g_RTC.SetRTCFromSystemTime() == false )
            {
                MyPrintf ( "[Loop]  +++ Warning -- Unable to set RTC time / Will try again later +++\n" );

                //
                //  Try to set the time again in 5 seconds :
                //
                s_NextRtcSetTime = ( curMillis + 5000 );
            }
            else
            {
                MyPrintf ( "[Loop]  We have successfully set the RTC time.\n" );

                //
                //  Set this to 0 to indicate the RTC has been updated :
                //
                s_NextRtcSetTime = 0;
            }
        }
    }


    //---------------------------------------------------------------
    //
    //  Toggle the built-in LED every second :
    //
    //---------------------------------------------------------------

    if ( (curMillis - s_LastMillis) >= 1000 )
    {
        digitalWrite ( LED_BUILTIN, ( ( ( (curMillis / 1000) % 2)  == 0 ) ? HIGH : LOW ) );

        s_LastMillis = curMillis;
    }


    //---------------------------------------------------------------
    //
    //  Clear our global "first time" flag since we've run this loop once :
    //
    //---------------------------------------------------------------

    g_FirstTime = false;
}


//----------------------------------------------------------------------------
//
//  end of  main.cpp
//
//----------------------------------------------------------------------------
