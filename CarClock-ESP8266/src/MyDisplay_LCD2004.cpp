//----------------------------------------------------------------------------
//
//  MyDisplay_LCD2004.cpp -- Display driver for Hitachi 20x4 LCD display.
//
//      Version 1.3
//
//----------------------------------------------------------------------------
//
//      The "time display" screen looks like this :
//
//           01234567890123456789
//          +--------------------+
//        0 |.DD.DDDD...DDDD.DDDD|
//        1 |..D.DDDD.C.DDDD.DDDD|
//        2 |S.D.DDDD.c.DDDD.DDDD|
//        3 |s.D.DDDD...DDDD.DDDD|
//          +--------------------+
//
//      Where :
//
//          D = Digit (0-9).  Note that the left most digit is narrower
//              than the other three digits.  This is because the left
//              most digit will only be "1" if the time is between 10:00
//              and 12:59.
//
//          C = Colon (top part).
//
//          c = Colon (bottom part).
//
//          S = Seconds field (tens unit).
//
//          s = Seconds field (ones unit).
//
//----------------------------------------------------------------------------


//
//  *Must* include Common.h first :
//
#include "Common.h"

//
//  Are we using this type of display ?
//
#ifdef  USE_DISPLAY_LCD2004


//
//  System include files :
//
#include <Arduino.h>


//
//  Local include files :
//
#include "MyDisplay.h"
#include "MyDisplay_LCD2004.h"
#include "MyWiFi.h"                         // For cMyWiFi.


//----------------------------------------------------------------------------
//
//  One instance of the cDisplay_LCD2004 object :
//
//----------------------------------------------------------------------------

static cMyDisplay_LCD2004   s_Display_LCD2004;

cMyDisplay* g_Display       = &s_Display_LCD2004;


//----------------------------------------------------------------------------
//
//  External global variables :
//
//----------------------------------------------------------------------------

extern cMyWiFi  g_WiFi;                 // For g_WiFi.MyHostname() call.


//----------------------------------------------------------------------------
//
//  Constants
//
//----------------------------------------------------------------------------

//
//  Initialize LCD (0x27 is common, some backpacks use 0x3F) :
//
static const int LCD_I2C_ADDR      = 0x27;              // Address of the "I2C backpack" on the LCD.
static const int LCD_I2C_SCL_PIN   = D1;                // Pin D1.
static const int LCD_I2C_SDA_PIN   = D2;                // Pin D2.
static const int LCD_COLS          = LCD2004_COLS;      // 20 column display.
static const int LCD_ROWS          = LCD2004_ROWS;      // 4 row display.


//---------------------------------------------------------
//
//  The number of customizable segments on the LCD2004 :
//
//---------------------------------------------------------

const uint8_t   SEGS_SIZE           = 8;                // Size of SEGS[].  (0x00..0x07)
const uint8_t   SEG_BLOCK           = (SEGS_SIZE);      // Location in SEGS[] of the 'block' character.


//---------------------------------------------------------
//
//  SEGS[]
//
//  --- 4-Line Custom Segments ---
//
//  Kept in the SEGS[SEGS_SIZE][8] format as requested.
//
//---------------------------------------------------------

static const byte SEGS[SEGS_SIZE][8] =
{
  //
  //    0x00 : Top bar - thick :
  //
  {0x1F,    // xxxxx
   0x1F,    // xxxxx
   0x1F,    // xxxxx
   0x1F,    // xxxxx
   0x00,    // .....
   0x00,    // .....
   0x00,    // .....
   0x00},   // .....

  //
  //   0x01 : Bottom bar - thick :
  //
  {0x00,    // .....
   0x00,    // .....
   0x00,    // .....
   0x00,    // .....
   0x1F,    // xxxxx
   0x1F,    // xxxxx
   0x1F,    // xxxxx
   0x1F},   // xxxxx

  //
  //    0x02 : Backslash (need to do this since a normal backslash displays a Japanese Yen symbol).
  //
  {0x00,    // .....
   0x10,    // x....
   0x08,    // .x...
   0x04,    // ..x..
   0x02,    // ...x.
   0x01,    // ....x
   0x00,    // .....
   0x00},   // .....

  //
  //    0x03 : *** Not used ***
  //
  {0x00,    // .....
   0x18,    // xx...
   0x18,    // xx...
   0x00,    // .....
   0x00,    // .....
   0x00,    // .....
   0x00,    // .....
   0x00},   // .....

  //
  //    0x04 : *** Not used ***
  //
  {0x00,    // .....
   0x00,    // .....
   0x00,    // .....
   0x00,    // .....
   0x1F,    // xxxxx
   0x1F,    // xxxxx
   0x1F,    // xxxxx
   0x1F},   // xxxxx

  //
  //    0x05 : *** Not used ***
  //
  {0x1F,    // xxxxx
   0x1F,    // xxxxx
   0x1F,    // xxxxx
   0x00,    // .....
   0x00,    // .....
   0x00,    // .....
   0x1F,    // xxxxx
   0x1F},   // xxxxx

  //
  //    0x06 : *** Not used ***
  //
  {0x00,    // .....
   0x00,    // .....
   0x1F,    // xxxxx
   0x1F,    // xxxxx
   0x1F,    // xxxxx
   0x1F,    // xxxxx
   0x00,    // .....
   0x00},   // .....

  //
  //    0x07 : *** Not used ***
  //
  {0x1F,
   0x1F,
   0x1F,
   0x1F,
   0x1F,
   0x1F,
   0x1F,
   0x1F}
};


//---------------------------------------------------------
//
//  SEGS_ASCII[]
//
//  The following array is used for displaying ASCII
//  representations of the SEGS[] array.  This is our
//  feable attempt at emulating the LCD2004 programmable
//  characters (0x00..0x07, 0xFF) that are on the LCD :
//
//  Note that the array is (NUM_SEGS+1).  This is because
//  we the first NUM_SEGS are for the programmable chars
//  and the (NUM_SEG+1) position is for the 'block'
//  character.
//
//---------------------------------------------------------

static const byte SEGS_ASCII[SEGS_SIZE+1] =
{
    '-',    // Custom char 0x00 : Top bar - thick.
    '_',    // Custom char 0x01 : Bottom bar - thick
    '\\',   // Custom char 0x02 : Backslash.
    '-',    // Custom char 0x03 : (not used)
    '-',    // Custom char 0x04 : (not used)
    '-',    // Custom char 0x05 : (not used)
    '-',    // Custom char 0x06 : (not used)
    '-',    // Custom char 0x07 : (not used)
    '*',    // Solid block character (0xFF).
//    '|',    // Solid block character (0xFF).
//    '+',    // Solid block character (0xFF).
};


//---------------------------------------------------------
//
//  SEGS_VT100[]
//
//  The following array is used for displaying ASCII
//  representations of the SEGS[] array, however, this
//  array is our feable attempt at emulating the LCD2004
//  programmable characters (0x00..0x07, 0xFF) that are on
//  the LCD using VT100 characters.  This works, for
//  example, if we use TeraTerm to talk to the serial port
//  of the ESP8266 since TeraTerm can emulate a VT100.
//
//  Note that the array is (NUM_SEGS+1).  This is because
//  we the first NUM_SEGS are for the programmable chars
//  and the (NUM_SEG+1) position is for the 'block'
//  character.
//
//---------------------------------------------------------

static const byte SEGS_VT100[SEGS_SIZE+1] =
{
    0x6f,   // Custom char 0x00 : Horizontal line - scan 1.
    0x73,   // Custom char 0x01 : Horizontal line - scan 9.
    0x7e,   // Custom char 0x02 : Centered dot (not use)
    0x7e,   // Custom char 0x03 : Centered dot (not use)
    0x7e,   // Custom char 0x04 : Centered dot (not use)
    0x7e,   // Custom char 0x05 : Centered dot (not use)
    0x7e,   // Custom char 0x06 : Centered dot (not use)
    0x7e,   // Custom char 0x07 : Centered dot (not use)
    0x61,   // Solid block character : Checkerboard.
};


// [Digit 0-9][Row 0-3][Col 0-3]
// 0xFF = Solid Block
// 0x20 = Space (32)
// 0x00-0x07 = Custom Segments (SEGS[0]-SEGS[7])
static const byte jumboNums[11][4][4] =
{
  {{0xFF, 0x00, 0x00, 0xFF},    // 0 : Digit '0'.
   {0xFF, 0x20, 0x20, 0xFF},
   {0xFF, 0x20, 0x20, 0xFF},
   {0xFF, 0x01, 0x01, 0xFF}},

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
   {0x20, 0x20, 0x20, 0xFF}},

  {{0x01, 0xFF, 0x20, 0x20},    // 10 : Digit '1'.  This is really a 3x4 digit!  The last column is ignored.
   {0x20, 0xFF, 0x20, 0x20},
   {0x20, 0xFF, 0x20, 0x20},
   {0x20, 0xFF, 0x20, 0x20}}
};


//----------------------------------------------------------------------------
//
//  Constructor
//
//----------------------------------------------------------------------------

cMyDisplay_LCD2004::cMyDisplay_LCD2004 ( void )
{
    MyPrintf ( "< cMyDisplay_LCD2004 constructor called >\n" );


    //
    //    Initialize our attributes :
    //
    m_IsInitialized             = false;
    m_LcdIsPresent              = false;
    m_pSerialCmd                = nullptr;      // Pointer to optional SerialCmd object.
    m_CommandDelay              = DEFAULT_COMMAND_DELAY;    // Default delay when printing or moving the cursor.
    m_DisplayingTime            = false;
    m_Seconds                   = 0;            // We keep track of the seconds.
    m_SecondsUpdateAt           = 0;            // When we should update the m_Seconds field.
    m_TextScreen_Posi_Col       = 0;            // Start at column 0.
    m_TextScreen_Posi_Row       = 0;            // Start at row 0.
    m_VT100_Emulation           = false;        // Flag : Enable VT100 emulation mode?


    //
    //  Clear our text "screen" (this will initialize m_TextScreen[][], so we
    //  don't have to do it here in the constructor):
    //
    TextScreen_ClearScreen ();
}


//----------------------------------------------------------------------------
//
//  DisplaySeconds () -- Display the seconds on the LCD.
//
//----------------------------------------------------------------------------

void cMyDisplay_LCD2004::DisplaySeconds ( int seconds )
{
    //
    //  Local variables :
    //
    byte    chTens      = ' ';          // Tens position of the seconds.
    byte    chOnes      = ' ';          // Oness position of the seconds.
    uint8_t tens        = 0;            // Tens field of 'seconds'.
    uint8_t ones        = 0;            // Ones field of 'seconds'.


    //
    //  Determine what to use for the tens and ones characters :
    //
    tens = ( (uint8_t)seconds / 10 );
    ones = ( (uint8_t)seconds % 10 );

    chTens = ( (tens > 0) ? ('0' + tens) : ' ' );
    chOnes = ( '0' + ones );

//    MyPrintf ( "[MyDisplay_LCD2004::DisplaySeconds]  Called : Seconds = %d (%d %d) [%c %c].\n",
//        seconds, tens, ones, chTens, chOnes );


    //
    //  Now display the seconds in the bottom left corner of the LCD.
    //
    //  Note that we display them vertically, not horizontally.
    //
    SetCursor ( 2, 0 );
    Write ( chTens );

    SetCursor ( 3, 0 );
    Write ( chOnes );
}


//----------------------------------------------------------------------------
//
//  TextScreen_ClearScreen () -- Clear our text "screen".
//
//----------------------------------------------------------------------------

void cMyDisplay_LCD2004::TextScreen_ClearScreen ( void )
{
    MyPrintf ( "[MyDisplay_LCD2004::TextScreen_ClearScreen]  Called.\n" );


    //
    //  Clear out the screen buffer :
    //
    memset ( m_TextScreen, ' ', sizeof(m_TextScreen) );


    //
    //  Position our cursor at position 0x0 :
    //
    m_TextScreen_Posi_Col = 0;
    m_TextScreen_Posi_Row = 0;
}


//----------------------------------------------------------------------------
//
//  TextScreen_Display () -- Display our "screen" to the serial port.
//
//----------------------------------------------------------------------------

void cMyDisplay_LCD2004::TextScreen_Display ( void )
{
    MyPrintf ( "[MyDisplay_LCD2004::TextScreen_Display]  Called.\n" );


    //
    //  Display the top line of the box around the display :
    //
    MyPrintf ( "+" );
    for ( uint8_t col = 0 ; col < LCD2004_COLS ; col++ )
    {
        Serial.printf ( "-" );
    }
    Serial.printf ( "+\n" );


    //
    //  Display the contents of m_TextScreen[][] :
    //
    for ( uint8_t row = 0 ; row < LCD2004_ROWS ; row++ )
    {
        MyPrintf ( "|" );
        for ( uint8_t col = 0 ; col < LCD2004_COLS ; col++ )
        {
            TextScreen_DisplayChar ( m_TextScreen[row][col] );
        } // for (col)
        Serial.printf ( "|\n" );
    } // for (row)


    //
    //  Display the bottom line of the box around the display :
    //
    MyPrintf ( "+" );
    for ( uint8_t col = 0 ; col < LCD2004_COLS ; col++ )
    {
        Serial.printf ( "-" );
    }
    Serial.printf ( "+\n" );
}


//----------------------------------------------------------------------------
//
//  TextScreen_DisplayChar () -- Display a character (that is in our text
//                               "screen") to the serial port.
//
//      Note that we try to convert the LCD2004 programmable character
//      (0x00..0x07 and 0xFF) into VT100 graphics characters to give a
//      more realistic display of what is on the LCD.
//
//----------------------------------------------------------------------------

void cMyDisplay_LCD2004::TextScreen_DisplayChar ( const byte ch )
{
    //
    //  Local variables :
    //
    byte    dispCh      = ch;           // Character to display.


    //
    //  Decode the character :
    //
    //      0x00..0x07 = Programmable digits.
    //      0x20       = Space (' ').
    //      0xFF       = Block ('*').
    //      0x20..0x7E = Printable character (' '..'~').
    //
    if ( ( (ch >= 0x00) && (ch <= 0x07) ) || (ch == 0xFF) )     // Programmable character or block.
    {
        //
        //  Okay, it's not a normal printable character, so it needs
        //  special processing in order for us to display it on the
        //  serial port.  First, are we in VT100 emulation mode?
        //
        if ( m_VT100_Emulation == true )
        {
            //
            //  VT100 emulation mode is on, so do our best to
            //  emulate what is on the LCD.  We use our SEGS_VT100[]
            //  array which is laid out like this :
            //
            //      SEGS_VT100[0..7] = Custom segment 0x00..0x07.
            //      SEGS_VT100[8]    = Block character.
            //
            dispCh = SEGS_VT100[ ( (ch == 0xFF) ? SEG_BLOCK : ch )];

            //
            //  Note that we have to wrap dispCh in the VT100 escape
            //  strings that put the terminal into and out of the VT100
            //  graphics character mode :
            //
            Serial.printf ( "\033(0" );         // <ESC>(0
            Serial.printf ( "%c", dispCh );
            Serial.printf ( "\033(B" );         // <ESC>(B
        }
        else
        {
            //
            //  VT100 emulation mode is not on, so do our best to
            //  emulate what is on the LCD.  We use our SEGS_ASCII[]
            //  array which is laid out like this :
            //
            //      SEGS_ASCII[0..7] = Custom segment 0x00..0x07.
            //      SEGS_ASCII[8]    = Block character.
            //
            dispCh = SEGS_ASCII[ ( (ch == 0xFF) ? SEG_BLOCK : ch )];

            Serial.printf ( "%c", dispCh );

            // Serial.printf ( "  <-- Ch = 0x%02x [%c].\n", ch, dispCh );
        }
    }
    else
    {
        //
        //  Not a programmable character, so handle it natively :
        //
        Serial.printf ( "%c", ( isprint(ch) ? ch : '?' ) );
    }
}


//----------------------------------------------------------------------------
//
//  DrawDigit () -- Draw the specified digit on our LCD2004 display at
//                  the specified location.
//
//----------------------------------------------------------------------------

void cMyDisplay_LCD2004::DrawDigit ( const int digit, const int x, const int width )
{
  for (int row = 0; row < 4; row++)
  {
    SetCursor ( row, x );

    for (int col = 0; col < width; col++)
    {
      Write ( (byte)jumboNums[digit][row][col] );
    }
  }
}

//----------------------------------------------------------------------------

#if 0   // DEBUG HACK

void debugRoutine01 ( void )
{
    MyPrintf ( "--- debugRoutine01 called ---\n" );

    while ( 1 )
    {
        for (int i = 0; i <= 9; i++)
        {
            MyPrintf ( "- Digit %d.\n", i );

            ClearScreen ();

            // Reference digit
            SetCursor ( 0, 10 );
            Print ( i );

            // Draw 4 copies
            drawDigit ( i, 0 );
            drawDigit ( i, 5 );

            // Spacer for colon will be at 10
            drawDigit ( i, 11 );
            drawDigit ( i, 16 );

            delay ( 1000 );
        } // for

#if 0
        for (int i = 0; i <= 9; i++)
        {
            ClearScreen ();
            SetCursor ( 0, 0) ;

            MyPrintf ( "- Digit %d.\n", i );

            //  Four copies of the big digit
            drawDigit ( i,  2 );    // Position 1
            drawDigit ( i,  6 );    // Position 2
            drawDigit ( i, 11 );    // Position 3
            drawDigit ( i, 15 );    // Position 4

#if 0   // Don't bother -- it messes up the big digits.
            //  Small reference digit in the corner
            SetCursor ( 0, 0 );
            Print ( i );
#endif

            //  Wait and loop
            delay ( 2000 );
        } // for
#endif

    } // while
}

#endif  // DEBUG HACK

//----------------------------------------------------------------------------

#if 0   // DEBUG HACK

void debugRoutine02 ( void )
{
    //
    //  Local variables :
    //
    char        ch;
    bool        automaticMode   = true;
    bool        forceIt         = false;
    int         digit           = 0;
    int         oldDigit        = -1;
    long unsigned int  timeout  = 0;


    MyPrintf ( "--- debugRoutine02 called ---\n" );

    ClearScreen ();
    SetCursor ( 0, 0 );

    Print ( "+ debugRoutine02 +" );
    delay ( 3000 );

    ClearScreen ();

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
            ClearScreen ();

            SetCursor ( 0, 0 );

            // Draw 4 copies
#if 0   // Enable to display a small version of the number we are displaying.
            snprintf ( buf, sizeof(buf), "%d", digit );
            Print ( buf );
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

#endif  // DEBUG HACK


//----------------------------------------------------------------------------
//
//  ClearScreen () -- Routine to clear our LCD2004 display.
//
//      Note that we clear the m_DisplayingTime flag so that we do not
//      display the blinking colon.
//
//----------------------------------------------------------------------------

void cMyDisplay_LCD2004::ClearScreen ( void )
{
    MyPrintf ( "[Display_LCD2004::ClearScreen]  Called.\n" );


    //
    //  Call the low level driver to clear the LCD :
    //
    if ( m_LcdIsPresent == true )
    {
        m_LCD.clear ();
        delay ( 20 );
    }


    //
    //  Clear our text "screen" :
    //
    TextScreen_ClearScreen ();


    //
    //  Mark that we are not/no longer displaying the time :
    //
    m_DisplayingTime = false;
}


//----------------------------------------------------------------------------
//
//  DisplaySplashScreen () -- Display our splash (initial) LCD.
//
//----------------------------------------------------------------------------

void cMyDisplay_LCD2004::DisplaySplashScreen ( void )
{
    //
    //  Local variables :
    //
    char    buf[LCD_COLS+1];        // Text buffer (size of one row of LCD + null character).
    char    buf2[100];              // Bigger text buffer.
    int     buf2Cap;                // Used to cap the length of buf[].


    MyPrintf ( "[Display_LCD2004::DisplaySplashScreen]  Called.\n" );


    //----------------------------------------------------------
    //
    //  Splash screen # 1 :
    //
    //----------------------------------------------------------

    ClearScreen ();

    SetCursor ( 0, 0 );
    Print ( g_WiFi.MyHostname() );


    // TextScreen_Display ();          // DEBUG HACK


    //
    //  Note that buf[] is intentionally only the size of one row
    //  of the LCD, so we format our output string into buf2[]
    //  (which is much bigger than buf[]) :
    //
    snprintf ( buf2, sizeof(buf2), "%s @ %s", __DATE__, __TIME__ );


    //
    //  Shorten the message in buf2[] to be no longer than one line
    //  of the LCD :
    //
    buf2Cap = min ( sizeof(buf), sizeof(buf2) );
    buf2[buf2Cap-1] = '\0';


    //
    //  Remove any trailing colons (':') just to make the line
    //  look a little nicer :
    //
    while ( (strlen(buf2) > 0) && (buf2[strlen(buf2)-1] == ':') )
    {
        buf2[strlen(buf2)-1] = '\0';
    }


    //
    //  Now display the message :
    //
    SetCursor ( 2, 0 );
    Print ( buf2 );

    TextScreen_Display ();          // DEBUG HACK

    delay ( 2000 );


    //----------------------------------------------------------
    //
    //  Splash screen # 2 :
    //
    //----------------------------------------------------------

    ClearScreen ();

    SetCursor ( 0, 0 );
    Print ( g_WiFi.MyHostname() );

    SetCursor ( 2, 0 );
    Print ( "Initializing..." );

    TextScreen_Display ();          // DEBUG HACK

    delay ( 3000 );
}


//----------------------------------------------------------------------------
//
//  DisplayMessage2 () -- Display the user specified messages on our display.
//
//----------------------------------------------------------------------------

void cMyDisplay_LCD2004::DisplayMessage2 (  const char* MesgPart1,
                                            const char* MesgPart2 )
{
    //
    //  Local variables :
    //
    char    buf[LCD_COLS+1];                // LCD_COLS + one place for the NULL pointer.


    MyPrintf ( "[Display_LCD2004::DisplayMessage2]  Called.\n" );

    MyPrintf ( "[Display_LCD2004::DisplayMessage2]  + Full mesg 1 = [%s] (%d).\n",
        ( (MesgPart1 != nullptr) ? MesgPart1 : "" ),
        ( (MesgPart1 != nullptr) ? strlen(MesgPart1) : 0 ) );

    MyPrintf ( "[Display_LCD2004::DisplayMessage2]  + Full mesg 2 = [%s] (%d).\n",
        ( (MesgPart2 != nullptr) ? MesgPart2 : "" ),
        ( (MesgPart2 != nullptr) ? strlen(MesgPart2) : 0 ) );


    //
    //  Clear the LCD then display the messages :
    //
    ClearScreen ();


    //
    //  Start message # 1 in the top left corner of the LCD :
    //
    if ( MesgPart1 != nullptr )
    {
        SetCursor ( 0, 0 );
        Print ( MesgPart1 );
    }


    //
    //  Try to fit message # 2 on the screen.  If it's short, then put
    //  it on the bottom line, otherwise start it on the second from
    //  bottom line :
    //
    if ( MesgPart2 != nullptr )
    {
        //
        //  Get the first part of message # 2 regardless the size
        //  of the message :
        //
        strncpy ( buf, MesgPart2, sizeof(buf) );
        buf[sizeof(buf)-1] = '\0';

        //
        //  Will the message fit on the bottom line ?
        //
        if ( strlen(MesgPart2) <= LCD_COLS )
        {
            //
            //  The message can fit on the bottom line :
            //
            MyPrintf ( "[Display_LCD2004::DisplayMessage2]  + Normal mesg 2 = [%s].\n", buf );

            SetCursor ( 3, 0 );
            Print ( buf );
        }
        else
        {
            //
            //  The message is too big for the bottom line, so try
            //  to fit it into the bottom two lines.  Note that we
            //  already put the first part of the message in 'buf' :
            //
            SetCursor ( 2, 0 );
            Print ( buf );

            MyPrintf ( "[Display_LCD2004::DisplayMessage2]  + Mesg 2 part 1 = [%s] (%d).\n", buf, strlen(buf) );

            //
            //  Now put the rest of the message on the bottom line :
            //
            strncpy ( buf, &MesgPart2[LCD_COLS], sizeof(buf) );
            buf[sizeof(buf)-1] = '\0';

            MyPrintf ( "[Display_LCD2004::DisplayMessage2]  + Mesg 2 part 2 = [%s] (%d).\n", buf, strlen(buf) );

            SetCursor ( 3, 0 );
            Print ( buf );
        }
    }

    TextScreen_Display ();          // DEBUG HACK

}


//----------------------------------------------------------------------------
//
//  DisplayMessage3 () -- Display the user specified messages on our display.
//
//----------------------------------------------------------------------------

void cMyDisplay_LCD2004::DisplayMessage3 (  const char* MesgPart1,
                                            const char* MesgPart2,
                                            const char* MesgPart3,
                                            const bool  ForceToBottom )
{
    //
    //  Local variables :
    //
    char    buf[LCD_COLS+1];                // LCD_COLS + one place for the NULL pointer.
    uint8_t bottomRowNumber;                // Which line do we consider the bottom line ?


    MyPrintf ( "[Display_LCD2004::DisplayMessage3]  Called : Force to bottom = [%s].\n",
        ( (ForceToBottom == true) ? "TRUE" : "FALSE" ) );

    MyPrintf ( "[Display_LCD2004::DisplayMessage3]  + Mesg part 1 = [%s].\n",
        ( (MesgPart1 != nullptr) ? MesgPart1 : "" ) );

    MyPrintf ( "[Display_LCD2004::DisplayMessage3]  + Mesg part 2 = [%s].\n",
        ( (MesgPart2 != nullptr) ? MesgPart2 : "" ) );

    MyPrintf ( "[Display_LCD2004::DisplayMessage3]  + Mesg part 3 = [%s].\n",
        ( (MesgPart3 != nullptr) ? MesgPart3 : "" ) );


    //
    //  If the third message is short (less than one row), do we put
    //  it on the bottom line or the second bottom line ?
    //
    bottomRowNumber = ( (ForceToBottom == true) ? 3 : 2 );


    //
    //  Clear the LCD then display the messages :
    //
    ClearScreen ();


    //
    //  Start message # 1 on the first line of the LCD :
    //
    if ( MesgPart1 != nullptr )
    {
        SetCursor ( 0, 0 );
        Print ( MesgPart1 );
    }


    //
    //  Start message # 2 on the second line of the LCD :
    //
    if ( MesgPart2 != nullptr )
    {
        SetCursor ( 1, 0 );
        Print ( MesgPart2 );
    }


    //
    //  Try to fit message # 3 on the screen.  If it's short, then put
    //  it on the bottom line, otherwise start it on the second from
    //  bottom line :
    //
    if ( MesgPart3 != nullptr )
    {
        //
        //  Get the first part of message # 2 regardless the size
        //  of the message :
        //
        strncpy ( buf, MesgPart3, sizeof(buf) );
        buf[sizeof(buf)-1] = '\0';

        //
        //  Will the message fit on the bottom line ?
        //
        if ( strlen(MesgPart3) <= LCD_COLS )
        {
            //
            //  The message can fit on the bottom line :
            //
            MyPrintf ( "[Display_LCD2004::DisplayMessage3]  + Full mesg 3 = [%s].\n", buf );

            SetCursor ( bottomRowNumber, 0 );
            Print ( buf );
        }
        else
        {
            //
            //  The message is too big for the bottom line, so try
            //  to fit it into the bottom two lines.  Note that we
            //  already put the first part of the message in 'buf' :
            //
            SetCursor ( 2, 0 );
            Print ( buf );

            MyPrintf ( "[Display_LCD2004::DisplayMessage3]  + Mesg 3 part 1 = [%s].\n", buf );

            //
            //  Now put the rest of the message on the bottom line :
            //
            strncpy ( buf, &MesgPart3[LCD_COLS], sizeof(buf) );
            buf[sizeof(buf)-1] = '\0';

            MyPrintf ( "[Display_LCD2004::DisplayMessage3]  + Mesg 3 part 2 = [%s].\n", buf );

            SetCursor ( 3, 0 );
            Print ( buf );
        }
    }

    TextScreen_Display ();          // DEBUG HACK

}


//----------------------------------------------------------------------------
//
//  DisplayTime () -- Display the time on our LCD.
//
//      Note that we set the m_DisplayingTime flag so that we can
//      display the blinking colon.
//
//----------------------------------------------------------------------------

void cMyDisplay_LCD2004::DisplayTime ( const int hours, const int minutes, const int seconds )
{
    MyPrintf ( "[Display_LCD2004::DisplayTime]  Called : Hour = %d / Minute = %d / Sec = %d.\n",
        hours, minutes, seconds );


    //
    //  Save the new seconds as well as the time we need to update the
    //  seconds field on the LCD :
    //
    m_Seconds = seconds;
    m_SecondsUpdateAt = ( millis() + 1000 );


    //
    //  Start by clearing the entire LCD :
    //
    ClearScreen ();


    //
    //  If the time is greater than 9:00, then we want to
    //  display a narrow '1' (our 3x4 '1') in the first
    //  three columns :
    //
    if ( hours > 9 )
    {
        DrawDigit ( 10, 0, 3 );         // Hours : Tens unit (Use the thin 3x4 '1' which is jumboNums[10]).
    }

    DrawDigit ( (hours % 10),  4 );     // Hours : Ones unit.

    DrawDigit ( (minutes / 10), 11 );   // Minutes : Tens unit.
    DrawDigit ( (minutes % 10), 16 );   // Minutes : Ones unit.


    //
    //  Now display the seconds :
    //
    DisplaySeconds ( seconds );


    TextScreen_Display ();          // DEBUG HACK


    //
    //  Mark that we are now displaying the time :
    //
    m_DisplayingTime = true;
}


//----------------------------------------------------------------------------
//
//  Print () -- Display the user specified text on the LCD.
//
//----------------------------------------------------------------------------

void cMyDisplay_LCD2004::Print ( const char* text )
{
    //
    //  Local variables :
    //
    char*   ptr         = (char*)text;          // Pointer into text[].


    //
    //  If we don't have a valid string, return now :
    //
    if ( text == nullptr )
    {
        return;
    }


    //
    //  Send the string to the LCD :
    //
    if ( m_LcdIsPresent == true )
    {
        m_LCD.print ( text );
        delay ( 20 );
    }


    //
    //  Update our text "screen" :
    //
    while ( (*ptr != '\0') && (m_TextScreen_Posi_Col < LCD2004_COLS) )
    {
        m_TextScreen[m_TextScreen_Posi_Row][m_TextScreen_Posi_Col] = *ptr;
        m_TextScreen_Posi_Col++;
        ptr++;
    } // while
}


//----------------------------------------------------------------------------
//
//  Print () -- Display the user specified character on the LCD.
//
//----------------------------------------------------------------------------

void cMyDisplay_LCD2004::Print ( const uint8_t ch )
{
    //
    //  Output the single character to the LCD :
    //
    if ( m_LcdIsPresent == true )
    {
        // m_LCD.print ( (char)ch );
        m_LCD.write ( (char)ch );
        delay ( m_CommandDelay );
    }


    //
    //  Save the character to the text "screen" :
    //
    if ( m_TextScreen_Posi_Col < LCD2004_COLS )
    {
        m_TextScreen[m_TextScreen_Posi_Row][m_TextScreen_Posi_Col] = ch;
        m_TextScreen_Posi_Col++;
    }
}


//----------------------------------------------------------------------------
//
//  Printf () -- Display the variable parameter user specified text on
//               the LCD.
//
//----------------------------------------------------------------------------

void cMyDisplay_LCD2004::Printf ( const char* format, ... )
{
  //
  //  *** TO DO : NEED TO IMPLEMENT THIS ***
  //

  // m_Display.Printf ( text );
}


//----------------------------------------------------------------------------
//
//  SetCursor () -- Move the cursor to the specified location.
//
//      Note that the parameter order is "row, column", not "column, row".
//
//----------------------------------------------------------------------------

void cMyDisplay_LCD2004::SetCursor ( const uint16_t row, const uint16_t column )
{
    //
    //  Note that setCursor() is "column, row" while this routin is
    //  "row, column".  We do that to make things line up with our
    //  PrintAt ( row, text ) routine :
    //
    if ( m_LcdIsPresent == true )
    {
        m_LCD.setCursor ( column, row );
        delay ( 13 );
    }


    //
    //  Update the cursor position on our text "screen" :
    //
    m_TextScreen_Posi_Col = column;
    m_TextScreen_Posi_Row = row;
}


//----------------------------------------------------------------------------
//
//  SetObjectSerialCmd () -- Routine to specify optional SerialCommand object.
//
//----------------------------------------------------------------------------

void cMyDisplay_LCD2004::SetObjectSerialCmd ( SerialCommand* SerialCmd )
{
    MyPrintf ( "[MyDisplay_LCD2004::SetObjectSerialCmd]  Called : SerialCmd = 0x%p.\n", SerialCmd );

    //
    //  Save the pointer to a SerialCommand object :
    //
    m_pSerialCmd = SerialCmd;
}


//----------------------------------------------------------------------------
//
//  Write () -- Display the user specified character (byte) on the LCD.
//
//----------------------------------------------------------------------------

void cMyDisplay_LCD2004::Write ( const uint8_t ch )
{
    //
    //  Output the single character to the LCD :
    //
    if ( m_LcdIsPresent == true )
    {
        m_LCD.write ( ch );
        delay ( 20 );
    }


    //
    //  Save the character to the text "screen" :
    //
    if ( m_TextScreen_Posi_Col < LCD2004_COLS )
    {
        m_TextScreen[m_TextScreen_Posi_Row][m_TextScreen_Posi_Col] = ch;
        m_TextScreen_Posi_Col++;
    }
}


//----------------------------------------------------------------------------
//
//  Write () -- Display the user specified text on the LCD.
//
//----------------------------------------------------------------------------

void cMyDisplay_LCD2004::Write ( const char* text )
{
    //
    //  Local variables :
    //
    char*   ptr         = (char*)text;          // Pointer into text[].


    //
    //  If we don't have a valid string, return now :
    //
    if ( text == nullptr )
    {
        return;
    }


    //
    //  Send the string to the LCD :
    //
    if ( m_LcdIsPresent == true )
    {
        m_LCD.write ( text );
        delay ( 20 );
    }


    //
    //  Update our text "screen" :
    //
    while ( (*ptr != '\0') && (m_TextScreen_Posi_Col < LCD2004_COLS) )
    {
        m_TextScreen[m_TextScreen_Posi_Row][m_TextScreen_Posi_Col] = *ptr;
        m_TextScreen_Posi_Col++;
        ptr++;
    } // while
}



//----------------------------------------------------------------------------
//
//  cb_WiFi_ConnectingAttempt () -- Callback routine which is
//      called when we are trying to connect to a WiFi network.
//      We are passed in the maximum number of attempts that will
//      be made and the number of attempts made so far.
//
//----------------------------------------------------------------------------

void cMyDisplay_LCD2004::cb_WiFi_ConnectingAttempt (    const uint8_t   numAttemptsSoFar,
                                                        const uint8_t   maxAttempts )
{
    //
    //  Local variables :
    //
    static uint8_t  s_Index             = 0;        // Index into PROGRESS_CHAR[].

    static const uint8_t    PROGRESS_CHAR[]     = { '|', '/', '-', 0x02 };      // For backslash, use our programmable character 0x02 (see SEGS[] above).
//    static const uint8_t    PROGRESS_CHAR[]     = { '|', '/', '-', '\\' };    // FYI a backslash (\) displays a Japanese Yen symbol.

    static const uint8_t    NUM_PROGRESS_CHARS  = ( sizeof(PROGRESS_CHAR) / sizeof (char) );


    //
    //  If this is the first time we're being called, then want to reset
    //  our index :
    //
    if ( numAttemptsSoFar == 0 )
    {
        s_Index = 0;
    }


    //
    //  Display some debugging information :
    //
    MyPrintf ( "[cMyDisplay_LCD2004::cb_WiFi_ConnectingAttempt]  Called : Attempt # %u of %u / Index = %u / Char = [%c].\n",
                numAttemptsSoFar,
                maxAttempts,
                s_Index,
                ( isprint(PROGRESS_CHAR[s_Index])  ? (const char)PROGRESS_CHAR[s_Index] :   // Printable character?
                  ((PROGRESS_CHAR[s_Index] == 0x02) ? '\\' : '?') ) );                      // Backslash (programmable character 0x02)?


    //
    //  Each time we're called, we want to display an indicator in the
    //  bottom right corner of the LCD2004 :
    //
    SetCursor ( (LCD_ROWS - 1), (LCD_COLS - 1) );
    Write ( PROGRESS_CHAR[s_Index] );


    //
    //  Display the updated ASCII screen to the serial port :
    //
    TextScreen_Display ();


    //
    //  Increment our index so we
    //
    s_Index = ( (s_Index + 1) % NUM_PROGRESS_CHARS );
}


//----------------------------------------------------------------------------
//
//  cb_WiFi_NoUsableNetwork () -- WiFi callback which is called when
//              the WiFi module is not able to connect to any network.
//
//      Note that we only display something the first time we are called
//      or if we tried at least one open network.
//
//----------------------------------------------------------------------------

void cMyDisplay_LCD2004::cb_WiFi_NoUsableNetwork (  const uint8_t   numSecureNetworks,
                                                    const uint8_t   numOpenNetworks )
{
    //
    //  Local variables :
    //
    char            buf[40];                        // Temporary text buffer.
    static bool     s_FirstTime     = true;         // Used to track the first time we're called.


    //
    //  Always display the following :
    //
    MyPrintf ( "[cMyDisplay_LCD2004::cb_WiFi_NoUsableNetwork]  ------------------------------------------\n" );
    MyPrintf ( "[cMyDisplay_LCD2004::cb_WiFi_NoUsableNetwork]  Called : First time = %s / Num secure = %u / Num open = %u.\n",
                ( (s_FirstTime == true) ? "TRUE" : "FALSE"),
                numSecureNetworks,
                numOpenNetworks );


    //
    //  Should we display a message on our display ?
    //
    if ( (s_FirstTime == true) || (numOpenNetworks > 0) )
    {
        snprintf ( buf, sizeof(buf), "%u open found.", numOpenNetworks );

        MyPrintf ( "[cMyDisplay_LCD2004::cb_WiFi_NoUsableNetwork]  Displaying stuff on the display just this one time.\n" );

        //                "--------------------"
        DisplayMessage3 ( "No usable networks",
                          "found.",
                          buf,
                          true );

        delay ( 2000 );

        s_FirstTime = false;
    }
}


//----------------------------------------------------------------------------
//
//  setup () -- Routine to setup our LCD2004 display.
//
//----------------------------------------------------------------------------

void cMyDisplay_LCD2004::setup ( void )
{
    //
    //  Local variables :
    //
    char    buf[30];
    int     status;


    //--------------------------------------------------------------------
    //
    //  Call the base class's setup() routine :
    //
    //--------------------------------------------------------------------

    cMyDisplay::setup ();


    MyPrintf ( "[Display_LCD2004::setup]  Called.\n" );

    MyPrintf ( "[Display_LCD2004::setup]  LED_BUILTIN pin = %d.\n", LED_BUILTIN );
    MyPrintf ( "[Display_LCD2004::setup]  LCD I2C SCL pin = %d.\n", LCD_I2C_SCL_PIN );
    MyPrintf ( "[Display_LCD2004::setup]  LCD I2C SDA pin = %d.\n", LCD_I2C_SDA_PIN );
    MyPrintf ( "[Display_LCD2004::setup]  LCD I2C address = 0x%02x.\n", LCD_I2C_ADDR );

#ifdef  SCL
    snprintf ( buf, sizeof(buf), "%d", SCL );
#else
    snprintf ( buf, sizeof(buf), "<Undefined>" );
#endif  // !SCL

    MyPrintf ( "[Display_LCD2004::setup]  Def I2C SCL pin = %s.\n", buf );

#ifdef  SDA
    snprintf ( buf, sizeof(buf), "%d", SDA );
#else
    snprintf ( buf, sizeof(buf), "<Undefined>" );
#endif  // !SDA

    MyPrintf ( "[Display_LCD2004::setup]  Def I2C SDA pin = %s.\n", buf );


    //
    //  Mark that we are not yet displaying time :
    //
    m_DisplayingTime = false;


    //
    //  Initialize the LCD with 20 cols, 4 rows
    //  This function performs the auto-configuration that the Diag tool did
    //
    status = m_LCD.begin ( LCD_COLS, LCD_ROWS );

    if ( status != 0 )
    {
        //
        //  If status is non-zero, it couldn't find/config the LCD :
        //
        MyPrintf ( "[Display_LCD2004::setup]  LCD initialization failed with status: %d.\n", status );
        MyPrintf ( "[Display_LCD2004::setup]  *** LCD2004 display not present ***\n" );
        m_LcdIsPresent = false;
    }
    else
    {
        MyPrintf ( "[Display_LCD2004::setup]  LCD initialization successful.\n" );
        m_LcdIsPresent = true;
    }


    //
    //  Do some more LCD initialization if the LCD is attached to us :
    //
    if ( m_LcdIsPresent == true )
    {
        m_LCD.backlight();
        ClearScreen ();

        // Load Custom Characters
        for ( int i = 0 ; i < SEGS_SIZE ; i++ )
        {
            m_LCD.createChar ( i, SEGS[i] );
        }
    }


    //
    //  Call ClearScreen() even if we don't have an LCD2004 display
    //  since ClearScreen() will also clear/initialize m_TextScreen[] :
    //
    ClearScreen ();


#if 0  // Enable debug routine -- DEBUG HACK
//    debugRoutine01 ();
    debugRoutine02 ();
#endif  // Enable debug routine.

}


//----------------------------------------------------------------------------
//
//  handle () -- Routine to check for updates to our LCD2004 display.
//
//----------------------------------------------------------------------------

void cMyDisplay_LCD2004::handle ( void )
{
    //
    //  Local variables :
    //
    char            colon_top;                              // Top part of the colon.
    char            colon_bottom;                           // Bottom part of the colon.
    uint32_t        curMillis               = millis();
    uint8_t         oldCommandDelay         = 0;            //
    static bool     s_ColonState            = false;
    static bool     s_OldDisplayingTime     = false;
    static uint32_t s_LastMillis            = 0;
    //static uint32_t s_LastTextScreenTime    = 0;
    static char     s_OldColonChar          = '-';
    bool            debug_DisplayTextScreen = false;


    //--------------------------------------------------------------------
    //
    //  Call the base class's handle() routine :
    //
    //--------------------------------------------------------------------

    cMyDisplay::handle ();


    //--------------------------------------------------------------------
    //
    //  Is this the first time we're being called ?
    //
    //--------------------------------------------------------------------

    if ( g_FirstTime == true )
    {
        MyPrintf ( F("[Display_LCD2004::handle]  Called.\n") );
    }


    //
    //    Are we currently displaying the time?  If so, then we should
    //    display the blinking colon :
    //
    if ( m_DisplayingTime == true )
    {
        if ( s_OldDisplayingTime != m_DisplayingTime )
        {
            MyPrintf ( "[Display_LCD2004::handle]  Displaying colon was %s / Now %s.\n",
                ( (s_OldDisplayingTime == true) ? "TRUE" : "FALSE" ),
                ( (m_DisplayingTime == true)    ? "TRUE" : "FALSE" ) );

            s_OldDisplayingTime = m_DisplayingTime;
        }

        //
        //  Flashing colon (runs every second) and also display
        //  the seconds in the bottom left corner of the LCD :
        //
        if ( (curMillis - s_LastMillis) >= 1000 )
        {
            s_LastMillis = curMillis;

            s_ColonState = !s_ColonState;

            //
            //  Are we supposed to display something ?
            //
            if ( s_ColonState == true )
            {
                //--------------------------------------------------------
                //
                //  Determine what to use for the top half of the colon.
                //  It is based on connection type :
                //
                //      '!' = Not connected to a network.
                //
                //      'O' = Connected to an open (insecure) network.
                //
                //      '*' = Connected to a secure network.
                //
                //      '?' = Unable to determine the type of network
                //            that we are connected to.
                //
                //--------------------------------------------------------

                //
                //  Determine what to display based on our connection type :
                //
                switch ( g_WiFi.GetHostConnectionType() )
                {
                    case CON_TYPE_NOT_CONNECTED : colon_top = '!';  break;
                    case CON_TYPE_OPEN          : colon_top = 'O';  break;
                    case CON_TYPE_SECURE        : colon_top = '*';  break;
                    case CON_TYPE_UNKNOWN       : colon_top = '?';  break;
                    default                     : colon_top = '?';  break;
                } // switch

                //--------------------------------------------------------
                //
                //  In this version, the bottom part of the colon simply
                //  mimics the top of part of the colon.  The status of
                //  various things (time, weather, etc) is now displayed
                //  in the top left corner of the display, so we don't
                //  need to do anything special with the bottom part of
                //  the colon.                          -JonS 2026/03/11
                //
                //--------------------------------------------------------

                colon_bottom    = colon_top;    // Use same character as the top part of the colon.

#if 0   // Pivoting...  The bottom part of the colon mimics the top part.
                //--------------------------------------------------------
                //
                //  Determine what to use for the bottom half of the
                //  colon.  It is based on if we were able to get the
                //  time, messages, weather, etc :
                //
                //      '!' = Not connected to WiFi network.
                //
                //      '*' = Connected to WiFi network + Everything is
                //            good (we got the messages, time, etc).
                //
                //      'M' = Unable to get the messages.   [TO-DO]
                //
                //      'T' = Unable to get the time.       [TO-DO]
                //
                //      'W' = Unable to get the weather.    [TO-DO]
                //
                //--------------------------------------------------------

                if ( WiFi.status() != WL_CONNECTED )
                {
                    colon_bottom    = '!';      // Not connected -- FOR NOW JUST DO THIS.
                }
                else
                {
                    colon_bottom    = '*';      // Connected -- FOR NOW JUST DO THIS.
                }
#endif  // Pivoting...  The bottom part of the colon mimics the top part.

                //
                //  Did the colon change ?
                //
                if ( s_OldColonChar != colon_top )
                {
                    MyPrintf ( "[Display_LCD2004::handle]  Colon was [%c] [%c] / Now [%c] [%c].\n",
                        s_OldColonChar,
                        s_OldColonChar,
                        colon_top,
                        colon_bottom );

                    s_OldColonChar = colon_top;
                }
            }
            else
            {
                //
                //  We are not supposed to display a colon, so use a
                //  space (' ') for both the top and bottom part of the
                //  colon :
                //
                colon_top       = ' ';
                colon_bottom    = ' ';
            }

            //
            //  Now display the colon in two steps -- note that
            //  we override the default 'command delay' since we
            //  want to display the colons quickly (unlike the
            //  digits of the hours and minutes) :
            //
            oldCommandDelay = SetCommandDelay ( 7 );        // Save the old delay + set the new delay.

            SetCursor ( 1, 9 );                             // Top part of the colon.
            Print ( colon_top );                            //

            SetCursor ( 2, 9 );                             // Bottom part of the colon.
            Print ( colon_bottom );                         //

            SetCommandDelay ( oldCommandDelay );            // Restore the old delay.

            delay ( 10 );                                   // Another delay() just for good measure.

            debug_DisplayTextScreen = true;

        } // if
    }


    //
    //  Is it time to update the seconds field on the LCD ?
    //
    if ( curMillis > m_SecondsUpdateAt )
    {
        m_Seconds = ( (m_Seconds + 1) % 60 );   // Make sure we stay in the 0..59 range.
        m_SecondsUpdateAt += 1000;
        DisplaySeconds ( m_Seconds );

        debug_DisplayTextScreen = true;
    }


#if 0
    //
    //  Is it time to display the text "screen" to the Serial port ?
    //
    if ( curMillis >= s_LastTextScreenTime )
    {
        MyPrintf ( "[Display_LCD2004::handle]  Time to display text screen.\n" );
        debug_DisplayTextScreen = true;
        s_LastTextScreenTime = ( curMillis + 11333 );
    }
#endif


    //
    //  Are we supposted to display the text "screen" ?
    //
    if ( debug_DisplayTextScreen == true )
    {
        // TextScreen_Display ();              // DEBUG HACK
    }
}


#endif  // USE_DISPLAY_LCD2004


//----------------------------------------------------------------------------
//
//  end of  MyDisplay_LCD2004.cpp
//
//----------------------------------------------------------------------------
