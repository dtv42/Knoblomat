// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Setting.cpp" company="DTV-Online">
//   Copyright(c) 2019 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// --------------------------------------------------------------------------------------------------------------------
#include <String.h>
#include <ArduinoJson.hpp>
#include <ArduinoJson.h>

#include "Settings.h"

/// <summary>
/// Initializes all data from the non volatile storage.
/// </summary>
void SettingsClass::init()
{
	ApSettings.init();
	WiFiSettings.init();
	GameSettings.init();
}

/// <summary>
/// Saves all data to the non volatile storage.
/// </summary>
void SettingsClass::save()
{
	ApSettings.save();
	WiFiSettings.save();
	GameSettings.save();
}

/// <summary>
///  Clears all data on the non volatile storage.
/// </summary>
void SettingsClass::clear()
{
	ApSettings.clear();
	WiFiSettings.clear();
	GameSettings.clear();
}

/// <summary>
///  Deserialize the data fields from a JSON string.
/// </summary>
/// <param name="json">The JSON string</param>
/// <returns>True if successful</returns>
bool SettingsClass::deserialize(String json)
{
	if (json.length() > 0)
	{
		const int capacity = 2 * JSON_OBJECT_SIZE(3) +
								 JSON_OBJECT_SIZE(7) +
								 JSON_OBJECT_SIZE(9) + 550;
		String json;
		StaticJsonDocument<capacity> doc;
		DeserializationError err = deserializeJson(doc, json);

		if (err)
		{
			return false;
		}

		serializeJson(doc["ApSettings"], json);
		ApSettings.deserialize(json);
		serializeJson(doc["WiFiSettings"], json);
		WiFiSettings.deserialize(json);
		serializeJson(doc["GameSettings"], json);
		GameSettings.deserialize(json);

		return true;
	}
}

/// <summary>
///  Serialize the class instance to a JSON string.
/// </summary>
/// <returns>The JSON string</returns>
String SettingsClass::serialize()
{
	const int capacity = 2 * JSON_OBJECT_SIZE(3) +
							 JSON_OBJECT_SIZE(7) +
							 JSON_OBJECT_SIZE(9) + 550;
	StaticJsonDocument<capacity> doc;
	String json;

	doc["ApSettings"] = serialized(ApSettings.serialize());
	doc["WiFiSettings"] = serialized(WiFiSettings.serialize());
	doc["GameSettings"] = serialized(GameSettings.serialize());

	serializeJsonPretty(doc, json);
	return json;
}
