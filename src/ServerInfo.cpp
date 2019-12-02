// --------------------------------------------------------------------------------------------------------------------
// <copyright file="ServerInfo.cpp" company="DTV-Online">
//   Copyright(c) 2019 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// --------------------------------------------------------------------------------------------------------------------
#include <String.h>
#include <ArduinoJson.hpp>
#include <ArduinoJson.h>

#include "ServerInfo.h"

char* ServerInfoClass::HOSTNAME = "knoblomat";		// The default hostname (mDNS)
int ServerInfoClass::PORT = 80;						// The default web server port (80)

/// <summary>
///  Using a WiFi instance to get the actual data.
/// </summary>
/// <param name="wifi">The WiFiClass instance</param>
ServerInfoClass::ServerInfoClass(WiFiClass wifi)
{
	Port = PORT;
	Name = String(HOSTNAME);
	WiFiAddress = wifi.localIP().toString();
	ApAddress = wifi.softAPIP().toString();
	Url = String("http://") + HOSTNAME;
}

/// <summary>
///  Serialize the ServerInfoClass instance to a JSON string.
/// </summary>
/// <returns>The JSON string</returns>
String ServerInfoClass::serialize()
{
	const int capacity = JSON_OBJECT_SIZE(4) + 100;
	StaticJsonDocument<capacity> doc;
	String json;

	doc["WiFiAddress"] = WiFiAddress;
	doc["ApAddress"] = ApAddress;
	doc["Name"] = Name;
	doc["Port"] = Port;
	doc["Url"] = Url;

	serializeJsonPretty(doc, json);

	return json;
}

/// <summary>
///  Prints the ServerInfoClass instance data on the Serial stream.
/// </summary>
void ServerInfoClass::print()
{
	Serial.println("Web Server Info:");
	Serial.print("    WiFi Address: "); Serial.println(WiFiAddress);
	Serial.print("    AP Address:   "); Serial.println(ApAddress);
	Serial.print("    Name:         "); Serial.println(Name);
	Serial.print("    Port:         "); Serial.println(Port);
	Serial.print("    URL:          "); Serial.println(Url);
}

