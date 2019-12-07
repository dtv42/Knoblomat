// --------------------------------------------------------------------------------------------------------------------
// <copyright file="WiFiInfo.h" company="DTV-Online">
//   Copyright(c) 2019 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// --------------------------------------------------------------------------------------------------------------------
#ifndef _WIFI_INFO_h
#define _WIFI_INFO_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <WiFi.h>

/// <summary>
/// This class holds the actual WiFi connection data.
/// </summary>
class WiFiInfoClass
{
public:
	WiFiInfoClass(WiFiClass wifi);

	String SSID;							// The WiFi SSID
	String PASS;							// The WiFi passphrase
	String Hostname;						// The WiFi hostname
	String NetworkID;						// The WiFi network ID
	String Address;							// The WiFi IP address
	String Gateway;							// The Gateway address
	String Subnet;							// The Subnet mask
	String DNS;								// The domain name server
	int RSSI;								// The signal strength
	String BSSID;							// The MAC address of the router
	String MAC;								// The MAC address

	String serialize();						// Return a string serialization (JSON)
	void print();							// Prints all fields on the serial line
};

#endif

