//----------------------------------------------------------------------------
//
//  MyDisplay_LCD2004.h
//
//    Low level display driver for the Hitachi 20x4 LCD display.
//
//    Jon Scheer (2026)
//
//    Rev 1.3
//
//----------------------------------------------------------------------------

#ifndef _MY_DISPLAY_LCD2004_H_
#define _MY_DISPLAY_LCD2004_H_


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
#include <Wire.h>                           // For I2C support.
#include <hd44780.h>                        // Hitachi LCD 2004 display.
#include <hd44780ioClass/hd44780_I2Cexp.h>  // I2C expander I/O class header.
#include <SerialCommand.h>                  // Serial/USB port command line interface.


//
//  Local include files :
//
#include "MyDisplay.h"              // Generic display.


//-----------------------------------------------------------------------------
//
//  Global constants
//
//-----------------------------------------------------------------------------

const uint8_t   LCD2004_COLS            = 20;
const uint8_t   LCD2004_ROWS            = 4;

const uint8_t   DEFAULT_COMMAND_DELAY   = 20;     // Number of msecs to delay() when driving the LCD.


//-----------------------------------------------------------------------------
//
//  cMyDisplay_LCD2004 object
//
//-----------------------------------------------------------------------------

class cMyDisplay_LCD2004 : public cMyDisplay
{
  private :

    //
    //  m_LcdIsPresent -- Flag used to determine if we have a display connected
    //                    to us (it's okay if we don't as we may be just using
    //                    a standalone ESP8266 with a serial/USB port).
    //
    bool  m_LcdIsPresent;

    //
    //  m_SerialCmd -- Pointer to an optional SerialCommand object.
    //
    SerialCommand *m_pSerialCmd;

    //
    //  m_CommandDelay -- Used with a delay() command after printing a character
    //                    or moving the cursor.  We need a delay so we don't
    //                    send commands to the LCD too quickly (if we send stuff
    //                    to the LCD too quickly, the ccommands will be dropped
    //                    and the LCD won't behave like we expect).
    //
    uint m_CommandDelay;

    //
    //  m_DisplayingTime -- Flag used to determine if we are displaying the time.
    //
    bool  m_DisplayingTime;

    //
    //  m_Seconds : We need to keep track of the seconds since the main loop
    //              only calls DisplayTime() once a minute.
    //
    uint8_t m_Seconds;


    //
    //  m_SecondsUpdateTime : The time we need to update m_Seconds.
    //
    uint32_t m_SecondsUpdateAt;


    //
    //  m_LCD : One instance of the LCD2004 object.
    //
    hd44780_I2Cexp m_LCD;


    //
    //  Character array that mimics what we've written to the LCD :
    //
    uint8_t m_TextScreen[LCD2004_ROWS][LCD2004_COLS];


    //
    //  Cursor position attributes for indexing into m_LCD_Screen[][]
    //  (column and row) :
    //
    uint8_t m_TextScreen_Posi_Col;
    uint8_t m_TextScreen_Posi_Row;


    //
    //  VT100 emulation mode?
    //
    bool m_VT100_Emulation;

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    //
    //  Display_Seconds () -- Display the seconds on the LCD :
    //
    void DisplaySeconds ( const int seconds );

    //
    //  TextScreen_ClearScreen () -- Clear our text "screen" :
    //
    void TextScreen_ClearScreen ( void );

    //
    //  TextScreen_Display () -- Display our "screen" to the serial port :
    //
    void TextScreen_Display ( void );

    //
    //  TextScreen_DisplayChar () -- Display a character (that is in
    //                               our text "screen") to the serial
    //                               port.
    //
    void TextScreen_DisplayChar ( const byte ch );


    //-----------------------------------------------------------------

  protected :

    //
    //  Routine to draw a big digit on the LCD :
    //
    void DrawDigit ( const int digit, const int x, const int width = 4 );

    //
    //  Print_Delay () -- Display the user specified character on the
    //                    LCD using the user specified delay :
    //
    // void Print_Delay ( const uint8_t delayValue, const uint8_t ch );


    //-----------------------------------------------------------------

  public :

    //
    //  Constructor :
    //
    cMyDisplay_LCD2004 ( void );


    //-----------------------------------------------------------------
    //
    //  Pure virtual functions defined in this class
    //
    //-----------------------------------------------------------------

    //
    //  ClearScreen () -- Routine to clear the display :
    //
    void ClearScreen ( void ) override;

    //
    //  DisplaySplashScreen () -- Display our splash (initial) screen :
    //
    void DisplaySplashScreen ( void ) override;

    //
    //  DisplayMessage2 () -- Display the user specified messages on our LCD :
    //
    void DisplayMessage2 (  const char* MesgPart1,
                            const char* MesgPart2 = nullptr ) override;

    //
    //  DisplayMessage3 () -- Display the user specified messages on our display :
    //
    void DisplayMessage3 (  const char* MesgPart1,
                            const char* MesgPart2     = nullptr,
                            const char* MesgPart3     = nullptr,
                            const bool  ForceToBottom = true ) override;

    //
    //  DisplayTime () -- Display the time on our LCD :
    //
    void DisplayTime ( const int hours, const int minutes, const int seconds = 0 ) override;

    //
    //  Print () -- Display the user specified text on the LCD :
    //
    void Print ( const char* text ) override;

    //
    //  Print () -- Display the user specified character on the LCD :
    //
    void Print ( const uint8_t ch ) override;

    //
    //  Printf () -- Display the variable parameter user specified text on the LCD :
    //
    void Printf ( const char* format, ... ) override;

    //
    //  SetCursor () -- Move the cursor to the specified location :
    //
    //    *** Note the parameter order is "row, column", not "column, row" ***
    //
    void SetCursor ( const uint16_t row, const uint16_t column ) override;

    //
    //  SetObjectSerialCmd () -- Routine to specify optional SerialCommand object :
    //
    void SetObjectSerialCmd ( SerialCommand* SerialCmd = nullptr ) override;

    //
    //  Write () -- Display the user specified character on the LCD :
    //
    void Write ( const byte ch ) override;

    //
    //  Write () -- Display the user specified text on the LCD :
    //
    void Write ( const char* text ) override;


    //-----------------------------------------------------------------
    //
    //  Virtual functions that are defined in this class
    //
    //-----------------------------------------------------------------

    //
    //  cb_WiFi_ConnectingAttempt () -- Callback routine which is
    //      called when we are trying to connect to a WiFi network.
    //      We are passed in the maximum number of attempts that will
    //      be made and the number of attempts made so far.
    //
    virtual void cb_WiFi_ConnectingAttempt (  const uint8_t   numAttemptsSoFar,
                                              const uint8_t   maxAttempts );

    //
    //  cb_WiFi_NoUsableNetwork () -- Callback routine which is called
    //      when we tried to (but was unsuccessful) in connecting to
    //      a WiFi network.
    //
    virtual void cb_WiFi_NoUsableNetwork (  const uint8_t   numSecureNetworks,
                                            const uint8_t   numOpenNetworks );


    //-----------------------------------------------------------------
    //
    //  Normal functions that are defined in this class
    //
    //-----------------------------------------------------------------

    //
    //  setup () -- Routine to setup our display :
    //
    void setup ( void );

    //
    //  handle () -- Routine to check for updates to our display :
    //
    void handle ( void );


    //-----------------------------------------------------------------
    //
    //  Inline functions
    //
    //-----------------------------------------------------------------

    //
    //  GetCommandDelay () -- Return the current 'command delay' value :
    //
    inline const uint8_t GetCommandDelay ( void )
    {
      return ( (const uint8_t)m_CommandDelay );
    }

    //
    //  SetCommandDelay () -- Sets the 'command delay' value to the
    //      user specified command delay value.  Note that we return
    //      the old command delay value so the user can change the
    //      command delay value then return it to the previous command
    //      delay value.
    //
    inline const uint8_t SetCommandDelay ( const uint8_t commandDelay )
    {
      const uint8_t oldCommandDelay = m_CommandDelay;

      m_CommandDelay = commandDelay;

      return ( oldCommandDelay );
    }



}; // cMyDisplay_LCD2004


#endif  // USE_DISPLAY_LCD2004

#endif  // !_MY_DISPLAY_LCD2004_H_


//----------------------------------------------------------------------------
//
//  end of  MyDisplay_LCD2004.h
//
//----------------------------------------------------------------------------
