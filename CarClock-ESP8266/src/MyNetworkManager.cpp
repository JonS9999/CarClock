//
//  System include files :
//
#include <Arduino.h>
#include <ESP8266WiFi.h>    // Wifi for ESP8266.


//
//  Local include files :
//
#include "MyNetworkManager.h"
#include "config.h"

//----------------------------------------------------------------------------

bool connectWifi ( void )
{
    //
    //  Local variables :
    //
    char buf[30];                   // Temporary text buffer.


    MyPrintf ( "[WiFi]  Scanning for networks...\n" );
    int n = WiFi.scanNetworks();
    MyPrintf ( "[WiFi]  Found %d networks.\n", n );

#if 1
    MyPrintf ( "[WiFi]           En                SSID               Sec\n" );
    MyPrintf ( "[WiFi]           -- - ------------------------------  ---\n" );

    for ( int i = 0 ; i < n ; i++ )
    {
        snprintf ( buf, sizeof(buf), "[%s]", WiFi.SSID(i).c_str() );

        MyPrintf ( "[WiFi]    SSID # %2d : %-30s   %c\n",
            i,
            buf,
            ( (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? ' ' : '*') );
    }
#endif

    if (n <= 0) {
        MyPrintf ( "[WiFi]  No networks found.\n" );
        return false;
    }

    // Loop through our 4 slots
    for (int slot = 0; slot < MAX_WIFI_NETWORKS; slot++)
    {
        if (strlen(config.wifiSlots[slot].ssid) == 0) continue;

        MyPrintf ( "[WiFI]  Looking for SSID [%s]...\n", config.wifiSlots[slot].ssid );

        // Check if our stored SSID is in the scan results
        for (int i = 0; i < n; i++) {
            if (WiFi.SSID(i) == String(config.wifiSlots[slot].ssid))
            {
                MyPrintf ( "[WiFi]  Found priority %d: [%s].  Connecting...", slot + 1, config.wifiSlots[slot].ssid );

                WiFi.begin(config.wifiSlots[slot].ssid, config.wifiSlots[slot].pass);

                // Wait up to 10s for connection
                unsigned long start = millis();
                while ( (WiFi.status() != WL_CONNECTED) && ((millis() - start) < 10000)) {
                    delay(500);
                    Serial.print(".");
                }
                Serial.printf ( "\n" );

                if (WiFi.status() == WL_CONNECTED) {
                    MyPrintf ( "[WiFi]  Connected!\n" );
                    config.isConnected = true;
                    displayWifiStatus();
                    return true;
                }
                MyPrintf ( "[WiFi]  Connection failed.\n" );
            }
        }
    }

    config.isConnected = false;

    return false;
}

//----------------------------------------------------------------------------

void displayStoredSSIDs(void) {

    MyPrintf ( "--- Stored WiFi Priorities ---\n" );

    for (int i = 0; (i < MAX_WIFI_NETWORKS); ++i) {
        MyPrintf ( "Slot %d: ", (i + 1) );
        if ( (strlen(config.wifiSlots[i].ssid) > 0) ) {
            Serial.printf("SSID: [%s]\n", config.wifiSlots[i].ssid);
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
//  end of  MyNetworkManager.cpp
//
//----------------------------------------------------------------------------
