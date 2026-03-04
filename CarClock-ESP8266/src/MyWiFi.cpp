//----------------------------------------------------------------------------
//
//  MyWiFi.cpp --
//
//      Version 1.0
//
//----------------------------------------------------------------------------


//
//  System include files :
//
#include <Arduino.h>
#include <ESP8266WiFi.h>    // Wifi for ESP8266.


//
//  Local include files :
//
#include "Common.h"
#include "MyWiFi.h"


//----------------------------------------------------------------------------
//
//  Constructor
//
//----------------------------------------------------------------------------

cMyWiFi::cMyWiFi ( void )
{
  MyPrintf ( "< cMyWiFi constructor called >\n" );


  //
  //  Set our flag to show that we have not yet been initialized :
  //
  m_IsInitialized = false;


  //
  //  Things we want to make sure we do only once :
  //
  //  m_State                               = WIFI_STATE_UNKNOWN;
  m_MyHostname[0]                           = '\0';
  //m_MyIpAddr                                = ( 0, 0, 0, 0 );
  m_MySSID[0]                               = '\0';
  m_Callback_Connecting                     = NULL;
  m_Callback_NoUsableNetwork                = NULL;
  m_Callback_ScanningForNetworks            = NULL;
  m_Callback_SearchingForOpenNetwork        = NULL;
  m_Callback_SearchingForPreferredNetwork   = NULL;
  m_CallbackOnConnect                       = NULL;
  m_CallbackOnDisconnect                    = NULL;
}


//----------------------------------------------------------------------------
//
//  Connect () -- Routine to connect to a WiFi network.
//
//----------------------------------------------------------------------------

bool cMyWiFi::Connect ( void )
{
    //
    //  Local variables :
    //
    char    buf[30];                        // Temporary text buffer.
    bool    firstTime       = true;         // First time we try something?


    //-----------------------------------------------------------------------
    //
    //  Call the optional user specified 'scanning for networks' callback :
    //
    //-----------------------------------------------------------------------

    if ( m_Callback_ScanningForNetworks != NULL )
    {
        MyPrintf ( "[WiFi::Connect]  +++ Calling 'scanning for networks' callback +++\n" );
        (*m_Callback_ScanningForNetworks) ();
        MyPrintf ( "[WiFi::Connect]  +++ Back from 'scanning for networks' callback +++\n" );
    }


    //-----------------------------------------------------------------------
    //
    //  Now get a list of networks we can see :
    //
    //-----------------------------------------------------------------------

    MyPrintf ( "[WiFi::Connect]  Scanning for networks...\n" );
    int n = WiFi.scanNetworks();
    MyPrintf ( "[WiFi::Connect]  Found %d networks.\n", n );

#if 1
    MyPrintf ( "[WiFi::Connect]           En                SSID               RSSI  Sec\n" );
    MyPrintf ( "[WiFi::Connect]           -- - ------------------------------  ----  ---\n" );

    for ( int i = 0 ; i < n ; i++ )
    {
        snprintf ( buf, sizeof(buf), "[%s]", WiFi.SSID(i).c_str() );

        MyPrintf ( "[WiFi::Connect]    SSID # %2d : %-30s  %4d   %c\n",
            i,
            buf,
            WiFi.RSSI(i),
            ( (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? ' ' : '*') );
    }
#endif

    if ( n <= 0 )
    {
        MyPrintf ( "[WiFi::Connect]  No networks found.\n" );
        return ( false );
    }


    //
    //  Be sure to set "firstTime" to 'true' so we display some initial
    //  messages on the display :
    //
    firstTime = true;


    //
    //  Loop through our list of preferred networks :
    //
    for (int slot = 0 ; slot < NUM_WIFI_PREFERRED ; slot++ )
    {
        //-----------------------------------------------------------------------
        //
        //  Do some minor sanity checks before we try to connect to a
        //  network :
        //
        //-----------------------------------------------------------------------

        if ( strlen(WIFI_PREFERRED[slot].SSID) == 0 ) continue;


        //-----------------------------------------------------------------------
        //
        //  If this is the first time we're attempting to look for a preferred
        //  network, then call the optional callback :
        //
        //-----------------------------------------------------------------------

        if ( (firstTime == true) && (m_Callback_SearchingForPreferredNetwork != NULL) )
        {
            MyPrintf ( "[WiFi::Connect]  +++ Calling 'searching for a preferred network' callback +++\n" );
            (*m_Callback_SearchingForPreferredNetwork) ();
            MyPrintf ( "[WiFi::Connect]  +++ Back from 'searching for a preferred network' callback +++\n" );

            firstTime =  false;
        }

        MyPrintf ( "[WiFi::Connect]  Looking for SSID [%s]...\n", WIFI_PREFERRED[slot].SSID );

        // Check if our stored SSID is in the scan results
        for ( int i = 0; i < n; i++ )
        {
            if ( WiFi.SSID(i) == String(WIFI_PREFERRED[slot].SSID) )
            {
                //
                //  Only attempt to connect if our preferred network
                //  secure/open setup matches the secure/open setting
                //  of the network we found :
                //
                if ( (strlen(WIFI_PREFERRED[slot].passwd) > 0) &&   // Preferred network is secure (has a password).
                     (WiFi.encryptionType(i) != ENC_TYPE_NONE) )    // Found network is secure (not open).
                {
                    //
                    //  Call the optional user specified 'connecting' callback :
                    //
                    if ( m_Callback_Connecting != NULL )
                    {
                        MyPrintf ( "[WiFi::Connect]  +++ Calling 'connecting' callback +++\n" );
                        (*m_Callback_Connecting) ( i, WIFI_PREFERRED[slot].SSID );
                        MyPrintf ( "[WiFi::Connect]  +++ Back from 'connecting' callback +++\n" );
                    }

                    MyPrintf ( "[WiFi::Connect]  Found priority %d: [%s].  Connecting...", (slot + 1), WIFI_PREFERRED[slot].SSID );

                    WiFi.begin(WIFI_PREFERRED[slot].SSID, WIFI_PREFERRED[slot].passwd);

                    // Wait up to 10s for connection
                    unsigned long start = millis();
                    while ( (WiFi.status() != WL_CONNECTED) && ((millis() - start) < 10000)) {
                        delay(500);
                        Serial.print(".");
                    }
                    Serial.printf ( "\n" );

                    if ( WiFi.status() == WL_CONNECTED )
                    {
                        MyPrintf ( "[WiFi::Connect]  Connected!\n" );
                        config.isConnected = true;
                        displayWifiStatus();
                        return ( true );
                    }
                    MyPrintf ( "[WiFi::Connect]  Connection failed.\n" );
                }
                else
                {
                    MyPrintf ( "[WiFi::Connect]  Skipping [%s] : Preferred is %s / Found is %s.\n",
                        WIFI_PREFERRED[slot].SSID,
                        ( (strlen(WIFI_PREFERRED[slot].passwd) > 0) ? "SECURE" : "OPEN" ),
                        ( (WiFi.encryptionType(i) != ENC_TYPE_NONE) ? "SECURE" : "OPEN" ) );
                }
            } // if
        } // for
    } // for


    //-----------------------------------------------------------------------
    //
    //  If we reach this point, then that means we've struck out on finding
    //  any of our preferred networks.  Therefore, we look for open networks
    //  in hopes of finding one that is connected to the Internet.
    //
    //-----------------------------------------------------------------------

    //
    //  Be sure to set "firstTime" to 'true' so we display some initial
    //  messages on the display :
    //
    firstTime = true;


    //
    //  Walk through the list of networks we found, and try to find
    //  one that is an open network :
    //
    for ( int i = 0 ; i < n ; i++ )
    {
        //
        //  Is this an open network ?
        //
        if ( WiFi.encryptionType(i) == ENC_TYPE_NONE )
        {
            if ( (firstTime == true) && (m_Callback_SearchingForOpenNetwork != NULL) )
            {
                MyPrintf ( "[WiFi::Connect]  +++ Calling 'searching for an open network' callback +++\n" );
                (*m_Callback_SearchingForOpenNetwork) ();
                MyPrintf ( "[WiFi::Connect]  +++ Back from 'searching for an open network' callback +++\n" );

                firstTime =  false;
            }


            //
            //  Call the optional user specified 'connecting' callback :
            //
            if ( m_Callback_Connecting != NULL )
            {
                MyPrintf ( "[WiFi::Connect]  +++ Calling 'connecting' callback +++\n" );
                (*m_Callback_Connecting) ( i, WiFi.SSID(i).c_str() );
                MyPrintf ( "[WiFi::Connect]  +++ Back from 'connecting' callback +++\n" );

                WiFi.begin ( WiFi.SSID(i).c_str() );

                // Wait up to 10s for connection
                unsigned long start = millis();
                while ( (WiFi.status() != WL_CONNECTED) && ((millis() - start) < 10000)) {
                    delay(500);
                    Serial.print(".");
                }
                Serial.printf ( "\n" );

                if ( WiFi.status() == WL_CONNECTED )
                {
                    MyPrintf ( "[WiFi::Connect]  Connected!\n" );
                    config.isConnected = true;
                    displayWifiStatus();
                    return ( true );
                }
                MyPrintf ( "[WiFi::Connect]  Connection failed.\n" );
            }
        }
    } // for


    //-----------------------------------------------------------------------
    //
    //  Finally, if we're reached this point, then we've exhausted all of
    //  our options, so we just bail :
    //
    //-----------------------------------------------------------------------

    config.isConnected = false;

    if ( m_Callback_NoUsableNetwork != NULL )
    {
        MyPrintf ( "[WiFi::Connect]  +++ Calling 'no usable network found' callback +++\n" );
        (*m_Callback_NoUsableNetwork) ();
        MyPrintf ( "[WiFi::Connect]  +++ Back from 'no usable network found' callback +++\n" );
    }

    return ( false );
}


//----------------------------------------------------------------------------
//
//   GetMacAddr () -- Return our MAC address in the form AABBCCDDEEFF.
//
//----------------------------------------------------------------------------

char* cMyWiFi::GetMacAddr ( void )
{
  //
  //  Local variables :
  //
  static bool   firstTime       = true;
  static char   myMacAddr[MAC_ADDR_SIZE];


  //
  //  If this is the first time we're being called, then we need to fetch
  //  the MAC address and format it.  From then on, we'll just return the
  //  formatted MAC address (since it will never change during runtime) :
  //
  if ( firstTime == true )
  {
    char  tempBuf[MAC_ADDR_SIZE + 6];
    char* srcPtr;
    char* dstPtr;

    WiFi.macAddress().toCharArray ( tempBuf, sizeof(tempBuf) );
    tempBuf[sizeof(tempBuf) - 1] = '\0';
    srcPtr = tempBuf;
    dstPtr = myMacAddr;

    *dstPtr = '\0';

    while ( (*srcPtr != '\0') && (strlen(myMacAddr) < (sizeof(myMacAddr) - 1)) )
    {
      //
      //  Only copy hexadecimal digits (0-9A-F) :
      //
      if ( (isdigit((int)*srcPtr) == true) || ( (*srcPtr >= 'A') && (*srcPtr <= 'F') ) )
      {
        *dstPtr = *srcPtr;
        dstPtr++;
        *dstPtr = '\0';
      }
      srcPtr++;

    } // while

    firstTime = false;

  } // if


  //
  //  Simply return a pointer to the formatted MAC address :
  //
  return ( myMacAddr );
}


//----------------------------------------------------------------------------
//
//   GetMacAddrHigh () -- Return the high half of our MAC address.
//
//      Example :
//
//          Our MAC = AA:BB:CC:DD:EE:FF
//
//          We return AABBCC.
//
//----------------------------------------------------------------------------

char* cMyWiFi::GetMacAddrHigh ( void )
{
  //
  //  Local variables :
  //
  static bool   firstTime       = true;
  static char   myMacAddrHigh[8];


  //
  //  If this is the first time we're being called, then we need to fetch
  //  the MAC address and format it.  From then on, we'll just return the
  //  formatted MAC address (since it will never change during runtime) :
  //
  if ( firstTime == true )
  {
    //
    //  Get the whole MAC adderss :
    //
    strncpy ( myMacAddrHigh, GetMacAddr(), sizeof(myMacAddrHigh) );
    myMacAddrHigh[sizeof(myMacAddrHigh) - 1] = '\0';

    //
    //  Now cut the string so we only use the first 6 characters :
    //
    myMacAddrHigh[6] = '\0';// "aabbcc".

    firstTime = false;
  } // if


  //
  //  Simply return a pointer to the formatted high portion of our MAC address :
  //
  return ( myMacAddrHigh );
}


//----------------------------------------------------------------------------
//
//   GetMacAddrLow () -- Return the low half of our MAC address.
//
//      Example :
//
//          Our MAC = AA:BB:CC:DD:EE:FF
//
//          We return DDEEFF.
//
//----------------------------------------------------------------------------

char* cMyWiFi::GetMacAddrLow ( void )
{
  //
  //  Local variables :
  //
  static bool   firstTime       = true;
  static char   myMacAddrLow[8];


  //
  //  If this is the first time we're being called, then we need to fetch
  //  the MAC address and format it.  From then on, we'll just return the
  //  formatted MAC address (since it will never change during runtime) :
  //
  if ( firstTime == true )
  {
    char  myMacAddr[MAC_ADDR_SIZE];

    //
    //  Get the whole MAC adderss :
    //
    strncpy ( myMacAddr, GetMacAddr(), sizeof(myMacAddr) );
    myMacAddr[sizeof(myMacAddr) - 1] = '\0';

    //
    //  Now copy only the last 6 characters of the MAC address :
    //
    strncpy ( myMacAddrLow, &myMacAddr[6], sizeof(myMacAddrLow) );
    myMacAddrLow[sizeof(myMacAddrLow) - 1] = '\0';

    firstTime = false;
  } // if


  //
  //  Simply return a pointer to the formatted low portion of our MAC address :
  //
  return ( myMacAddrLow );
}


//----------------------------------------------------------------------------
//
//  MyHostname () -- Routine to return the name of this app (including the
//                   last four characters of our MAC address).
//
//----------------------------------------------------------------------------

const char* cMyWiFi::MyHostname ( void )
{
    //
    //  Local variables :
    //
    static char     s_MyHostname[60];           // My name + low four MAC address characters.


    //
    //  Generate our hostname based on the last few digits of our MAC :
    //
    snprintf ( s_MyHostname, sizeof(s_MyHostname), "%s-%s",
        PROJECT_NAME,                                           // Common.h
        cMyWiFi::GetMacAddrLow() );                                      // WiFi.cpp


    //
    //  Return a pointer to our name :
    //
    return ( s_MyHostname );
}


//-----------------------------------------------------------------------------
//
//  Routine to set the 'connecting' callback :
//
//-----------------------------------------------------------------------------

void  cMyWiFi::SetCallback_Connecting ( void (*ptr) ( const int index, const char* ssid ) )
{
  MyPrintf ( "[WiFi::SetCallback_Connecting] : Called : Callback = 0x%08X.\n", ( (ptr != NULL) ? ptr : 0x0000 ) );

  m_Callback_Connecting = ptr;
}


//-----------------------------------------------------------------------------
//
//  Routine to set the 'no usable network found' callback :
//
//-----------------------------------------------------------------------------

void  cMyWiFi::SetCallback_NoUsableNetwork ( void (*ptr) ( void ) )
{
  MyPrintf ( "[WiFi::SetCallback_NoUsableNetwork] : Called : Callback = 0x%08X.\n", ( (ptr != NULL) ? ptr : 0x0000 ) );

  m_Callback_NoUsableNetwork = ptr;
}


//-----------------------------------------------------------------------------
//
//  Routine to set the 'scanning for networks' callback :
//
//-----------------------------------------------------------------------------

void  cMyWiFi::SetCallback_ScanningForNetworks ( void (*ptr) ( void ) )
{
  MyPrintf ( "[WiFi::SetCallback_ScanningForNetworks] : Called : Callback = 0x%08X.\n", ( (ptr != NULL) ? ptr : 0x0000 ) );

  m_Callback_ScanningForNetworks = ptr;
}


//-----------------------------------------------------------------------------
//
//  Routine to set the 'searching for an open network' callback :
//
//-----------------------------------------------------------------------------

void  cMyWiFi::SetCallback_SearchingForOpenNetwork ( void (*ptr) ( void ) )
{
  MyPrintf ( "[WiFi::SetCallback_SearchingForOpenNetwork] : Called : Callback = 0x%08X.\n", ( (ptr != NULL) ? ptr : 0x0000 ) );

  m_Callback_SearchingForOpenNetwork = ptr;
}


//-----------------------------------------------------------------------------
//
//  Routine to set the 'searching for a preferred network' callback :
//
//-----------------------------------------------------------------------------

void  cMyWiFi::SetCallback_SearchingForPreferredNetwork ( void (*ptr) ( void ) )
{
  MyPrintf ( "[WiFi::SetCallback_SearchingForPreferredNetwork] : Called : Callback = 0x%08X.\n", ( (ptr != NULL) ? ptr : 0x0000 ) );

  m_Callback_SearchingForPreferredNetwork = ptr;
}


//-----------------------------------------------------------------------------
//
//  Routine to set the 'on connect' callback :
//
//-----------------------------------------------------------------------------

void  cMyWiFi::SetCallbackOnConnect ( void (*ptr) ( void ) )
{
  MyPrintf ( "[WiFi::SetCallbackOnConnect] : Called : Callback = 0x%08X.\n", ( (ptr != NULL) ? ptr : 0x0000 ) );

  m_CallbackOnConnect = ptr;
}


//-----------------------------------------------------------------------------
//
//  Routine to set the 'on disconnect' callback :
//
//-----------------------------------------------------------------------------

void  cMyWiFi::SetCallbackOnDisconnect ( void (*ptr) ( void ) )
{
  MyPrintf ( "[WiFi::SetCallbackOnDisconnect] : Called : Callback = 0x%08X.\n", ( (ptr != NULL) ? ptr : 0x0000 ) );

  m_CallbackOnDisconnect = ptr;
}


//----------------------------------------------------------------------------
//
//  setup () -- Routine to setup all things WiFi related.
//
//----------------------------------------------------------------------------

void cMyWiFi::setup ( void )
{
    MyPrintf ( "[WiFi::setup]  Called.\n" );


    //
    //  Initialize our OTA object :
    //
    m_WiFi_OTA.setup ( MyHostname() );
}


//----------------------------------------------------------------------------
//
//  handle () -- Routine to check for WiFi changes.
//
//----------------------------------------------------------------------------

void cMyWiFi::handle ( void )
{
  //
  //  Local variables :
  //
#if 0   // OLD CODE
  bool              nowIsConnected      = IsConnected();                    // Are we currently connected?
  time_t            curMSecs            = millis();                         // Current time in milli-seconds.
  time_t            curDeciSecs         = MSECS_TO_DECI_SECS ( curMSecs );  // Current time in deci-seconds.
  time_t            curSecs             = MSECS_TO_SECS ( curMSecs );       // Current time in seconds.
  static  bool      oldIsConnected      = false;                            // Were we connect the last time we were called?
  static  time_t    oldDeciSecs         = 0;                                // Number of deci-seconds the last time we were called.
  static  time_t    oldSecs             = 0;                                // Number of seconds the last time we were called.
  static  uint32_t  debounceDeciSecs    = 0;                                // Used to "debounce" detecting WiFi connection.  1 deci-second = 0.1 seconds.
  const   uint32_t  DEBOUNCE_DECI_SECS  = 30;                               // Number of deci-seconds (0.1 seconds) for debouncing.
#endif


  //--------------------------------------------------------------------
  //
  //  Is this the first time we're being called ?
  //
  //--------------------------------------------------------------------

  if ( g_FirstTime == true )
  {
    MyPrintf ( F("[WiFi::handle]  Called.\n") );
  }


    //
    //  Call our OTA handler :
    //
    m_WiFi_OTA.handle ();
}


//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

void displayStoredSSIDs(void) {

    MyPrintf ( "--- Stored WiFi Priorities ---\n" );

    for (int i = 0; (i < NUM_WIFI_PREFERRED); ++i) {
        MyPrintf ( "Slot %d: ", (i + 1) );
        if ( (strlen(WIFI_PREFERRED[i].SSID) > 0) ) {
            Serial.printf("SSID: [%s]\n", WIFI_PREFERRED[i].SSID);
        } else {
            Serial.println("<EMPTY>");
        }
    }
    MyPrintf ( "------------------------------\n") ;
}

//----------------------------------------------------------------------------

void displayVisibleNetworks ( void )
{
    //
    //  Local variables :
    //
    char buf[30];                   // Temporary text buffer.


    MyPrintf ( "[displayVisibleNetworks]  Starting scan...\n" );

    // WiFi.scanNetworks returns the number of networks found
    int n = WiFi.scanNetworks();

    MyPrintf ( "[displayVisibleNetworks]  Scan complete.\n" );

    MyPrintf ( "[displayVisibleNetworks]  %d networks found.\n", n );

    for (int i = 0; i < n; ++i)
    {
        // Print SSID and RSSI for each network found
        // RSSI: -30 to -60 is excellent, -90 is very poor.
        snprintf ( buf, sizeof(buf), "[%s]", WiFi.SSID(i).c_str() );

        MyPrintf ( "[displayVisibleNetworks]     %2d : %-15s  (%d dBm) %s\n",
            ( i + 1 ),
            buf,
            WiFi.RSSI(i),
            (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");

        delay(10);
    }

    // Clean up scan results from memory
    WiFi.scanDelete();
}

//----------------------------------------------------------------------------

void displayWifiStatus(void)
{
    if ( (WiFi.status() == WL_CONNECTED) )
    {
        MyPrintf ( "--- Current Network Status ---\n" );
        MyPrintf ( "SSID:       %s\n", WiFi.SSID().c_str() );
        MyPrintf ( "IP Address: %s\n", WiFi.localIP().toString().c_str()) ;
        MyPrintf ( "Subnet:     %s\n", WiFi.subnetMask().toString().c_str() );
        MyPrintf ( "Gateway:    %s\n", WiFi.gatewayIP().toString().c_str() );
        MyPrintf ( "RSSI:       %d dBm\n", WiFi.RSSI()) ;
        MyPrintf ( "------------------------------\n" );
    }
    else
    {
        MyPrintf ( "[displayWifiStatus]  WiFi not connected.\n" );
    }
}


//----------------------------------------------------------------------------
//
//  end of  MyWifi.cpp
//
//----------------------------------------------------------------------------
