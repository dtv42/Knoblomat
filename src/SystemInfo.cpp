// --------------------------------------------------------------------------------------------------------------------
// <copyright file="SystemInfo.cpp" company="DTV-Online">
//   Copyright(c) 2019 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// --------------------------------------------------------------------------------------------------------------------
#include <String.h>
#include <ArduinoJson.hpp>
#include <ArduinoJson.h>
#include <ESP.h>

#include "SystemInfo.h"

/// <summary>
///  Using the global ESP instance to get the actual data.
/// </summary>
SystemInfoClass::SystemInfoClass()
{
	ChipRevision = ESP.getChipRevision();
	CpuFreqMHz = ESP.getCpuFreqMHz();
	FlashChipSpeed = ESP.getFlashChipSpeed() / 1000000;
	FlashChipSize = ESP.getFlashChipSize() / 1000;
	HeapSize = ESP.getHeapSize() / 1000;
	FreeHeap = ESP.getFreeHeap() / 1000;
	SketchSize = ESP.getSketchSize() / 1000;
	FreeSketchSpace = ESP.getFreeSketchSpace() / 1000;
	SketchMD5 = ESP.getSketchMD5();
	SdkVersion = String(ESP.getSdkVersion());

	uint64_t mac = ESP.getEfuseMac();
	uint8_t bytes[6];
	char chipid[13];

	bytes[0] = (mac >> 40) & 0xFF;
	bytes[1] = (mac >> 32) & 0xFF;
	bytes[2] = (mac >> 24) & 0xFF;
	bytes[3] = (mac >> 16) & 0xFF;
	bytes[4] = (mac >> 8) & 0xFF;
	bytes[5] = (mac >> 0) & 0xFF;

	sprintf(chipid, "%02X%02X%02X%02X%02X%02X", bytes[0], bytes[1], bytes[2], bytes[3], bytes[4], bytes[5]);

	ChipID = String(chipid);
	Software = String(SOFTWARE_VERSION);
}

/// <summary>
///  Serialize the SystemInfoClass instance to a JSON string.
/// </summary>
/// <returns>The JSON string</returns>
String SystemInfoClass::serialize()
{
	const int capacity = JSON_OBJECT_SIZE(11) + 205;
	StaticJsonDocument<capacity> doc;
	String json;

	doc["ChipRevision"] = ChipRevision;
	doc["CpuFreqMHz"] = CpuFreqMHz;
	doc["FlashChipSpeed"] = FlashChipSpeed;
	doc["FlashChipSize"] = FlashChipSize;
	doc["HeapSize"] = HeapSize;
	doc["FreeHeap"] = FreeHeap;
	doc["SketchSize"] = SketchSize;
	doc["FreeSketchSpace"] = FreeSketchSpace;
	doc["SketchMD5"] = SketchMD5;
	doc["SdkVersion"] = SdkVersion;
	doc["ChipID"] = ChipID;
	doc["Software"] = Software;

	serializeJsonPretty(doc, json);

	return json;
}

/// <summary>
///  Prints the SystemInfoClass instance data on the Serial stream.
/// </summary>
void SystemInfoClass::print()
{
	Serial.println("System Info:");
	Serial.print("    ChipRevision:    "); Serial.println(ChipRevision);
	Serial.print("    CpuFreqMHz:      "); Serial.println(CpuFreqMHz);
	Serial.print("    FlashChipSpeed:  "); Serial.println(FlashChipSpeed);
	Serial.print("    FlashChipSize:   "); Serial.println(FlashChipSize);
	Serial.print("    HeapSize:        "); Serial.println(HeapSize);
	Serial.print("    FreeHeap:        "); Serial.println(FreeHeap);
	Serial.print("    SketchSize:      "); Serial.println(SketchSize);
	Serial.print("    FreeSketchSpace: "); Serial.println(FreeSketchSpace);
	Serial.print("    SketchMD5:       "); Serial.println(SketchMD5);
	Serial.print("    SdkVersion:      "); Serial.println(SdkVersion);
	Serial.print("    ChipID:          "); Serial.println(ChipID);
	Serial.print("    Software:        "); Serial.println(Software);
}

