// --------------------------------------------------------------------------------------------------------------------
// <copyright file="ApInfo.h" company="DTV-Online">
//   Copyright(c) 2019 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// --------------------------------------------------------------------------------------------------------------------
#pragma once

#include <WiFi.h>

/// <summary>
/// This class holds the actual WiFi access point data.
/// </summary>
class ApInfoClass
{
public:
	ApInfoClass(WiFiClass wifi);			// Using a WiFi instance to get the data

	String SSID;							// The WiFi Access Point SSID
	String PASS;							// The WiFi Access Point passphrase
	String Hostname;						// The WiFi Access Point hostname
	String NetworkID;						// The WiFi Access Point network ID
	String Address;							// The WiFi Access Point IP address
	int Clients;							// The number of clients (max. 4)
	String MAC;								// The WiFi Access Point MAC address

	String serialize();						// Return a string serialization (JSON)
	void print();							// Prints all fields on the serial line
};

