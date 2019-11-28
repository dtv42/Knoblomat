// --------------------------------------------------------------------------------------------------------------------
// <copyright file="WiFiInfo.cpp" company="DTV-Online">
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

#include "include/WiFiInfo.h"

/// <summary>
///  Using a WiFi instance to get the actual data.
/// </summary>
/// <param name="wifi">The WiFiClass instance</param>
WiFiInfoClass::WiFiInfoClass(WiFiClass wifi)
{
	wifi_config_t config;
	esp_wifi_get_config(WIFI_IF_STA, &config);

	SSID = String(reinterpret_cast<const char*>(config.sta.ssid));
	PASS = String(reinterpret_cast<const char*>(config.sta.password));
	Hostname = String(wifi.getHostname());
	NetworkID = wifi.networkID().toString();
	Address = wifi.localIP().toString();
	Gateway = wifi.gatewayIP().toString();
	Subnet = wifi.subnetMask().toString();
	DNS = wifi.dnsIP().toString();
	RSSI = wifi.RSSI();
	BSSID = wifi.BSSIDstr();
	MAC = wifi.macAddress();
}

/// <summary>
///  Serialize the WiFiInfoClass instance to a JSON string.
/// </summary>
/// <returns>The JSON string</returns>
String WiFiInfoClass::serialize()
{
	const int capacity = JSON_OBJECT_SIZE(11) + 318;
	StaticJsonDocument<capacity> settings;
	String json;

	settings["SSID"] = SSID;
	settings["PASS"] = PASS;
	settings["Hostname"] = Hostname;
	settings["NetworkID"] = NetworkID;
	settings["Address"] = Address;
	settings["Gateway"] = Gateway;
	settings["Subnet"] = Subnet;
	settings["DNS"] = DNS;
	settings["RSSI"] = RSSI;
	settings["BSSID"] = BSSID;
	settings["MAC"] = MAC;

	serializeJsonPretty(settings, json);

	return json;
}

/// <summary>
///  Prints the ApInfoClass instance data on the Serial stream.
/// </summary>
void WiFiInfoClass::print()
{
	Serial.println("WiFi Connection Info:");
	Serial.print("    SSID:      "); Serial.println(SSID);
	Serial.print("    PASS:      "); Serial.println(PASS);
	Serial.print("    NetworkID: "); Serial.println(NetworkID);
	Serial.print("    Hostname:  "); Serial.println(Hostname);
	Serial.print("    Address:   "); Serial.println(Address);
	Serial.print("    Gateway:   "); Serial.println(Gateway);
	Serial.print("    Subnet:    "); Serial.println(Subnet);
	Serial.print("    DNS:       "); Serial.println(DNS);
	Serial.print("    RSSI(dBm): "); Serial.println(RSSI);
	Serial.print("    BSSID:     "); Serial.println(BSSID);
	Serial.print("    MAC:       "); Serial.println(MAC);
}
