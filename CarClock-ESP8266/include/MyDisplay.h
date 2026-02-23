//----------------------------------------------------------------------------
//
//  MyDisplay.h
//
//    High level display driver.  This is the base class for the actual
//    display driver (e.g., MyDisplay_LCD2004).
//
//    Jon Scheer (2026)
//
//    Rev 1.0
//
//----------------------------------------------------------------------------

#ifndef _MY_DISPLAY_H_
#define _MY_DISPLAY_H_


//
//  System include files :
//
#include <Arduino.h>
#include <SerialCommand.h>


//
//  Local include files :
//
#include "Common.h"

#ifdef  USE_DISPLAY_LCD2004
#include "MyDisplay_LCD2004.h"          // Hitachi 20x4 LCD display.
#endif



//-----------------------------------------------------------------------------
//
//  cMyDisplay object
//
//-----------------------------------------------------------------------------

class cMyDisplay
{
  private :

    //
    //  m_IsInitialized -- Flag used track if we've been initialized yet or not.
    //
    bool  m_IsInitialized;

    //-----------------------------------------------------------------

#ifdef  USE_DISPLAY_LCD2004

    //
    //  One instance of the LCD2004 display driver :
    //
    cMyDisplay_LCD2004  m_Display;

#endif  // USE_DISPLAY_LCD2004

    //-----------------------------------------------------------------

  public :

    //
    //  Constructor :
    //
    cMyDisplay ( void );

    //
    //  ClearScreen () -- Routine to clear the display :
    //
    void ClearScreen ( void );

    //
    //  DisplaySplashScreen () -- Display our splash (initial) screen :
    //
    void DisplaySplashScreen ( void );

    //
    //  DisplayMessage2 () -- Display the user specified messages on our display :
    //
    void DisplayMessage2 (  const char* MesgPart1,
                            const char* MesgPart2 = nullptr );

    //
    //  DisplayMessage3 () -- Display the user specified messages on our display :
    //
    void DisplayMessage3 (  const char* MesgPart1,
                            const char* MesgPart2     = nullptr,
                            const char* MesgPart3     = nullptr,
                            const bool  ForceToBottom = true );

    //
    //  DisplayTime () -- Display the time on our LCD :
    //
    void DisplayTime ( const int hours, const int minutes, const int seconds = 0 );

    //
    //  Print () -- Display the user specified text on the LCD :
    //
    void Print ( const char* text );

    //
    //  Print () -- Display the user specified character on the LCD :
    //
    void Print ( const uint8_t ch );

    //
    //  Printf () -- Display the variable parameter user specified text on the LCD :
    //
    void Printf ( const char* format, ... );

    //
    //  SetCursor () -- Move the cursor to the specified location :
    //
    //    *** Note the parameter order is "row, column", not "column, row" ***
    //
    void SetCursor ( const uint16_t row, const uint16_t column );

    //
    //  SetSerialCmdObject () -- Routine to specify optional SerialCommand object :
    //
    void SetObjectSerialCmd ( SerialCommand* SerialCmd = nullptr );

    //
    //  Write () -- Display the user specified character on the LCD :
    //
    void Write ( const byte ch );

    //
    //  Write () -- Display the user specified text on the LCD :
    //
    void Write ( const char* text );

    //
    //  setup () -- Routine to setup our display :
    //
    void setup ( void );

    //
    //  handle () -- Routine to check for updates to our display :
    //
    void handle ( void );

}; // cMyDisplay


#endif  // !_MY_DISPLAY_H_


//----------------------------------------------------------------------------
//
//  end of  MyDisplay.h
//
//----------------------------------------------------------------------------
