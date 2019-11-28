// --------------------------------------------------------------------------------------------------------------------
// <copyright file="WiFiSettings.h" company="DTV-Online">
//   Copyright(c) 2019 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// --------------------------------------------------------------------------------------------------------------------
#ifndef _WIFI_SETTINGS_h
#define _WIFI_SETTINGS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <Preferences.h>

/// <summary>
/// 
/// </summary>
class WiFiSettingsClass
{
private:
	const char* NAMESPACE = "WiFi";				// The namspace used in preferences
	const char* KEY_SSID = "SSID";				// The preference key for the SSID field
	const char* KEY_PASS = "PASS";				// The preference key for the passphrase field
	const char* KEY_HOSTNAME = "Hostname";		// The preference key for the Hostname field
	const char* KEY_DHCP = "DHCP";				// The preference key for the DHCP field
	const char* KEY_ADDRESS = "Address";		// The preference key for the Address field
	const char* KEY_GATEWAY = "Gateway";		// The preference key for the Gateway field
	const char* KEY_SUBNET = "Subnet";			// The preference key for the SubnetMask field
	const char* KEY_DNS1 = "DNS1";				// The preference key for the DNS1 field
	const char* KEY_DNS2 = "DNS2";				// The preference key for the DNS2 field

	const char* WIFI_HOSTNAME = "knoblomat";	// The default access point hostname
	const char* SUBNET_MASK = "255.255.255.0";	// The default network mask

	const int MAX_SSID_LEN = 32;				// The maximum length for the SSID
	const int MAX_PASS_LEN = 64;				// The maximum length for the PASS
	const int MAX_HOSTNAME_LEN = 32;			// The maximum length for the hostname
	const int MAX_IPADDRESS_LEN = 15;			// The maximum length for an IP address

	Preferences preferences;					// The EPS32 preferences instance

public:
	WiFiSettingsClass();

	String SSID;								// The WiFi SSID
	String PASS;								// The WiFi Passphrase
	String Hostname;							// The WiFi Hostname
	bool DHCP;									// The WiFi DHCP mode (default: true)
	String Address;								// The static Address
	String Gateway;								// The Gateway address
	String Subnet;								// The SubnetMask
	String DNS1;								// The primary domain name server
	String DNS2;								// The secondary domain name server

	bool deserialize(String settings);			// Read a JSON string and updates the fields.
	String serialize();							// Return a string serialization (JSON)
	void clear();								// Clears the persistent storage
	void save();								// Save the fields to storage
	void init();								// Initializes the fields from storage
};

#endif

