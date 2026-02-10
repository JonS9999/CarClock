//
//  System include files :
//
#include <Arduino.h>
#include <stdarg.h>         // Required for variable arguments
#include <ESP8266WiFi.h>    // Wifi for ESP8266.
#include <ezTime.h>
#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h> // I2C expander i/o class header


//
//  Local include files :
//
#include "config.h"
#include "MyDebug.h"
#include "MyNetworkManager.h"

#if 0
#include "MyWebManager.h"
#endif


//
//  WiFi networks information :
//
tSystemConfig    config;


//
//  Initialize LCD (0x27 is common, some backpacks use 0x3F) :
//
const int LCD_I2C_ADDR  = 0x27;             // Address of the "I2C backpack" on the LCD.
const int LCD_I2C_SDA   = D2;
const int LCD_I2C_SCL   = D1;
const int LCD_COLS      = 20;               // 20 column display.
const int LCD_ROWS      = 4;                // 4 row display.

// Create the LCD object
hd44780_I2Cexp lcd;

Timezone myTZ;

//----------------------------------------------------------------------------

// --- 4-Line Custom Segments ---
// Kept in the segs[8][8] format as requested.
byte segs[8][8] =
{
  // 0x00 : Top bar - thick
  {0x1F,    // xxxxx
   0x1F,    // xxxxx
   0x1F,    // xxxxx
   0x1F,    // xxxxx
   0x00,    // .....
   0x00,    // .....
   0x00,    // .....
   0x00},   // .....

   // 0x01 : Bottom bar - thick
  {0x00,    // .....
   0x00,    // .....
   0x00,    // .....
   0x00,    // .....
   0x1F,    // xxxxx
   0x1F,    // xxxxx
   0x1F,    // xxxxx
   0x1F},   // xxxxx

   // 0x02 : Top half of colon (:) :  *** Not used ***
  {0x00,    // .....
   0x00,    // .....
   0x00,    // .....
   0x00,    // .....
   0x00,    // .....
   0x18,    // xx...
   0x18,    // xx...
   0x00},   // .....

   // 0x03 : Bottom half of colon (:) :  *** Not used ***
  {0x00,    // .....
   0x18,    // xx...
   0x18,    // xx...
   0x00,    // .....
   0x00,    // .....
   0x00,    // .....
   0x00,    // .....
   0x00},   // .....

   // 0x04 : *** Not used ***
  {0x00,    // .....
   0x00,    // .....
   0x00,    // .....
   0x00,    // .....
   0x1F,    // xxxxx
   0x1F,    // xxxxx
   0x1F,    // xxxxx
   0x1F},   // xxxxx

   // 0x05 : *** Not used ***
  {0x1F,    // xxxxx
   0x1F,    // xxxxx
   0x1F,    // xxxxx
   0x00,    // .....
   0x00,    // .....
   0x00,    // .....
   0x1F,    // xxxxx
   0x1F},   // xxxxx

   // 0x06 : *** Not used ***
  {0x00,    // .....
   0x00,    // .....
   0x1F,    // xxxxx
   0x1F,    // xxxxx
   0x1F,    // xxxxx
   0x1F,    // xxxxx
   0x00,    // .....
   0x00},   // .....

   // 0x07 : *** Not used ***
  {0x1F,
   0x1F,
   0x1F,
   0x1F,
   0x1F,
   0x1F,
   0x1F,
   0x1F}
};

// [Digit 0-9][Row 0-3][Col 0-3]
// 0xFF = Solid Block
// 0x20 = Space (32)
// 0x00-0x07 = Custom Segments (segs[0]-segs[7])
const byte jumboNums[10][4][4] =
{
  {{0xFF, 0x00, 0x00, 0xFF},    // 0 : Digit '0'.
   {0xFF, 0x20, 0x20, 0xFF},
   {0xFF, 0x20, 0x20, 0xFF},
   {0xFF, 0x01, 0x01, 0xFF}},

//  {{0x20, 0x20, 0xFF, 0x20},
  {{0x20, 0x01, 0xFF, 0x20},    // 1 : Digit '1'.
   {0x20, 0x20, 0xFF, 0x20},
   {0x20, 0x20, 0xFF, 0x20},
   {0x20, 0x20, 0xFF, 0x20}},

  {{0x00, 0x00, 0x00, 0xFF},    // 2 : Digit '2'.
   {0x01, 0x01, 0x01, 0xFF},
   {0xFF, 0x20, 0x20, 0x20},
   {0xFF, 0x01, 0x01, 0x01}},

  {{0x00, 0x00, 0x00, 0xFF},    // 3 : Digit '3'.
   {0x20, 0x01, 0x01, 0xFF},
   {0x20, 0x20, 0x20, 0xFF},
   {0x01, 0x01, 0x01, 0xFF}},

  {{0xFF, 0x20, 0x20, 0xFF},    // 4 : Digit '4'.
   {0xFF, 0x01, 0x01, 0xFF},
   {0x20, 0x20, 0x20, 0xFF},
   {0x20, 0x20, 0x20, 0xFF}},

  {{0xFF, 0x00, 0x00, 0x00},    // 5 : Digit '5'.
   {0xFF, 0x01, 0x01, 0x01},
   {0x20, 0x20, 0x20, 0xFF},
   {0x01, 0x01, 0x01, 0xFF}},

  {{0xFF, 0x00, 0x00, 0x00},    // 6 : Digit '6'.
   {0xFF, 0x01, 0x01, 0x01},
   {0xFF, 0x20, 0x20, 0xFF},
   {0xFF, 0x01, 0x01, 0xFF}},

  {{0x00, 0x00, 0x00, 0xFF},    // 7 : Digit '7'.
   {0x20, 0x20, 0x20, 0xFF},
   {0x20, 0x20, 0x20, 0xFF},
   {0x20, 0x20, 0x20, 0xFF}},

  {{0xFF, 0x00, 0x00, 0xFF},    // 8 : Digit '8'.
   {0xFF, 0x01, 0x01, 0xFF},
   {0xFF, 0x20, 0x20, 0xFF},
   {0xFF, 0x01, 0x01, 0xFF}},

  {{0xFF, 0x00, 0x00, 0xFF},    // 9 : Digit '9'.
   {0xFF, 0x01, 0x01, 0xFF},
   {0x20, 0x20, 0x20, 0xFF},
   {0x20, 0x20, 0x20, 0xFF}}
};

//----------------------------------------------------------------------------

void drawDigit ( int digit, int x )
{
  for (int row = 0; row < 4; row++)
  {
    lcd.setCursor ( x, row );

    for (int col = 0; col < 4; col++)
    {
      lcd.write ( (byte)jumboNums[digit][row][col] );
      delay ( 5 );
    }
  }
}

//----------------------------------------------------------------------------

void debugRoutine01 ( void )
{
    MyPrintf ( "--- debugRoutine01 called ---\n" );

    while ( 1 )
    {
        for (int i = 0; i <= 9; i++)
        {
            MyPrintf ( "- Digit %d.\n", i );

            lcd.clear();

            // Reference digit
            lcd.setCursor(10, 0);
            lcd.print(i);

            // Draw 4 copies
            drawDigit(i, 0);
            drawDigit(i, 5);

            // Spacer for colon will be at 10
            drawDigit(i, 11);
            drawDigit(i, 16);

            delay(1000);
        } // for

  #if 0
        for (int i = 0; i <= 9; i++)
        {
            lcd.clear();
            lcd.setCursor(0, 0);

            MyPrintf ( "- Digit %d.\n", i );

            //  Four copies of the big digit
            drawDigit ( i,  2 );    // Position 1
            drawDigit ( i,  6 );    // Position 2
            drawDigit ( i, 11 );    // Position 3
            drawDigit ( i, 15 );    // Position 4

#if 0   // Don't bother -- it messes up the big digits.
            //  Small reference digit in the corner
            lcd.setCursor(0, 0);
            lcd.print(i);
#endif

            //  Wait and loop
            delay ( 2000 );
        } // for
#endif

    } // while
}

//----------------------------------------------------------------------------

void debugRoutine02 ( void )
{
    //
    //  Local variables :
    //
    char        ch;
    // char        buf[100];
    bool        automaticMode   = true;
    bool        forceIt         = false;
    int         digit           = 0;
    int         oldDigit        = -1;
    long unsigned int  timeout  = 0;


    MyPrintf ( "--- debugRoutine02 called ---\n" );

    lcd.clear ();
    lcd.setCursor ( 0, 0 );
    delay ( 100 );
    lcd.print ( "+ debugRoutine02 +" );
    delay ( 3000 );
    lcd.clear ();

    while ( 1 )
    {
        //
        //  Do we have serial input ?
        //
        if ( Serial.available() > 0 )
        {
            ch = Serial.read();
            MyPrintf ( "- Read character [%c].\n", ch );

            switch ( ch )
            {
                case 'a' :
                    MyPrintf ( "- Enabling automatic mode.\n" );
                    automaticMode = true;
                    forceIt = true;
                    break;

                default :
                    if ( (ch >= '0') && (ch <= '9') )
                    {
                        MyPrintf ( "- Manual digit '%c'.\n", ch );
                        digit = ( ch - '0' );
                        automaticMode = false;
                        forceIt = true;
                    }
                    break;
            } // switch
        } // if


        //
        //  Are we supposed to display something ?
        //
        if ( (forceIt == true) || (oldDigit != digit) )
        {
            MyPrintf ( "%c Digit %d.\n",
                ( (automaticMode == true) ? '*' : '-' ),
                digit );

            // Reference digit
            lcd.clear();
            delay ( 10 );

            lcd.setCursor ( 0, 0 );
            delay ( 10 );

            // Draw 4 copies
#if 0   // Enable to display a small version of the number we are displaying.
            snprintf ( buf, sizeof(buf), "%d", digit );
            lcd.print ( buf );
            delay ( 100 );
#else
            drawDigit ( digit, 0 );
#endif
            drawDigit ( digit, 5 );

            // Spacer for colon will be at 10
            drawDigit ( digit, 11 );
            drawDigit ( digit, 16 );
        }

        //
        //  Update some things...
        //
        oldDigit = digit;
        forceIt  = false;

        //
        //  If we are in automatic mode, then :
        //      - Advance to the next digit.
        //      - Sleep for a bit.
        //
        if ( automaticMode == true )
        {
            digit = ( (digit + 1) % 10 );

            //
            //  Delay (sleep) for 1 second, but allow the keyboard
            //  to interrupt us :
            //
            timeout = ( millis() + 1000 );
            while ( (millis() <= timeout) && (Serial.available() == 0) )
            {
                delay ( 10 );
            }
        } // if

    } // while
}


//----------------------------------------------------------------------------
//
//  cb_TimeSyncEvent () -- Callback which is called by ezTime library.
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
//  setup ()
//
//----------------------------------------------------------------------------

void setup ( void )
{
    //
    //  Local variables :
    //
    int     status;


    Serial.begin(115200);
    delay ( 2000 );

    Serial.println ( "\n\n\n" );
    MyPrintf ( "=== %s ===\n", __FILE__ );
    MyPrintf ( "Built %s at %s.\n", __DATE__, __TIME__ );

    MyPrintf ( "LED_BUILTIN pin = %d.\n", LED_BUILTIN );
    MyPrintf ( "LCD I2C SCL pin = %d.\n", LCD_I2C_SCL );
    MyPrintf ( "LCD I2C SDA pin = %d.\n", LCD_I2C_SDA );
    MyPrintf ( "LCD I2C address = 0x%02x.\n", LCD_I2C_ADDR );


    //
    //  Status (heartbeat) LED :
    //
    pinMode ( LED_BUILTIN, OUTPUT );
    for ( int i = 0 ; i < 6 ; i++ )
    {
        digitalWrite ( LED_BUILTIN, HIGH );
        delay ( 100 );
        digitalWrite ( LED_BUILTIN, LOW );
        delay ( 100 );
    }


    //
    //  Initialize our system configuration :
    //
    memset ( &config, 0, sizeof(config) );


    //
    //  Initialize the LCD with 20 cols, 4 rows
    //  This function performs the auto-configuration that the Diag tool did
    //
    status = lcd.begin ( 20, 4 );

    if ( status )
    {
        // If status is non-zero, it couldn't find/config the LCD
        Serial.printf("LCD initialization failed with status: %d\n", status);
    }

    lcd.backlight();
    lcd.clear();

    // Load Custom Characters
    for ( int i = 0 ; i < 8 ; i++ )
    {
        lcd.createChar(i, segs[i]);
    }


#if 0  // Enable debug routine -- DEBUG HACK
//    debugRoutine01 ();
    debugRoutine02 ();
#endif  // Enable debug routine.


    lcd.setCursor ( 0, 0 );
    lcd.print ( "Initializing..." );


    //
    //  Setup the ezTime "event" callback :
    //
    // setEvent ( cb_TimeSyncEvent );
//    setSyncEvent ( cb_TimeSyncEvent );          // ezTime.cpp
//    setSyncProvider ( cb_TimeSyncEvent );       // ezTime.cpp


    //
    //  Load our secret information (WiFI SSIDs and passwords, etc) :
    //
    secretsSetup ();


    //
    //  Clear the LCD :
    //
    lcd.clear();
    delay ( 20 );


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
    char            c1;
    char            c2;
    int             h;
    int             m;
    int             curMin              = minute();
    int             curTzMin            = myTZ.minute();
    int             curTzHour           = myTZ.hour();
    uint32_t        curMillis           = millis();
    time_t          newNtpUpdateTime;
    time_t          oldNtpUpdateTime;
    static bool     s_ColonState        = false;
    static int      s_NumGetTimeRetries = 0;
    static int      s_NumWiFiRetries    = 0;
    static int      s_LastMin           = -1;
    static uint32_t s_LastMillis        = 0;
    static uint32_t s_NextWiFiTry       = 0;
    static uint32_t s_NextTimeFetch     = 0;


    events();               // exTime.cpp -- ezTime background tasks


    //---------------------------------------------------------------
    //
    //  Are we supposed to try to connect to a WiFi network ?
    //
    //---------------------------------------------------------------

    if ( (WiFi.status() != WL_CONNECTED) && (curMillis > s_NextWiFiTry) )
    {
        MyPrintf ( "[Loop]  Trying to connect to a WiFi network...\n" );

        lcd.clear ();
        delay ( 20 );

        //          "--------------------"
        lcd.setCursor ( 0, 0 );
        lcd.print ( "Attempting to" );
        delay ( 20 );

        lcd.setCursor ( 0, 1 );
        lcd.print ( "connect to WiFi..." );
        delay ( 20 );

        lcd.setCursor ( 0, 3 );
        lcd.print ( "Please wait..." );
        delay ( 20 );

        //
        //  Use our own semi-fancy WiFi manager :
        //
        success = connectWifi ();

        lcd.clear ();
        delay ( 20 );

        //
        //  If successful, then force us to get the time now :
        //
        if ( success == true )
        {
            MyPrintf ( "[Loop]  Connected.  We need to get the time...\n" );

            //
            //  Reset our count of the number of WiFi retries :
            //
            s_NumWiFiRetries = 0;

            //
            //  Display some stuff on the LCD display :
            //
            lcd.setCursor ( 0, 0 );
            lcd.print ( "Connected to WiFi:" );
            delay ( 20 );

            lcd.setCursor ( 0, 1 );
            lcd.print ( WiFi.SSID().c_str() );
            delay ( 20 );

            delay ( 1000 );

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

        lcd.clear ();
        delay ( 20 );

        lcd.setCursor ( 0, 0 );
        lcd.print ( "Connected to WiFi:" );
        delay ( 20 );

        lcd.setCursor ( 0, 1 );
        lcd.print ( WiFi.SSID().c_str() );
        delay ( 20 );

        lcd.setCursor ( 0, 3 );
        lcd.print ( "Getting the time..." );
        delay ( 20 );

        // waitForSync();                              // ezTime.cpp
//        waitForSync ( 15 );     // ezTime.cpp : Increase timeout to 15 seconds for cellular
        waitForSync ( 8 );     // ezTime.cpp : Increase timeout to 15 seconds for cellular

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
            }
        }
        else
        {
            MyPrintf ( "[Loop]  waitForSync() time request succeeded.\n" );
            s_NumGetTimeRetries = 0;
        }

        myTZ.setLocation ( "America/Chicago" );

        MyPrintf ( "[Loop]  Time synced: [%s]\n", myTZ.dateTime().c_str() );

        MyPrintf ( "[Loop]  Continuing...\n" );

        lcd.clear ();
        delay ( 20 );

        //
        //  Try to get the time again based on how many time we've
        //  tried to get the time :
        //
        //  Get the time in 10 seconds if we failed to get the time
        //  1 to 9 times :
        //
        if ( (s_NumGetTimeRetries > 0) && (s_NumGetTimeRetries < 10) )
        {
            MyPrintf ( "[Loop]  Will try to get the time again in %d seconds.\n", 60 );
            s_NextTimeFetch = ( curMillis + SECS_TO_MSECS(60) );      // Try to get the time every 60 seconds.
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

    if ( (forceDisplayUpdate == true) || (curMin != s_LastMin) )
    {
        lcd.clear ();
        delay ( 10 );

        s_LastMin = curMin;

        h = curTzHour;
        m = curTzMin;

        MyPrintf ( "  %s.\n", myTZ.dateTime("D, M j Y  g:i a").c_str() );

        // 12-hour format conversion
        if (h > 12) h -= 12;
        if (h == 0) h = 12;

        //
        //  Is the hour field is <= 9 ?
        //
        if ( h <= 9 )
        {
            //
            //  Only draw the 'ones' digit, and also move it
            //  to the left one column to give the blinking colon
            //  more space :
            //
            drawDigit ( (h % 10),  4 );
        }
        else
        {
            //
            //  Draw both the 'tens' digit and the 'ones' digit
            //  of the hour field :
            //
            drawDigit ( (h / 10),  0 );
            drawDigit ( (h % 10),  5 );
        }

        drawDigit ( (m / 10), 11 );
        drawDigit ( (m % 10), 16 );
    } // if

    //
    //  Flashing colon (runs every second) :
    //
    if ( (curMillis - s_LastMillis) >= 1000 )
    {
        s_LastMillis = curMillis;

        s_ColonState = !s_ColonState;

        //
        //  Decide which type of colon to use :
        //
        //      ' ' : No colon.
        //       *  : Colon on -- WiFi connected.
        //       !  : Colon on -- WiFi not connected.
        //
        c1 = ' ';       // Left part of colon.
        c2 = ' ';       // Right part of colon.

        if ( s_ColonState == true )
        {
            //
            //  We print something, so if we are connected, then
            //  we will print '*'.  If we are not connected, then
            //  we will print '!!' (two '!').
            //
            if ( WiFi.status() == WL_CONNECTED )
            {
                c1 = '*';
            }
            else
            {
                c1 = '!';
                c2 = '!';
            }
        }

        digitalWrite ( LED_BUILTIN, ( (c1 == ' ') ? HIGH : LOW ) );

        lcd.setCursor ( 9, 1 );
        lcd.print ( c1 );
        lcd.print ( c2 );

        lcd.setCursor ( 9, 2 );
        lcd.print ( c1 );
        lcd.print ( c2 );
    } // if
}
