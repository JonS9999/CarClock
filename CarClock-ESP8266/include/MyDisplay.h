//----------------------------------------------------------------------------
//
//  MyDisplay.h
//
//    High level display driver.  This is the base class for the actual
//    display driver (e.g., MyDisplay_LCD2004).
//
//    Jon Scheer (2026)
//
//    Rev 1.1
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


//-----------------------------------------------------------------------------
//
//  cMyDisplay object
//
//-----------------------------------------------------------------------------

class cMyDisplay
{
  protected :

    //
    //  m_IsInitialized -- Flag used track if we've been initialized yet or not.
    //
    bool  m_IsInitialized;

    //-----------------------------------------------------------------

  public :

    //
    //  Constructor :
    //
    cMyDisplay ( void );


    //
    //  setup () -- Routine to setup our display :
    //
    virtual void setup ( void );


    //
    //  handle () -- Routine to check for updates to our display :
    //
    virtual void handle ( void );


    //-----------------------------------------------------------------
    //
    //  Pure virtual functions (*MUST* be defined in derived class)
    //
    //-----------------------------------------------------------------

    //
    //  ClearScreen () -- Routine to clear the display :
    //
    virtual void ClearScreen ( void ) = 0;

    //
    //  DisplaySplashScreen () -- Display our splash (initial) screen :
    //
    virtual void DisplaySplashScreen ( void ) = 0;

    //
    //  DisplayMessage2 () -- Display the user specified messages on our display :
    //
    virtual void DisplayMessage2 (  const char* MesgPart1,
                                    const char* MesgPart2 = nullptr )     = 0;

    //
    //  DisplayMessage3 () -- Display the user specified messages on our display :
    //
    virtual void DisplayMessage3 (  const char* MesgPart1,
                                    const char* MesgPart2     = nullptr,
                                    const char* MesgPart3     = nullptr,
                                    const bool  ForceToBottom = true )    = 0;

    //
    //  DisplayTime () -- Display the time on our LCD :
    //
    virtual void DisplayTime (  const int   hours,
                                const int   minutes,
                                const int   seconds   = 0 )     = 0;

    //
    //  Print () -- Display the user specified text on the LCD :
    //
    virtual void Print ( const char* text ) = 0;

    //
    //  Print () -- Display the user specified character on the LCD :
    //
    virtual void Print ( const uint8_t ch ) = 0;

    //
    //  Printf () -- Display the variable parameter user specified text on the LCD :
    //
    virtual void Printf ( const char* format, ... ) = 0;

    //
    //  SetCursor () -- Move the cursor to the specified location :
    //
    //    *** Note the parameter order is "row, column", not "column, row" ***
    //
    virtual void SetCursor ( const uint16_t row, const uint16_t column ) = 0;

    //
    //  SetSerialCmdObject () -- Routine to specify optional SerialCommand object :
    //
    virtual void SetObjectSerialCmd ( SerialCommand* SerialCmd = nullptr ) = 0;

    //
    //  Write () -- Display the user specified character on the LCD :
    //
    virtual void Write ( const byte ch ) = 0;

    //
    //  Write () -- Display the user specified text on the LCD :
    //
    virtual void Write ( const char* text ) = 0;

}; // cMyDisplay


//
//  External declaration for singleton (one instance of) g_Display object :
//
extern cMyDisplay*  g_Display;

#endif  // !_MY_DISPLAY_H_


//----------------------------------------------------------------------------
//
//  end of  MyDisplay.h
//
//----------------------------------------------------------------------------
