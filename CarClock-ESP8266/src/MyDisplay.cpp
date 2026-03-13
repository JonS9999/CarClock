//----------------------------------------------------------------------------
//
//  MyDisplay.cpp --
//
//      Version 1.1
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
//  cb_WiFi_ConnectingAttempt () -- Callback routine which is
//      called when we are trying to connect to a WiFi network.
//      We are passed in the maximum number of attempts that will
//      be made and the number of attempts made so far.
//
//----------------------------------------------------------------------------

void cMyDisplay::cb_WiFi_ConnectingAttempt (  const uint8_t   numAttemptsSoFar,
                                              const uint8_t   maxAttempts )
{
    MyPrintf ( "[Display::cb_WiFi_ConnectingAttempt]  Called : Attempt # %u of %u.\n",
                numAttemptsSoFar,
                maxAttempts );
}


//----------------------------------------------------------------------------
//
//  cb_WiFi_NoUsableNetwork () -- Callback routine which is called
//      when we tried to (but was unsuccessful) in connecting to
//      a WiFi network.
//
//----------------------------------------------------------------------------

void cMyDisplay::cb_WiFi_NoUsableNetwork (  const uint8_t   numSecureNetworks,
                                            const uint8_t   numOpenNetworks )
{
    MyPrintf ( "[Display::cb_WiFi_NoUsableNetwork]  Called : Num secure = %u / Num open = %u.\n",
                numSecureNetworks,
                numOpenNetworks );
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
    //  Initialize our attributes :
    //
    m_IsInitialized   = false;
}


//----------------------------------------------------------------------------
//
//  handle () -- Routine to check for updates to our display.
//
//----------------------------------------------------------------------------

void cMyDisplay::handle ( void )
{
  //--------------------------------------------------------------------
  //
  //  Is this the first time we're being called ?
  //
  //--------------------------------------------------------------------

  if ( g_FirstTime == true )
  {
    MyPrintf ( F("[Display::handle]  Called.\n") );
  }
}


//----------------------------------------------------------------------------
//
//  end of  MyDisplay.cpp
//
//----------------------------------------------------------------------------
