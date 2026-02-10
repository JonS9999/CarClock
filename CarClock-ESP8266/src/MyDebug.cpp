//
//  System include files :
//
#include <Arduino.h>


//
//  Local include files :
//
#include "MyDebug.h"
#include "config.h"
// #include "MyNetworkManager.h"


static unsigned long lastMinuteReport = 0;
static bool virtualApButtonActive = false; // The internal "latched" state



void MyPrintf ( const char* format, ... )
{
    // 1. Create the timestamp prefix [mm/dd/yy hh:mm:ss]
    // ezTime format: "m/d/y H:i:s"
    // "g:i:s A" -> 3:15:02 PM (g is hour, i is minute, s is second, A is AM/PM)
    // If time isn't set yet, it will show [01/01/70 00:00:00]
    String timestamp = "[" + myTZ.dateTime("m/d/Y g:i:s A") + "]  ";
//    String timestamp = "[" + myTZ.dateTime("m/d/y H:i:s") + "]  ";

    // 2. Handle the variable arguments
    char buffer[256]; // Adjust size if you plan on printing very long strings
    va_list args;

    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    // 3. Output the formatted string
    Serial.print(timestamp);
    Serial.print(buffer);

    // Optional: Force a newline if you want every MyPrintf to be its own line
    // Serial.println();
}


void MyDebug_MinuteReport ( bool forceUpdate )
{
    if ( (forceUpdate == true) || ( (myTZ.second() == 0) && (millis() - lastMinuteReport > 2000) ) )
    {
        // myTZ.dateTime() formats the time according to the local offset
        // "H:i:s" is 24-hour; use "g:i:s a" for 12-hour with AM/PM
        MyPrintf ( "[Debug:TimeReport]  %s\n", myTZ.dateTime("g:i:s a").c_str() );
//        Serial.printf("[Debug:TimeReport]  %s\n", myTZ.dateTime("H:i:s").c_str());

        lastMinuteReport = millis();
    }
}


bool MyDebug_IsApButtonPressed(void)
{
    //
    //  Local variables :
    //
    bool returnValue    = false;

    if ( virtualApButtonActive == true )
    {
        MyPrintf ( "[Debug:IsApButtonPressed]  Detected simulated button press.\n" );
        virtualApButtonActive = false;  // Reset the "latch" after it's been read
        returnValue = true;
    }

    return ( returnValue );
}


void MyDebug_SerialCheck(void)
{
    if (Serial.available() > 0)
    {
        char cmd = Serial.read();

        switch (cmd) {
            case '?' :
            case 'h' :
            case 'H' :
                Serial.printf ( "\n" );
                MyPrintf ( "Available commands :\n" );
                MyPrintf ( "   a : Simulate pressing and releasing the 'AP Mode' button.\n" );
                MyPrintf ( "   c : Display configuration (stored SSIDs, etc).\n" );
                MyPrintf ( "   i : Display WiFi information.\n" );
                MyPrintf ( "   R : Reboot the system.\n" );
                MyPrintf ( "   w : Show available WiFi networks.\n" );
                Serial.printf ( "\n" );
                break;

            case 'a':
            case 'A':
                MyPrintf ( "[Debug:Serial]  Simulating AP-Mode button press...\n" );
                virtualApButtonActive = true;
                break;

            case 'c': // New: Show Current Config
                MyPrintf ( "[Debug:Serial]  Displaying stored SSIDs...\n" );
//                displayStoredSSIDs();
                break;

            case 'i':
            case 'I':
                MyPrintf ( "[Debug:Serial]  Displaying WiFi status...\n" );
//                displayWifiStatus();
                break;

//            case 'r':
            case 'R':
                MyPrintf ( "[Debug:Serial]  Rebooting...\n" );
                ESP.restart();
                break;

            case 'w':
            case 'W':
                MyPrintf ( "[Debug:Serial]  Displaying available WiFi networks...\n" );
//                displayVisibleNetworks();
                break;

            default :
                MyPrintf ( "[Debug:Serial]  *** Unknown command '%c' ***\n", cmd );
                break;
        } // switch
    } // if
}
