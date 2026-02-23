//----------------------------------------------------------------------------
//
//  MyRTC.h
//
//    RTC (Real Time Clock) driver.
//
//    Jon Scheer (2026)
//
//    Rev 1.0
//
//----------------------------------------------------------------------------

#ifndef _MY_RTC_H_
#define _MY_RTC_H_


//
//  System include files :
//
#include <Arduino.h>
#include <RTClib.h>


//
//  Local include files :
//
#include "Common.h"


//----------------------------------------------------------------------------
//
//  cMyRTC object
//
//----------------------------------------------------------------------------

class cMyRTC
{
  private :

    //
    //  m_IsInitialized -- Flag used track if we've been initialized yet or not.
    //
    bool  m_IsInitialized;

    //
    //  One instance of the RTC object.
    //
    RTC_DS3231 m_RTC;

    //-----------------------------------------------------------------

  protected :

    //-----------------------------------------------------------------

  public :

    //
    //  Constructor :
    //
    cMyRTC ( void );

    //
    //  DisplayTime () -- Display the RTC in a human readable format :
    //
    void DisplayTime ( void );

    //
    //  Get the time from the RTC and return it in 'hours', 'minutes'
    //  and 'seconds' :
    //
    bool GetTime (  uint8_t     *hours,
                    uint8_t     *minutes,
                    uint8_t     *seconds );

    //
    //  Set the time in the RTC from our local (system) time :
    //
    bool SetRTCFromSystemTime ( void );

    //
    //  Set our system time from the time in the RTC :
    //
    bool SetSystemTimeFromRTC ( void );

    //
    //  Debug_Test_001 () -- Test routine # 1 :
    //
    void Debug_Test_001 ( void );

    //
    //  setup () -- Routine to setup our RTC :
    //
    void setup ( void );

    //
    //  handle () -- Routine to check for updates for our RTC :
    //
    void handle ( void );


    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    //
    //  Inline functions
    //
    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    inline bool IsInitialized ( void )
    {
      return ( m_IsInitialized );
    }

}; // cMyRTC


#endif  // !_MY_RTC_H_


//----------------------------------------------------------------------------
//
//  end of  MyRTC.h
//
//----------------------------------------------------------------------------
