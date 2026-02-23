//----------------------------------------------------------------------------
//
//  MyWiFi_OTA.h
//
//    Arduino library to include the built-in OTA (Over-The-Air) code and
//    to provide high level wrapper/helper functions.
//
//    Jon Scheer (2026)
//
//    Rev 1.0
//
//----------------------------------------------------------------------------

#ifndef _MY_WIFI_OTA_H_
#define _MY_WIFI_OTA_H_


//
//  System include files :
//
#include <ArduinoOTA.h>

//
//  Local include files :
//
#include "Common.h"



//-----------------------------------------------------------------------------
//
//  cMyWiFi_OTA object
//
//-----------------------------------------------------------------------------

class cMyWiFi_OTA
{
  private :

    //
    //  m_IsInitialized -- Flag used track if we've been initialized yet or not.
    //
    bool  m_IsInitialized;

    //-----------------------------------------------------------------

  public :

    //
    //  Constructor :
    //
    cMyWiFi_OTA ( void );

    //
    //  isUpdating () -- Routine that returns 'true' if an OTA update
    //                   is currently happening.
    //
    bool isUpdating ( void );

    //
    //  percentComplete () -- Routine that returns percentage complete of
    //                        the OTA update.
    //
    uint16_t percentComplete ( void );

    //
    //  Routine to set the OTA 'onStart' callback :
    //
    void SetCallbackOnStart ( void (*ptr) ( void ) );

    //
    //  Routine to set the OTA 'onEnd' callback :
    //
    void SetCallbackOnEnd ( void (*ptr) ( void ) );

    //
    //  Routine to set the OTA 'onError' callback :
    //
    void SetCallbackOnError ( void (*ptr) ( void ) );

    //
    //  Routine to set the OTA 'onProgress' callback :
    //
    void SetCallbackOnProgress ( void (*ptr) ( unsigned int progress, unsigned int total ) );

    //
    //  setup () -- Routine to setup all things WiFi OTA related :
    //
    void setup ( const char* myHostname );

    //
    //  handle () -- Routine to check for WiFi OTA related things :
    //
    void handle ( void );


    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    //
    //  Inline functions
    //
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#if 0
    //
    //  isUpdating () -- Routine that returns 'true' if an OTA update
    //                   is currently happening.
    //
    inline bool isUpdating ( void )
    {
      return ( g_IsUpdating );
    }


    //
    //  percentComplete () -- Routine that returns percentage complete of
    //                        the OTA update.
    //
    inline uint16_t percentComplete ( void )
    {
      return ( g_PercentComplete );
    }
#endif

}; // cMyWiFi_OTA


#endif  // !_MY_WIFI_OTA_H_


//----------------------------------------------------------------------------
//
//  end of  MyWiFi_OTA.h
//
//----------------------------------------------------------------------------
