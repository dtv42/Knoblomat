// --------------------------------------------------------------------------------------------------------------------
// <copyright file="ApSettings.h" company="DTV-Online">
//   Copyright(c) 2019 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// --------------------------------------------------------------------------------------------------------------------
#ifndef _AP_SETTINGS_h
#define _AP_SETTINGS_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <Preferences.h>

/// <summary>
/// 
/// </summary>
class ApSettingsClass
{
private:
	const char* NAMESPACE = "AP";				// The namspace used in preferences
	const char* KEY_SSID = "SSID";				// The preference key for the SSID field
	const char* KEY_PASS = "PASS";				// The preference key for the passphrase field
	const char* KEY_CUSTOM = "Custom";			// The preference key for the Custom field
	const char* KEY_HOSTNAME = "Hostname";		// The preference key for the Hostname field
	const char* KEY_ADDRESS = "Address";		// The preference key for the Address field
	const char* KEY_GATEWAY = "Gateway";		// The preference key for the Gateway field
	const char* KEY_SUBNET = "Subnet";			// The preference key for the SubnetMask field

	const char* SUBNET_MASK = "255.255.255.0";	// The default network mask

	const int MAX_SSID_LEN = 32;				// The maximum length for the SSID
	const int MAX_PASS_LEN = 64;				// The maximum length for the PASS
	const int MAX_HOSTNAME_LEN = 32;			// The maximum length for the hostname
	const int MAX_IPADDRESS_LEN = 15;			// The maximum length for an IP address

	Preferences preferences;					// The EPS32 preferences instance

public:
	static char* WIFI_SSID_AP;					// The default access point SSID
	static char* WIFI_HOSTNAME;					// The default access point hostname

	ApSettingsClass();

	String SSID;								// The WiFi Access Point SSID
	String PASS;								// The WiFi Access Point Passphrase
	bool Custom;								// The WiFi Custom mode (default: false)
	String Hostname;							// The WiFi Access Point Hostname
	String Address;								// The WiFi Access Point Address
	String Gateway;								// The Gateway address
	String Subnet;								// The SubnetMask

	bool deserialize(String settings);			// Read a JSON string and updates the fields.
	String serialize();							// Return a string serialization (JSON)
	void clear();								// Clears the persistent storage
	void save();								// Save the fields to storage
	void init();								// Initializes the fields from storage
};

#endif

