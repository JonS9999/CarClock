#ifndef _MY_WIFI_H_
#define _MY_WIFI_H_


//
//  System include files :
//
#include <Arduino.h>

//
//  Local include files :
//
#include "Common.h"
#include "MyWiFi_OTA.h"



//----------------------------------------------------------------------------
//
//  Enumerations
//
//----------------------------------------------------------------------------

typedef enum
{
    CON_TYPE_NOT_CONNECTED,             // Not connected to any network.
    CON_TYPE_OPEN,                      // Connected to open (insecure) network.
    CON_TYPE_SECURE,                    // Connected to secure network.
    CON_TYPE_UNKNOWN,                   // Connected to a network, but cannot identify type.
} tWiFiConnectionType;


//-----------------------------------------------------------------------------
//
//  cMyWiFi object
//
//-----------------------------------------------------------------------------

class cMyWiFi
{
  private :

    //
    //  m_IsInitialized -- Flag used track if we've been initialized yet or not.
    //
    bool  m_IsInitialized;

    //
    //  The state of our WiFi connection :
    //
    //    tWiFi_State   m_State;

    //
    //  Flag to track if we're connected to a WiFi network :
    //
    //    bool  m_WiFiIsConnected;

    //
    //  My hostname :
    //
    char  m_MyHostname[40];

    //
    //  My IP address :
    //
    IPAddress m_MyIpAddr;

    //
    //  My SSID :
    //
    char  m_MySSID[40];

    //
    //  SSID of the host we're connected to :
    //
    char  m_HostSSID[40];

    //
    //  Type of connection we have to the hose :
    //
    tWiFiConnectionType m_HostConnectionType;

    //
    //  Type of security of the hose we're connected to :
    //
    uint8_t m_HostEncryptionType;

    //
    //  Routine that we call when we are attempting to connect to the WiFi :
    //
    void (*m_Callback_Connecting) (   const int   index,
                                      const char* ssid );

    //
    //  Routine to set the 'no usable network found' callback :
    //
    void (*m_Callback_NoUsableNetwork) ( void );

    //
    //  Routine to set the 'scanning for networks' callback :
    //
    void (*m_Callback_ScanningForNetworks) ( void );

    //
    //  Routine to set the 'searching for an open network' callback :
    //
    void (*m_Callback_SearchingForOpenNetwork) ( void );

    //
    //  Routine to set the 'searching for a preferred network' callback :
    //
    void (*m_Callback_SearchingForPreferredNetwork) ( void );

    //
    //  Routine that we call when we connect to the WiFi :
    //
    void (*m_CallbackOnConnect) ( void );

    //
    //  Routine that we call when we disconnect from the WiFi :
    //
    void (*m_CallbackOnDisconnect) ( void );


    //
    //  One instance of the WifManager library :
    //
    //    WiFiManager   m_WiFiManager;

    //
    //  One instance of the WiFi MQTT manager (client object) :
    //
#ifdef  WIFI_MQTT
    cMyWiFi_MQTT  m_WiFi_MQTT;
#endif  // WIFI_MQTT

#ifdef  WIFI_NTP
    //
    //  One instance of the WiFi NTP client :
    //
    cMyWiFi_NTP   m_WiFi_NTP;
#endif  // WIFI_NTP

    //
    //  One instance of the WiFi OTA (Over The Air) manager :
    //
    cMyWiFi_OTA   m_WiFi_OTA;


    //-----------------------------------------------------------------

  public :

#if 1   // Make constructor private to force only one instance of it.
    //
    //  Constructor :
    //
    cMyWiFi ( void );
#endif

#if 1   // DEBUG HACK

    //
    //  Routine to connect to a WiFi network :
    //
    bool Connect ( void );

    //
    //  Return the type of connection we have to our host :
    //
    const tWiFiConnectionType GetHostConnectionType ( void );

    //
    //  Return our MAC address in the form AABBCCDDEEFF :
    //
    char*  GetMacAddr     ( void );

    //
    //  Return the high half of our MAC address :
    //
    char*  GetMacAddrHigh ( void );

    //
    //  Return the low half of our MAC address :
    //
    char*  GetMacAddrLow  ( void );

    //
    //  Routine to return the name of this app (including
    //  the last four characters of our MAC address) :
    //
    const char* MyHostname ( void );

    //-------------------------------------------------------

    //
    //  Routine to say if we're connected to a WiFi network :
    //
    bool   IsConnected    ( void );

    //
    //  Routine to set the 'connecting' callback :
    //
    void  SetCallback_Connecting ( void (*ptr) ( const int index, const char* ssid ) );

    //
    //  Routine to set the 'no usable network found' callback :
    //
    void  SetCallback_NoUsableNetwork ( void (*ptr) ( void ) );

    //
    //  Routine to set the 'scanning for networks' callback :
    //
    void  SetCallback_ScanningForNetworks ( void (*ptr) ( void ) );

    //
    //  Routine to set the 'searching for an open network' callback :
    //
    void  SetCallback_SearchingForOpenNetwork ( void (*ptr) ( void ) );

    //
    //  Routine to set the 'searching for a preferred network' callback :
    //
    void  SetCallback_SearchingForPreferredNetwork ( void (*ptr) ( void ) );

    //
    //  Routine to set the 'on connect' callback :
    //
    void  SetCallbackOnConnect ( void (*ptr) ( void ) );

    //
    //  Routine to set the 'on disconnect' callback :
    //
    void  SetCallbackOnDisconnect ( void (*ptr) ( void ) );

    //
    //  Routine to return an ASCII printable representation of our state
    //  or any state :
    //
    //    const char*         State_Str   ( const tWiFi_State State );
    //    inline const char*  State_Str   ( void ) { return ( State_Str ( m_State ) ); }

    //
    //  Routine to reset our stored WiFi parameters :
    //
    //    void ResetSettings ( void );

    //
    //  setup () -- Routine to setup all things WiFi related :
    //
    void setup ( void );

    //
    //  handle () -- Routine to check for WiFi related things :
    //
    void handle ( void );


    //-----------------------------------------------------------------
    //
    //  WiFi inline functions :
    //
    //-----------------------------------------------------------------

    //
    //  Return my hostname :
    //
    inline const char* GetMyHostname ( void )
    {
      return ( (const char*)m_MyHostname );
    }

    //
    //  Return my IP address (as an IPAddress) :
    //
    inline const IPAddress GetMyIpAddr ( void )
    {
      return ( (const IPAddress)m_MyIpAddr );
    }

    //
    //  Return my IP address (as a String) :
    //
    inline const String GetMyIpAddrStr( void )
    {
      return ( (const String)m_MyIpAddr.toString() );
    }

    // return ( WiFi.localIP().toString().c_str()

    //
    //  Return my SSID :
    //
    inline const char* GetMySSID ( void )
    {
      return ( (const char*)m_MySSID );
    }

    //
    //  Return the SSID of the host we're connected to :
    //
    inline const char* GetHostSSID ( void )
    {
      return ( (const char*)m_HostSSID );
    }

    //
    //  Return the encryption type of the host we're connected to :
    //
    inline const uint8_t GetHostEncryptionType ( void )
    {
      return ( (const uint8_t)m_HostEncryptionType );
    }


#ifdef  WIFI_MQTT

    //-----------------------------------------------------------------
    //
    //  MQTT related functions :
    //
    //-----------------------------------------------------------------

    //
    //  mqttClient () -- Return a refernce to our MQTT broker object :
    //
    inline cMyWiFi_MQTT* mqttClient ( void )
    {
      return ( &m_WiFi_MQTT );
    }

    //
    //  MQTT_IsConnected () -- Are we connected to the MQTT broker :
    //
    inline bool MQTT_IsConnected ( void )
    {
      return ( m_WiFi_MQTT.IsConnected () );
    }

    //
    //  MQTT_Publish () -- Send (publish) the specified message to
    //      the MQTT broker :
    //
    inline bool MQTT_Publish (  const char*   mesg,
                                const char*   topic   = NULL )
    {
      return ( m_WiFi_MQTT.Publish ( mesg, topic ) );
    }

    //
    //  MQTT_SetBrokerIp () -- Set the IP address of the MQTT broker :
    //
    inline bool MQTT_SetBrokerIp ( const char* brokerIp )
    {
      return ( m_WiFi_MQTT.SetBrokerIp ( brokerIp ) );
    }

    //
    //  MQTT_SetBrokerPort () -- Set the port number of the MQTT broker :
    //
    inline bool MQTT_SetBrokerPort ( const uint16_t brokerPort )
    {
      return ( m_WiFi_MQTT.SetBrokerPort ( brokerPort ) );
    }

    //
    //  MQTT_SetCallbackMesgRcvd () -- Set the routine that should be
    //      called whenever we get a message from the topic we have
    //      subscribed to :
    //
    inline bool MQTT_SetCallbackMesgRcvd ( void (*ptr) (  const char*         topic,
                                           const uint8_t*      payload,
                                           const unsigned int  length ) )
    {
      return ( m_WiFi_MQTT.SetCallbackMesgRcvd ( ptr ) );
    }

    //
    //  MQTT_SetCallbackOnConnect () -- Set the routine that we should
    //      call when we connect to the MQTT broker :
    //
    inline bool MQTT_SetCallbackOnConnect ( void (*ptr) ( void ) )
    {
      return ( m_WiFi_MQTT.SetCallbackOnConnect ( ptr ) );
    }

    //
    //  MQTT_SetCallbackOnDisconnect () -- Set the routine that we should
    //      call when we disconnect from the MQTT broker :
    //
    inline bool MQTT_SetCallbackOnDisconnect ( void (*ptr) ( void ) )
    {
      return ( m_WiFi_MQTT.SetCallbackOnDisconnect ( ptr ) );
    }

    //
    //  MQTT_SetClientId () -- Set the client ID :
    //
    inline bool MQTT_SetClientId ( const char* clientId )
    {
      return ( m_WiFi_MQTT.SetClientId ( clientId ) );
    }

    //
    //  MQTT_SetPubTopic () -- Set the 'publish' topic :
    //
    inline bool MQTT_SetPubTopic ( const char* publishTopic )
    {
      return ( m_WiFi_MQTT.SetPubTopic ( publishTopic ) );
    }

    //
    //  MQTT_SetSubTopic () -- Set the 'subscribe' topic :
    //
    inline bool MQTT_SetSubTopic ( const char* subscribeTopic )
    {
      return ( m_WiFi_MQTT.SetSubTopic ( subscribeTopic ) );
    }

    //
    //  MQTT_WiFiNowConnected () -- Routine that is called when we
    //      connect to the WiFi :
    //
    inline bool MQTT_WiFiNowConnected ( void )
    {
      return ( m_WiFi_MQTT.WiFiNowConnected () );
    }

    //
    //  MQTT_WiFiNowDisconnected () -- Routine that is called when
    //      we disconnect from the WiFi :
    //
    inline bool MQTT_WiFiNowDisconnected ( void )
    {
      return ( m_WiFi_MQTT.WiFiNowDisconnected () );
    }

    //
    //  MQTT_Start () -- Start the MQTT client.  Note that the
    //      user should set all the attributes (broker IP,
    //      publish and subscription topics, etc) before calling
    //      MQTT_Start() :
    //
    inline bool MQTT_Start ( void )
    {
      return ( m_WiFi_MQTT.Start () );
    }

    //
    //  Stop () -- Stop the MQTT client.  This is typically called when
    //      we lose WiFi connection :
    //
    inline bool MQTT_Stop ( void )
    {
      return ( m_WiFi_MQTT.Stop () );
    }

#endif  // WIFI_MQTT


#ifdef  WIFI_NTP

    //-----------------------------------------------------------------
    //
    //  NTP related functions :
    //
    //-----------------------------------------------------------------

    //
    //  ntpClient () -- Return a refernce to our NTP client object :
    //
    inline cMyWiFi_NTP* ntpClient ( void )
    {
      return ( &m_WiFi_NTP );
    }

    //
    //  NTP_GetAsciiTime () -- Routine to return the user specified
    //      time in an ASCII printable format :
    //
    inline const char* NTP_GetAsciiTime ( const time_t userTime )
    {
      return ( m_WiFi_NTP.GetAsciiTime ( userTime ) );
    }

    //
    //  NTP_GetLocalTime () -- Routine to return the time in local time :
    //
    inline time_t NTP_GetLocalTime ( void )
    {
      return ( m_WiFi_NTP.GetLocalTime() );
    }

    //
    //  NTP_GetUtcTime () -- Routine to return the UTC (Epoch) time :
    //
    inline time_t NTP_GetUtcTime ( void )
    {
      return ( m_WiFi_NTP.GetUtcTime() );
    }

    //
    //  NTP_SetDstOffset () -- Routine to set the Daylight Saving Time (DST)
    //      offset from UTC in minutes :
    //
    inline bool NTP_SetDstOffset ( time_t offset )
    {
      return ( m_WiFi_NTP.SetDstOffset( offset ) );
    }

    //
    //  NTP_WiFiNowConnected () -- Routine that is called when we
    //      connect to the WiFi :
    //
    inline bool NTP_WiFiNowConnected ( void )
    {
      return ( m_WiFi_NTP.WiFiNowConnected () );
    }

    //
    //  NTP_WiFiNowDisconnected () -- Routine that is called when
    //      we disconnect from the WiFi :
    //
    inline bool NTP_WiFiNowDisconnected ( void )
    {
      return ( m_WiFi_NTP.WiFiNowDisconnected () );
    }

    //
    //  NTP_SetStdOffset () -- Routine to set the "standard time" offset from
    //      UTC in minutes :
    //
    inline bool NTP_SetStdOffset ( time_t offset )
    {
      return ( m_WiFi_NTP.SetStdOffset( offset ) );
    }

    //
    //  NTP_Start () -- Start the NTP client.  Note that the user
    //      should set all the attributes (NTP server info, etc) before
    //      calling NTP_Start() :
    //
    inline bool NTP_Start ( void )
    {
      return ( m_WiFi_NTP.Start () );
    }

    //
    //  Stop () -- Stop the NTP client.  This is typically called when
    //      we lose WiFi connection :
    //
    inline bool NTP_Stop ( void )
    {
      return ( m_WiFi_NTP.Stop () );
    }

#endif  // WIFI_NTP


    //-----------------------------------------------------------------
    //
    //  OTA (Over The Air) programming related functions :
    //
    //-----------------------------------------------------------------

    inline bool OTA_IsUpdating ( void )
    {
      return ( m_WiFi_OTA.isUpdating() );
    }

    inline uint16_t OTA_PercentComplete ( void )
    {
      return ( m_WiFi_OTA.percentComplete() );
    }

    inline void OTA_SetCallbackOnStart ( void (*ptr) ( void ) )
    {
      m_WiFi_OTA.SetCallbackOnStart ( ptr );
    }

    inline void OTA_SetCallbackOnEnd ( void (*ptr) ( void ) )
    {
      m_WiFi_OTA.SetCallbackOnEnd ( ptr );
    }

    inline void OTA_SetCallbackOnError ( void (*ptr) ( void ) )
    {
      m_WiFi_OTA.SetCallbackOnError ( ptr );
    }

    inline void OTA_SetCallbackOnProgress ( void (*ptr) ( unsigned int progress, unsigned int total ) )
    {
      m_WiFi_OTA.SetCallbackOnProgress ( ptr );
    }

    inline void OTA_Setup ( void )
    {
      m_WiFi_OTA.setup ( m_MyHostname );
    }

#endif  // DEBUG HACK

};  // cMyWiFi


//----------------------------------------------------------------------------
//
//  Global routines that aren't part of cMyWiFi :
//
//----------------------------------------------------------------------------

extern bool connectWifi(void);
extern void displayStoredSSIDs(void);
extern void displayVisibleNetworks(void);
extern void displayWifiStatus(void);

#endif // !_MY_WIFI_H_
