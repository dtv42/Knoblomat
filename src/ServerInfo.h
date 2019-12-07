// --------------------------------------------------------------------------------------------------------------------
// <copyright file="ServerInfo.h" company="DTV-Online">
//   Copyright(c) 2019 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// --------------------------------------------------------------------------------------------------------------------
#pragma once

#include <WiFi.h>

/// <summary>
/// This class holds the actual HTTP server settings data.
/// </summary>
class ServerInfoClass
{
public:
	static char* HOSTNAME;					// The default hostname (mDNS)
	static int PORT;						// The default web server port (80)

	ServerInfoClass(WiFiClass wifi);

	String WiFiAddress;						// The web server IP address on the WiFi
	String ApAddress;						// The web server IP address on the accesspoint
	String Name;							// The web server hostname
	int Port;								// The web server IP port
	String Url;								// The web server URL (mDNS)

	String serialize();						// Return a string serialization (JSON)
	void print();							// Prints all fields on the serial line
};
