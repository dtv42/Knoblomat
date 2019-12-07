// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Settings.h" company="DTV-Online">
//   Copyright(c) 2019 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// --------------------------------------------------------------------------------------------------------------------
#ifndef _SETTINGS_h
#define _SETTINGS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "ApSettings.h"
#include "WiFiSettings.h"
#include "GameSettings.h"

/// <summary>
/// This class holds the all settings data.
/// </summary>
class SettingsClass
{
public:
	ApSettingsClass ApSettings;				// The Access Point settings 
	WiFiSettingsClass WiFiSettings;			// The WiFi connection settings
	GameSettingsClass GameSettings;			// The Knoblomat game settings (score)

	bool deserialize(String settings);		// Read a JSON string and updates the fields.
	String serialize();						// Return a string serialization (JSON)
	void clear();							// Clears the persistent storage
	void save();							// Save the fields to storage
	void init();							// Initializes the fields from storage
};

#endif

