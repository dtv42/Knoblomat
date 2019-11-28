// --------------------------------------------------------------------------------------------------------------------
// <copyright file="GameSettings.cpp" company="DTV-Online">
//   Copyright(c) 2019 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// --------------------------------------------------------------------------------------------------------------------
#include <String.h>
#include <ArduinoJson.hpp>
#include <ArduinoJson.h>

#include "GameSettings.h"

/// <summary>
/// Initializes selected data fields to default values.
/// </summary>
GameSettingsClass::GameSettingsClass()
{

}

/// <summary>
/// Initializes all data from the non volatile storage.
/// </summary>
void GameSettingsClass::init()
{
	preferences.begin(NAMESPACE, false);
	Ties = preferences.getInt(KEY_TIES, 0);
	Wins = preferences.getInt(KEY_WINS, 0);
	Losses = preferences.getInt(KEY_LOSSES, 0);
	preferences.end();
}

/// <summary>
/// Saves all data to the non volatile storage.
/// </summary>
void GameSettingsClass::save()
{
	preferences.begin(NAMESPACE, false);
	preferences.putInt(KEY_TIES, Ties);
	preferences.putInt(KEY_WINS, Wins);
	preferences.putInt(KEY_LOSSES, Losses);
	preferences.end();
}

/// <summary>
///  Clears all data on the non volatile storage.
/// </summary>
void GameSettingsClass::clear()
{
	preferences.begin(NAMESPACE, false);
	preferences.remove(KEY_TIES);
	preferences.remove(KEY_WINS);
	preferences.remove(KEY_LOSSES);
	preferences.end();
}

/// <summary>
///  Deserialize the data fields from a JSON string.
/// </summary>
bool GameSettingsClass::deserialize(String text)
{
	if (text.length() > 0)
	{
		const int capacity = JSON_OBJECT_SIZE(3) + 17;
		StaticJsonDocument<capacity> settings;
		DeserializationError err = deserializeJson(settings, text);

		if (err)
		{
			return false;
		}

		Ties = settings["Ties"] | Ties;
		Wins = settings["Wins"] | Wins;
		Losses = settings["Losses"] | Losses;

		return true;
	}
}

/// <summary>
///  Serialize the class instance to a JSON string.
/// </summary>
/// <returns>The JSON string</returns>
String GameSettingsClass::serialize()
{
	const int capacity = JSON_OBJECT_SIZE(3) + 17;
	StaticJsonDocument<capacity> settings;
	String json;

	settings["Ties"] = Ties;
	settings["Wins"] = Wins;
	settings["Losses"] = Losses;

	serializeJsonPretty(settings, json);

	return json;
}
