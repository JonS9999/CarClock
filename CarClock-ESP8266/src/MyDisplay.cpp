//----------------------------------------------------------------------------
//
//  MyDisplay.cpp --
//
//      Version 1.0
//
//----------------------------------------------------------------------------


//
//  System include files :
//
#include <Arduino.h>


//
//  Local include files :
//
#include "Common.h"
#include "MyDisplay.h"


//----------------------------------------------------------------------------
//
//  Constants :
//
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
//
//  Constructor
//
//----------------------------------------------------------------------------

cMyDisplay::cMyDisplay ( void )
{
  MyPrintf ( "< cMyDisplay constructor called >\n" );
}


//----------------------------------------------------------------------------
//
//  ClearScreen () -- Routine to clear the display.
//
//----------------------------------------------------------------------------

void cMyDisplay::ClearScreen ( void )
{
    MyPrintf ( "[Display::ClearScreen]  Called.\n" );


    //
    //  Clear our display objects screen :
    //
    m_Display.ClearScreen ();
}


//----------------------------------------------------------------------------
//
//  DisplaySplashScreen () -- Display our splash (initial) screen.
//
//----------------------------------------------------------------------------

void cMyDisplay::DisplaySplashScreen ( void )
{
  m_Display.DisplaySplashScreen ();
}


//----------------------------------------------------------------------------
//
//  DisplayMessage2 () -- Display the user specified messages on our display.
//
//----------------------------------------------------------------------------

void cMyDisplay::DisplayMessage2 (  const char* MesgPart1,
                                    const char* MesgPart2 )
{
  m_Display.DisplayMessage2 ( MesgPart1, MesgPart2 );
}


//----------------------------------------------------------------------------
//
//  DisplayMessage3 () -- Display the user specified messages on our display.
//
//----------------------------------------------------------------------------

void cMyDisplay::DisplayMessage3 (  const char* MesgPart1,
                                    const char* MesgPart2,
                                    const char* MesgPart3,
                                    const bool  ForceToBottom )
{
  m_Display.DisplayMessage3 ( MesgPart1, MesgPart2, MesgPart3, ForceToBottom );
}


//----------------------------------------------------------------------------
//
//  DisplayTime () -- Display the time on our display.
//
//----------------------------------------------------------------------------

void cMyDisplay::DisplayTime ( const int hours, const int minutes, const int seconds )
{
  m_Display.DisplayTime ( hours, minutes, seconds );
}


//----------------------------------------------------------------------------
//
//  Print () -- Display the user specified text on the LCD.
//
//----------------------------------------------------------------------------

void cMyDisplay::Print ( const char* text )
{
  m_Display.Print ( text );
}


//----------------------------------------------------------------------------
//
//  Print () -- Display the user specified character on the LCD.
//
//----------------------------------------------------------------------------

void cMyDisplay::Print ( const uint8_t ch )
{
  m_Display.Print ( ch );
}


//----------------------------------------------------------------------------
//
//  Printf () -- Display the variable parameter user specified text on
//               the LCD.
//
//----------------------------------------------------------------------------

void cMyDisplay::Printf ( const char* format, ... )
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

void cMyDisplay::SetCursor ( const uint16_t row, const uint16_t column )
{
  m_Display.SetCursor ( row, column );
}


//----------------------------------------------------------------------------
//
//  SetObjectSerialCmd () -- Routine to specify optional SerialCommand object.
//
//----------------------------------------------------------------------------

void cMyDisplay::SetObjectSerialCmd ( SerialCommand* SerialCmd )
{
    MyPrintf ( "[Display::SetObjectSerialCmd]  Called : SerialCmd = 0x%p.\n", SerialCmd );

    m_Display.SetObjectSerialCmd ( SerialCmd );
}


//----------------------------------------------------------------------------
//
//  Write () -- Display the user specified character on the LCD.
//
//----------------------------------------------------------------------------

void cMyDisplay::Write ( const byte ch )
{
  m_Display.Write ( ch );
}


//----------------------------------------------------------------------------
//
//  Write () -- Display the user specified text on the LCD.
//
//----------------------------------------------------------------------------

void cMyDisplay::Write ( const char* text )
{
  m_Display.Write ( text );
}


//----------------------------------------------------------------------------
//
//  setup () -- Routine to setup our display.
//
//----------------------------------------------------------------------------

void cMyDisplay::setup ( void )
{
    MyPrintf ( "[Display::setup]  Called.\n" );


    //
    //  Initialize our display object :
    //
    m_Display.setup ();
}


//----------------------------------------------------------------------------
//
//  handle () -- Routine to check for updates to our display.
//
//----------------------------------------------------------------------------

void cMyDisplay::handle ( void )
{
  //
  //  Local variables :
  //


  //--------------------------------------------------------------------
  //
  //  Is this the first time we're being called ?
  //
  //--------------------------------------------------------------------

  if ( g_FirstTime == true )
  {
    MyPrintf ( F("[Display::handle]  Called.\n") );
  }


    //
    //  Call our display-specific handler :
    //
    m_Display.handle ();
}


//----------------------------------------------------------------------------
//
//  end of  MyDisplay.cpp
//
//----------------------------------------------------------------------------
