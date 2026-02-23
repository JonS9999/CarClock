//----------------------------------------------------------------------------
//
//  MyWiFi_OTA.cpp
//
//    Arduino library to include the built-in OTA (Over-The-Air) code and
//    to provide high level wrapper/helper functions.
//
//    Jon Scheer (2019-2020)
//
//    Rev 1.1
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
#include "MyWifi.h"
#include "MyWiFi_OTA.h"


//-----------------------------------------------------------------------------
//
//  Global variable :
//
//-----------------------------------------------------------------------------

//
//  g_IsUpdating -- Flag used to track if an OTA update is in progress.
//
static bool      g_IsUpdating        = false;

//
//  g_PercentComplete -- If we're doing an OTA update, then this indicates
//                       how much we've updated (0..100%).
//
static uint16_t  g_PercentComplete   = 0;

//
//  g_CallbackOnStart -- Routine that we call when we process an
//                       OTA 'onStart' operation.
//
void (*g_CallbackOnStart) ( void );

//
//  g_CallbackOnEnd -- Routine that we call when we process an
//                     OTA 'onEnd' operation.
//
void (*g_CallbackOnEnd) ( void );

//
//  g_CallbackOnError -- Routine that we call when we process an
//                       OTA 'onError' operation.
//
void (*g_CallbackOnError) ( void );

//
//  g_CallbackOnProgress -- Routine that we call when we process an
//                          OTA 'onProgress' operation.
//
void (*g_CallbackOnProgress) ( unsigned int progress, unsigned int total );


//-----------------------------------------------------------------------------
//
//  Constructor
//
//-----------------------------------------------------------------------------

cMyWiFi_OTA::cMyWiFi_OTA ( void )
{
  MyPrintf ( "< cMyWiFi_OTA constructor called >\n" );


  //
  //  Set our flag to show that we have not yet been initialized :
  //
  m_IsInitialized = false;


  //
  //  Things we want to make sure we do only once :
  //
  g_IsUpdating          = false;
  g_PercentComplete     = 0;
  g_CallbackOnStart     = NULL;
  g_CallbackOnEnd       = NULL;
  g_CallbackOnError     = NULL;
  g_CallbackOnProgress  = NULL;
}


//-----------------------------------------------------------------------------
//
//  isUpdating () -- Routine that returns 'true' if an OTA update
//                   is currently happening.
//
//-----------------------------------------------------------------------------

bool cMyWiFi_OTA::isUpdating ( void )
{
  return ( g_IsUpdating );
}


//-----------------------------------------------------------------------------
//
//  percentComplete () -- Routine that returns percentage complete of
//                        the OTA update.
//
//-----------------------------------------------------------------------------

uint16_t cMyWiFi_OTA::percentComplete ( void )
{
  return ( g_PercentComplete );
}


//-----------------------------------------------------------------------------
//
//  SetCallbackOnStart () -- Routine to set the OTA 'onStart' callback.
//
//-----------------------------------------------------------------------------

void cMyWiFi_OTA::SetCallbackOnStart ( void (*ptr) ( void ) )
{
  MyPrintf ( "[WiFi_OTA::SetCallbackOnStart] : Called : Callback = 0x%08X.\n", ( (ptr != NULL) ? ptr : 0x0000 ) );

  g_CallbackOnStart = ptr;
}


//-----------------------------------------------------------------------------
//
//  SetCallbackOnEnd () -- Routine to set the OTA 'onEnd' callback.
//
//-----------------------------------------------------------------------------

void cMyWiFi_OTA::SetCallbackOnEnd ( void (*ptr) ( void ) )
{
  MyPrintf ( "[WiFi_OTA::SetCallbackOnEnd] : Called : Callback = 0x%08X.\n", ( (ptr != NULL) ? ptr : 0x0000 ) );

  g_CallbackOnEnd = ptr;
}


//-----------------------------------------------------------------------------
//
//  SetCallbackOnError () -- Routine to set the OTA 'onError' callback.
//
//-----------------------------------------------------------------------------

void cMyWiFi_OTA::SetCallbackOnError ( void (*ptr) ( void ) )
{
  MyPrintf ( "[WiFi_OTA::SetCallbackOnError] : Called : Callback = 0x%08X.\n", ( (ptr != NULL) ? ptr : 0x0000 ) );

  g_CallbackOnError = ptr;
}


//-----------------------------------------------------------------------------
//
//  SetCallbackOnProgress () -- Routine to set the OTA 'onProgress' callback.
//
//-----------------------------------------------------------------------------

void cMyWiFi_OTA::SetCallbackOnProgress ( void (*ptr) ( unsigned int progress, unsigned int total ) )
{
  MyPrintf ( "[WiFi_OTA::SetCallbackOnProgress] : Called : Callback = 0x%08X.\n", ( (ptr != NULL) ? ptr : 0x0000 ) );

  g_CallbackOnProgress = ptr;
}


//-----------------------------------------------------------------------------
//
//  setup () -- Routine to setup all the WiFi OTA stuff.
//
//-----------------------------------------------------------------------------

void cMyWiFi_OTA::setup ( const char* myHostname )
{
  //
  //  Stop now if we've already been run :
  //
  if ( m_IsInitialized == true )
  {
    return;
  }


  //
  //  Mark that we've been initialized :
  //
  m_IsInitialized = true;


  //
  //  Display an entry message :
  //
  MyPrintf ( "[WiFi_OTA::setup] : Called.  My hostname = [%s].\n", myHostname );


  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);


  //
  //  Set up OTA (Over The Air) programming stuff :
  //
  MyPrintf ( "[WiFi_OTA::setup] : Setting up OTA stuff...  Hostname = [%s].\n", myHostname );
  ArduinoOTA.setHostname ( myHostname );

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  ArduinoOTA.onStart ( []()
  {
    String type;

    g_IsUpdating        = true;
    g_PercentComplete   = 0;

    if (ArduinoOTA.getCommand() == U_FLASH)
    {
      type = "sketch";
    }
    else
    { // U_SPIFFS
      type = "filesystem";
    }

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    MyPrintf ( "[WiFi_OTA::onStart] : Start updating of %s.\n\r", type.c_str() );

    //
    //  If we have a user specified 'onStart' callback, call it :
    //
    if ( g_CallbackOnStart != NULL )
    {
      MyPrintf ( F("[WiFi_OTA::onStart] : --- Calling 'onStart' callback ---\n") );
      (*g_CallbackOnStart) ();
      MyPrintf ( F("[WiFi_OTA::onStart] : -- Back from 'onStart' callback ---\n") );
    }
  });

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  ArduinoOTA.onEnd ( []()
  {
    g_PercentComplete = 100;

    //
    //  If we have a user specified 'onEnd' callback, call it :
    //
    if ( g_CallbackOnEnd != NULL )
    {
      MyPrintf ( F("[WiFi_OTA::onEnd] : --- Calling 'onEnd' callback ---\n") );
      (*g_CallbackOnEnd) ();
      MyPrintf ( F("[WiFi_OTA::onEnd] : -- Back from 'onEnd' callback ---\n") );
    }

    MyPrintf ( "[WiFi_OTA::onEnd] : End.\n\n" );
  });

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  ArduinoOTA.onProgress ( [](unsigned int progress, unsigned int total )
  {
    // char        buf[30];
    uint16_t    newProgress   = ( ( progress / ( total / 100 ) ) / 10 );

    g_IsUpdating = true;

    if ( newProgress != g_PercentComplete )
    {
      delay ( 20 );
      Serial.printf ( "\n\r" );
      delay ( 20 );
      MyPrintf ( "[WiFi_OTA::onProgress] : Progress: %u %%\n\r", ( newProgress * 10 ) );
      delay ( 20 );
      g_PercentComplete = newProgress;

#if 0     // JonS -- DEBUG HACK

      g_Display.clear ();

      g_Display.printAt ( 0, 1, "Firmware is" );
      g_Display.printAt ( 0, 2, "updating..." );

      snprintf ( buf, sizeof(buf), "%u % complete.", g_WiFi.OTA_PercentComplete() );
      g_Display.printAt ( 0, 4, buf );

      g_Display.handle ();

#endif    // JonS -- DEBUG HACK

    }
    else
    {
      Serial.printf ( "." );
    }

    //
    //  If we have a user specified 'onProgress' callback, call it :
    //
    if ( g_CallbackOnProgress != NULL )
    {
      MyPrintf ( F("[WiFi_OTA::onProgress] : --- Calling 'onProgress' callback ---\n") );
      (*g_CallbackOnProgress) ( progress, total );
      MyPrintf ( F("[WiFi_OTA::onProgress] : -- Back from 'onProgress' callback ---\n") );
    }
  });

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  ArduinoOTA.onError ( [](ota_error_t error)
  {
    MyPrintf ( "[WiFi_OTA::onError] : Error[%u]: ", error  );

    if ( error == OTA_AUTH_ERROR )          {
      Serial.println ( "Auth Failed." );
    }
    else if ( error == OTA_BEGIN_ERROR )    {
      Serial.println ( "Begin Failed." );
    }
    else if ( error == OTA_CONNECT_ERROR )  {
      Serial.println ( "Connect Failed." );
    }
    else if ( error == OTA_RECEIVE_ERROR )  {
      Serial.println ( "Receive Failed." );
    }
    else if ( error == OTA_END_ERROR )      {
      Serial.println ( "End Failed." );
    }
    else                                    {
      Serial.printf  ( "* Unknown error %u (0x%04X) *\n", error, error );
    }

    //
    //  If we have a user specified 'onError' callback, call it :
    //
    if ( g_CallbackOnError != NULL )
    {
      MyPrintf ( F("[WiFi_OTA::onError] : --- Calling 'onError' callback ---\n") );
      (*g_CallbackOnError) ();
      MyPrintf ( F("[WiFi_OTA::onError] : -- Back from 'onError' callback ---\n") );
    }
  });

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


  //
  //  Start the OTA service :
  //
  MyPrintf ( "[WiFi_OTA::setup] : Starting the OTA service...\n\r" );
  // delay ( 1000 );
  ArduinoOTA.begin ();
  MyPrintf ( "[WiFi_OTA::setup] : OTA service has been started.\n\r" );
  // delay ( 1000 );
}


//-----------------------------------------------------------------------------
//
//  handle () -- Routine to check for WiFi OTA changes.
//
//-----------------------------------------------------------------------------

void cMyWiFi_OTA::handle ( void )
{
  //
  //  Is this the first time we're being called ?
  //
  if ( g_FirstTime == true )
  {
    MyPrintf ( "[WiFi_OTA::handle] : Loop now running...\n" );
  }


  //
  //  Only do work if we've been initialized :
  //
  if ( m_IsInitialized == true )
  {
    ArduinoOTA.handle ();
  }
}


//----------------------------------------------------------------------------
//
//  end of  MyWiFi_OTA.cpp
//
//----------------------------------------------------------------------------
