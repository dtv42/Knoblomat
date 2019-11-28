// --------------------------------------------------------------------------------------------------------------------
// <copyright file="WiFiSettings.cpp" company="DTV-Online">
//   Copyright(c) 2019 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// --------------------------------------------------------------------------------------------------------------------
#include <String.h>
#include <ArduinoJson.hpp>
#include <ArduinoJson.h>
#include "WiFiSettings.h"

/// <summary>
/// Initializes selected data fields to default values.
/// </summary>
WiFiSettingsClass::WiFiSettingsClass()
{
	SSID.reserve(MAX_SSID_LEN);
	PASS.reserve(MAX_PASS_LEN);
	Hostname.reserve(MAX_HOSTNAME_LEN);
	Address.reserve(MAX_IPADDRESS_LEN);
	Gateway.reserve(MAX_IPADDRESS_LEN);
	Subnet.reserve(MAX_IPADDRESS_LEN);

	Hostname = String(WIFI_HOSTNAME);
	DHCP = true;
	Subnet = String(SUBNET_MASK);
}

/// <summary>
/// Initializes all data from the non volatile storage.
/// </summary>
void WiFiSettingsClass::init()
{
	preferences.begin(NAMESPACE, false);
	SSID = preferences.getString(KEY_SSID, "");
	PASS = preferences.getString(KEY_PASS, "");
	Hostname = preferences.getString(KEY_HOSTNAME, WIFI_HOSTNAME);
	DHCP = preferences.getBool(KEY_DHCP, "");
	Address = preferences.getString(KEY_ADDRESS, "");
	Gateway = preferences.getString(KEY_GATEWAY, "");
	Subnet = preferences.getString(KEY_SUBNET, SUBNET_MASK);
	DNS1 = preferences.getString(KEY_DNS1, "");
	DNS2 = preferences.getString(KEY_DNS2, "");
	preferences.end();
}

/// <summary>
/// Saves all data to the non volatile storage.
/// </summary>
void WiFiSettingsClass::save()
{
	preferences.begin(NAMESPACE, false);
	preferences.putString(KEY_SSID, SSID);
	preferences.putString(KEY_PASS, PASS);
	preferences.putString(KEY_HOSTNAME, Hostname);
	preferences.putBool(KEY_DHCP, DHCP);
	preferences.putString(KEY_ADDRESS, Address);
	preferences.putString(KEY_GATEWAY, Gateway);
	preferences.putString(KEY_SUBNET, Subnet);
	preferences.putString(KEY_DNS1, DNS1);
	preferences.putString(KEY_DNS2, DNS2);
	preferences.end();
}

/// <summary>
///  Clears all data on the non volatile storage.
/// </summary>
void WiFiSettingsClass::clear()
{
	preferences.begin(NAMESPACE, false);
	preferences.remove(KEY_SSID);
	preferences.remove(KEY_PASS);
	preferences.remove(KEY_HOSTNAME);
	preferences.remove(KEY_DHCP);
	preferences.remove(KEY_ADDRESS);
	preferences.remove(KEY_GATEWAY);
	preferences.remove(KEY_SUBNET);
	preferences.remove(KEY_DNS1);
	preferences.remove(KEY_DNS2);
	preferences.end();
}

/// <summary>
///  Deserialize the data fields from a JSON string.
/// </summary>
/// <param name="json">The JSON string</param>
/// <returns>True if successful</returns>
bool WiFiSettingsClass::deserialize(String json)
{
	if (json.length() > 0)
	{
		String s;
		const int capacity = JSON_OBJECT_SIZE(9) + 268;
		StaticJsonDocument<capacity> doc;
		DeserializationError err = deserializeJson(doc, json);

		if (err)
		{
			return false;
		}

		s = doc["SSID"] | SSID;
		SSID = (s.length() > MAX_SSID_LEN) ? s.substring(0, MAX_SSID_LEN) : s;

		s = doc["PASS"] | PASS;
		PASS = (s.length() > MAX_PASS_LEN) ? s.substring(0, MAX_PASS_LEN) : s;

		s = doc["Hostname"] | Hostname;
		Hostname = (s.length() > MAX_HOSTNAME_LEN) ? s.substring(0, MAX_HOSTNAME_LEN) : s;

		DHCP = doc["DHCP"] | DHCP;

		s = doc["Address"] | Address;
		Address = (s.length() > MAX_IPADDRESS_LEN) ? s.substring(0, MAX_IPADDRESS_LEN) : s;

		s = doc["Gateway"] | Gateway;
		Gateway = (s.length() > MAX_IPADDRESS_LEN) ? s.substring(0, MAX_IPADDRESS_LEN) : s;

		s = doc["Subnet"] | Subnet;
		Subnet = (s.length() > MAX_IPADDRESS_LEN) ? s.substring(0, MAX_IPADDRESS_LEN) : s;

		s = doc["DNS1"] | DNS1;
		DNS1 = (s.length() > MAX_IPADDRESS_LEN) ? s.substring(0, MAX_IPADDRESS_LEN) : s;

		s = doc["DNS2"] | DNS2;
		DNS2 = (s.length() > MAX_IPADDRESS_LEN) ? s.substring(0, MAX_IPADDRESS_LEN) : s;

		return true;
	}
}

/// <summary>
///  Serialize the class instance to a JSON string.
/// </summary>
/// <returns>The JSON string</returns>
String WiFiSettingsClass::serialize()
{
	const int capacity = JSON_OBJECT_SIZE(9) + 268;
	StaticJsonDocument<capacity> settings;
	String json;

	settings["SSID"] = SSID;
	settings["PASS"] = PASS;
	settings["Hostname"] = Hostname;
	settings["DHCP"] = DHCP;
	settings["Address"] = Address;
	settings["Gateway"] = Gateway;
	settings["Subnet"] = Subnet;
	settings["DNS1"] = DNS1;
	settings["DNS2"] = DNS2;

	serializeJsonPretty(settings, json);

	return json;
}
