// --------------------------------------------------------------------------------------------------------------------
// <copyright file="GameSettings.h" company="DTV-Online">
//   Copyright(c) 2019 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// --------------------------------------------------------------------------------------------------------------------
#ifndef _GAME_SETTINGS_h
#define _GAME_SETTINGS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <Preferences.h>

/// <summary>
/// This class holds the Knoblomat game result data.
/// </summary>
class GameSettingsClass
{
private:
	const char* NAMESPACE = "Game";			// The namspace used in preferences
	const char* KEY_TIES = "Ties";			// The preference key for the Ties field
	const char* KEY_WINS = "Wins";			// The preference key for the Wins field
	const char* KEY_LOSSES = "Losses";		// The preference key for the Losses field

	Preferences preferences;				// The EPS32 preferences instance

public:
	GameSettingsClass() {}

	int Ties;								// The total number of ties
	int Wins;								// The total number of wins
	int Losses;								// The total number of losses

	bool deserialize(String settings);		// Read a JSON string and updates the fields.
	String serialize();						// Return a string serialization (JSON)
	void clear();							// Clears the persistent storage
	void save();							// Save the fields to storage
	void init();							// Initializes the fields from storage
};

#endif

