// --------------------------------------------------------------------------------------------------------------------
// <copyright file="ApInfo.cpp" company="DTV-Online">
//   Copyright(c) 2019 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// --------------------------------------------------------------------------------------------------------------------
#include <String.h>
#include <ArduinoJson.hpp>
#include <ArduinoJson.h>
#include <esp_wifi.h>

#include "ApInfo.h"

/// <summary>
///  Using a WiFi instance to get the actual data.
/// </summary>
/// <param name="wifi">The WiFiClass instance</param>
ApInfoClass::ApInfoClass(WiFiClass wifi)
{
	wifi_config_t config;
	esp_wifi_get_config(WIFI_IF_AP, &config);

	SSID = String(reinterpret_cast<const char*>(config.ap.ssid));
	PASS = String(reinterpret_cast<const char*>(config.ap.password));
	Hostname = String(WiFi.softAPgetHostname());
	NetworkID = wifi.softAPNetworkID().toString();
	Address = wifi.softAPIP().toString();
	Clients = wifi.softAPgetStationNum();
	MAC = wifi.softAPmacAddress();
}

/// <summary>
///  Serialize the ApInfoClass instance to a JSON string.
/// </summary>
/// <returns>The JSON string</returns>
String ApInfoClass::serialize()
{
	const int capacity = JSON_OBJECT_SIZE(7) + 230;
	StaticJsonDocument<capacity> settings;
	String json;

	settings["SSID"] = SSID;
	settings["PASS"] = PASS;
	settings["Hostname"] = Hostname;
	settings["NetworkID"] = NetworkID;
	settings["Address"] = Address;
	settings["Clients"] = Clients;
	settings["MAC"] = MAC;

	serializeJsonPretty(settings, json);

	return json;
}

/// <summary>
///  Prints the ApInfoClass instance data on the Serial stream.
/// </summary>
void ApInfoClass::print()
{
	Serial.println("WiFi Accesspoint Info:");
	Serial.print("    SSID:      "); Serial.println(SSID);
	Serial.print("    PASS:      "); Serial.println(PASS);
	Serial.print("    Hostname:  "); Serial.println(Hostname);
	Serial.print("    NetworkID: "); Serial.println(NetworkID);
	Serial.print("    Address:   "); Serial.println(Address);
	Serial.print("    Clients:   "); Serial.println(Clients);
	Serial.print("    MAC:       "); Serial.println(MAC);
}
