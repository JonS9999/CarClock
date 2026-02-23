//----------------------------------------------------------------------------
//
//  MyRTC.cpp
//
//    RTC (Real Time Clock) driver.
//
//    Jon Scheer (2026)
//
//    Rev 1.0
//
//----------------------------------------------------------------------------


//
//  System include files :
//
#include <Arduino.h>
#include <Wire.h>


//
//  Local include files :
//
#include "Common.h"
#include "MyRTC.h"


//----------------------------------------------------------------------------
//
//  Constructor :
//
//----------------------------------------------------------------------------

cMyRTC::cMyRTC ( void )
{
    MyPrintf ( "< cMyRTC constructor called >\n" );
}


//----------------------------------------------------------------------------
//
//  DisplayTime () -- Display the RTC in a human readable format.
//
//----------------------------------------------------------------------------

void cMyRTC::DisplayTime ( void )
{
    //
    //  Local variables :
    //
    char        buf[30];
    time_t      nowRTC;                 // Time stored in the RTC (in UTC format).


    //
    //  Show the time currently stored in the RTC.
    //
    //  Note that we use snprintf() to make sure we have a null character
    //  ('\0') at the end of the string, but since ctime() puts a '\n' at
    //  the end, we change that '\n' into a null character (thus "removing"
    //  the '\n') :
    //
    nowRTC = m_RTC.now().unixtime();

    snprintf ( buf, sizeof(buf), "%s", ctime(&nowRTC) );

    if ( strlen(buf) > 0 )
    {
        buf[strlen(buf)-1] = '\0';
    }


    MyPrintf ( "[RTC::DisplayTime]  RTC time is [%s] UTC.\n", buf );
}


//----------------------------------------------------------------------------
//
//  GetTime () -- Routine to get the time from the RTC and return it in
//                the user specified hours, minutes and seconds parameters.
//
//----------------------------------------------------------------------------

bool cMyRTC::GetTime (  uint8_t     *hours,
                        uint8_t     *minutes,
                        uint8_t     *seconds )
{
    //
    //  Local variables :
    //
    DateTime    now             = m_RTC.now();


    //
    //  Return now if we havent' initialized communications with the RTC :
    //
    if ( m_IsInitialized == false )
    {
        MyPrintf ( "[RTC::GetTime]  Called, but RTC has not been initialized !!!\n" );
        return ( false );
    }


    //
    //  If the user wants the hours, get that value :
    //
    if ( hours != nullptr )
    {
        *hours = now.hour();
    }


    //
    //  If the user wants the minutess, get that value :
    //
    if ( minutes != nullptr )
    {
        *minutes = now.minute();
    }


    //
    //  If the user wants the seconds, get that value :
    //
    if ( seconds != nullptr )
    {
        *seconds = now.second();
    }


    MyPrintf ( "[RTC::GetTime]  Returning %d h / %d m / %d s.\n",
        ( (hours   != nullptr) ? *hours   : -1 ),
        ( (minutes != nullptr) ? *minutes : -1 ),
        ( (seconds != nullptr) ? *seconds : -1 ) );


    //
    //  Return success :
    //
    return ( true );
}


//----------------------------------------------------------------------------
//
//  SetRTCFromSystemTime () -- Set the time in the RTC from our current
//                             system (local) time.
//
//----------------------------------------------------------------------------

bool cMyRTC::SetRTCFromSystemTime ( void )
{
    //
    //  Local variables :
    //
    char        buf[255];
    time_t      nowUTC;                 // System time (in UTC format).
    time_t      nowRTC;                 // Time stored in the RTC (in UTC format).


    //
    //  Return now if we havent' initialized communications with the RTC :
    //
    if ( m_IsInitialized == false )
    {
        MyPrintf ( "[RTC::SetRTCFromSystemTime]  Called, but RTC has not been initialized !!!\n" );
        return ( false );
    }


    //
    //  Make sure we have a valid system time -- we do that by getting
    //  our system time, and then we make sure it's nowhere close to
    //  Jan 1st, 1970 :
    //
    nowUTC = UTC.now();

    if ( year(nowUTC) < 2020 )
    {
        MyPrintf ( "[RTC::SetRTCFromSystemTime]  +++ Warning - System time appears to be invalid +++\n" );
        return ( false );
    }


    MyPrintf ( "[RTC::SetRTCFromSystemTime]  Local time is [%s]\n", myTZ.dateTime().c_str() );


#if 1   // DEBUG HACK -- Display the RTC time for fun.

    //
    //  Show the time currently stored in the RTC.
    //
    //  Note that we use snprintf() to make sure we have a null character
    //  ('\0') at the end of the string, but since ctime() puts a '\n' at
    //  the end, we change that '\n' into a null character (thus "removing"
    //  the '\n') :
    //
    nowRTC = m_RTC.now().unixtime();

    snprintf ( buf, sizeof(buf), "%s", ctime(&nowRTC) );

    if ( strlen(buf) > 0 )
    {
        buf[strlen(buf)-1] = '\0';
    }

    MyPrintf ( "[RTC::SetRTCFromSystemTime]  RTC time was [%s] UTC.\n", buf );

#endif  // DEBUG HACK -- Display the RTC time for fun.


    //
    //  Write the system time (in UTC) to the RTC :
    //
    m_RTC.adjust ( DateTime(nowUTC) );


#if 1   // DEBUG HACK -- Display the RTC time for fun.

    //
    //  Show the time currently stored in the RTC :
    //
    nowRTC = m_RTC.now().unixtime();

    snprintf ( buf, sizeof(buf), "%s", ctime(&nowRTC) );

    if ( strlen(buf) > 0 )
    {
        buf[strlen(buf)-1] = '\0';
    }

    MyPrintf ( "[RTC::SetRTCFromSystemTime]  RTC time now [%s] UTC.\n", buf );

#endif  // DEBUG HACK -- Display the RTC time for fun.


    //
    //  Return success :
    //
    return ( true );
}


//----------------------------------------------------------------------------
//
//  SetSystemTimeFromRTC () -- Set our system time from the time in the RTC.
//
//----------------------------------------------------------------------------

bool cMyRTC::SetSystemTimeFromRTC ( void )
{
    //
    //  Local variables :
    //
    char        buf[255];
    DateTime    now;
    time_t      rtcTimestamp;


    MyPrintf ( "[RTC::SetSystemTimeFromRTC]  Local time was [%s]\n", myTZ.dateTime().c_str() );


    //
    //  Return now if we havent' initialized communications with the RTC :
    //
    if ( m_IsInitialized == false )
    {
        MyPrintf ( "[RTC::SetSystemTimeFromRTC]  +++ Warning : RTC has not been initialized !!! +++\n" );
        return ( false );
    }


#if 1   // DEBUG HACK -- Display the RTC time for fun.

    //
    //  Show the time currently stored in the RTC :
    //
    rtcTimestamp = m_RTC.now().unixtime();

    snprintf ( buf, sizeof(buf), "%s", ctime(&rtcTimestamp) );

    if ( strlen(buf) > 0 )
    {
        buf[strlen(buf)-1] = '\0';
    }

    MyPrintf ( "[RTC::SetSystemTimeFromRTC]  RTC time is [%s] UTC.\n", buf );

#endif  // DEBUG HACK -- Display the RTC time for fun.


    //
    //  Get the time (in UTC) from the RTC :
    //
    rtcTimestamp = m_RTC.now().unixtime();


    //
    //  Use ezTime's UTC object to set the system time :
    //
    //MyPrintf ( "Before any time operations.\n" );
    //delay ( 1000 );

    UTC.setTime ( rtcTimestamp );

    //MyPrintf ( "After UTC.setTime() call.\n" );
    //delay ( 1000 );

    //myTZ.setTime ( rtcTimestamp );
    //MyPrintf ( "After myTZ.setTime() call.\n" );
    //delay ( 1000 );

    //
    //  Run events() to update our myTZ object :
    //
    events ();

    //MyPrintf ( "After events() call.\n" );
    //delay ( 1000 );


    //MyPrintf("[%s] Master UTC Clock\n", UTC.dateTime().c_str());
    //MyPrintf("[%s] Local Glenview Clock\n", myTZ.dateTime().c_str());

#if 0
    MyPrintf ( "Before setPosix() call.\n" );
    delay ( 1000 );

    // 1. Tell the object WHAT rules to use (CST/CDT)
    // Use setPosix so it doesn't need the internet to know the rules
    //myTZ.setPosix("CST6CDT,M3.2.0,M11.1.0");

    MyPrintf ( "After setPosix() call.\n" );
    delay ( 1000 );

    // 2. Read the UTC timestamp from your RTC
    DateTime now1 = m_RTC.now();
    time_t rtcUTC = now1.unixtime();

    MyPrintf ( "Before setTime() call.\n" );
    delay ( 1000 );

    // 3. Set the GLOBAL master clock only
    UTC.setTime(rtcUTC);

    MyPrintf ( "After setTime() call.\n" );
    delay ( 1000 );

    // 4. Update the internal ezTime states
    events();

    MyPrintf ( "After events() call.\n" );
    delay ( 1000 );

    // 5. Query the Timezone object for the shifted time
    MyPrintf("[%s] Local Time\n", myTZ.dateTime().c_str());
#endif


    MyPrintf ( "[RTC::SetSystemTimeFromRTC]  Local time now [%s]\n", myTZ.dateTime().c_str() );


    //
    //  Return success :
    //
    return ( true );
}


//----------------------------------------------------------------------------
//
//  Debug_Test_001 () -- Test routine # 1.
//
//----------------------------------------------------------------------------

void cMyRTC::Debug_Test_001 ( void )
{
    MyPrintf ( "=== RTC - Debug_Test_001 called ===\n" );


    if ( m_IsInitialized == false )
    {
        if ( !m_RTC.begin() )
        {
            MyPrintf ( "Couldn't find RTC !!!\n" );
            return;
        }

        m_IsInitialized = true;
    }


    if ( m_RTC.lostPower() )
    {
        MyPrintf ( "RTC lost power, let's set the time!\n" );

        // Sets the RTC to the date & time this sketch was compiled
        m_RTC.adjust ( DateTime(F(__DATE__), F(__TIME__)) );
    }


    for ( int i = 0 ; i < 10 ; i++ )
    {
        DateTime now = m_RTC.now();

        MyPrintf ( "  The time is  %2d:%02d:%02d   ",
            now.hour(),
            now.minute (),
            now.second () );

        //
        //  Bonus: The DS3231 has a built-in temp sensor!
        //
        float tempC = m_RTC.getTemperature();
        float tempF = ( (tempC * 1.8) + 32.0 );

        Serial.printf ("Temperature : %0.2f C / %0.2f F\n", tempC, tempF );

        delay ( 1000 );
    } // for

    MyPrintf ( "=== RTC - Debug_Test_001 done ===\n" );
}


//----------------------------------------------------------------------------
//
//  setup () -- Routine to setup our RTC.
//
//----------------------------------------------------------------------------

void cMyRTC::setup ( void )
{
    MyPrintf ( "[RTC::setup]  Called.\n" );


    //
    //  Finish initializing some of our attributes :
    //
    m_IsInitialized = false;


    //
    //  Try to start the RTC :
    //
    if ( !m_RTC.begin() )
    {
        MyPrintf ( "[RTC::setup]  *******************************************\n" );
        MyPrintf ( "[RTC::setup]  *** Error -- Could not find the RTC !!! ***\n" );
        MyPrintf ( "[RTC::setup]  *******************************************\n" );
        return;
    }


    //
    //  See if the RTC has lost power.  If it has, well, there's nothing
    //  we can do about it, so just display a warning and keep going :
    //
    if ( m_RTC.lostPower() )
    {
        MyPrintf ( "[RTC::setup]  +++\n" );
        MyPrintf ( "[RTC::setup]  +++ Warning - RTC has lost power / RTC time may not be correct +++\n" );
        MyPrintf ( "[RTC::setup]  +++\n" );
    }


    //
    //  Just for kicks, display the current time that is stored in the RTC :
    //
    DisplayTime ();


    //
    //  We successfully initialized the RTC, so make note of it :
    //
    m_IsInitialized = true;
}


//----------------------------------------------------------------------------
//
//  handle () -- Routine to check for updates for our RTC.
//
//----------------------------------------------------------------------------

void cMyRTC::handle ( void )
{
    //--------------------------------------------------------------------
    //
    //  Is this the first time we're being called ?
    //
    //--------------------------------------------------------------------

    if ( g_FirstTime == true )
    {
        MyPrintf ( F("[RTC::handle]  Called.\n") );
    }
}


//----------------------------------------------------------------------------
//
//  end of  MyRTC.cpp
//
//----------------------------------------------------------------------------
