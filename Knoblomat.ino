// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Knoblomat.ino" company="DTV-Online">
//  Copyright(c) 2019 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//  Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// --------------------------------------------------------------------------------------------------------------------
#include <jled.h>
#include <neotimer.h>
#include <ArduinoJson.hpp>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <NetBIOS.h>
#include <ESPmDNS.h>
#include <SPIFFS.h>
#include <StringArray.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <esp_log.h>
#include <esp_wifi.h>
#include <nvs_flash.h>
#include <rom/rtc.h>

#include "include/Settings.h"
#include "include/ApInfo.h"
#include "include/WiFiInfo.h"
#include "include/ApSettings.h"
#include "include/ServerInfo.h"
#include "include/SystemInfo.h"

char* SystemInfoClass::SOFTWARE_VERSION = "V1.2.2 2019-11-30";

// Message lines to be printed on the serial line during startup.
auto HEADER = "KNOBLOMAT - a DTV classic since the 1970s";
auto COPYRIGHT = "Copyright (c) 2019 - Dr. Peter Trimmel";

// Watchdog timer - 10 minute timer
Neotimer timer = Neotimer(10* 60 * 1000); 

// On board LED.
JLed led = JLed(LED_BUILTIN).Breathe(1000).DelayAfter(1000).Forever();

// The global application settings.
SettingsClass settings;

// Create Webserver at the default port.
AsyncWebServer server(ServerInfoClass::PORT);

// Flag indicating that a smart config is about to be performed.
bool smartconfig = false;

// Flag indicating that a reboot is about to be performed.
bool reboot = false;

// Flag indicating that a WiFi connection to an access point is OK.
bool wifiOK = false;

// Flag indicating that a WiFi access point is running.
bool apOK = false;

/// <summary>
/// Try to connect to WiFi (returns the WiFi status).
/// After every try wait for 500 msec.
/// </summary>
/// <param name="count">The number of tries</param>
/// <returns>The WiFi status</returns>
int wait4WiFi(int count = 20)
{
	//Wait for WiFi to connect
	Serial.println("Waiting for WiFi");
	while ((WiFi.status() != WL_CONNECTED) && (--count > 0)) {
		delay(500);
		Serial.print(".");
	}
	Serial.println("");

	return WiFi.status();
}

/// <summary>
/// Try to connect to WiFi network using the WiFiSettings.
/// </summary>
void connectWiFi(void)
{
	wifiOK = false;
	WiFi.setAutoReconnect(true);
	WiFi.setHostname(settings.WiFiSettings.Hostname.c_str());
	int status = WL_IDLE_STATUS;

	// Check if WiFi network SSID is availabel and try to connect.
	if (settings.WiFiSettings.SSID != "")
	{
		// Trying to connect to WiFi network using the stored settings.
		Serial.print("Attempting to connect to WiFi network, SSID: ");
		Serial.println(settings.WiFiSettings.SSID);

		// Using WiFi passphrase to connect.
		if (settings.WiFiSettings.PASS != "")
		{
			if (settings.WiFiSettings.DHCP)
			{
				// attempt to connect to Wifi network:
				WiFi.begin(settings.WiFiSettings.SSID.c_str(), settings.WiFiSettings.PASS.c_str());
				status = wait4WiFi();
			}
			else
			{
				IPAddress address;
				IPAddress gateway;
				IPAddress subnet;
				IPAddress dns1;
				IPAddress dns2;

				bool addressOK = address.fromString(settings.WiFiSettings.Address);
				bool gatewayOK = gateway.fromString(settings.WiFiSettings.Gateway);
				bool subnetOK = subnet.fromString(settings.WiFiSettings.Subnet);
				bool dns1OK = dns1.fromString(settings.WiFiSettings.DNS1);
				bool dns2OK = dns2.fromString(settings.WiFiSettings.DNS2);

				if (addressOK && gatewayOK && subnetOK)
				{
					if (dns1OK && dns2OK)
					{
						WiFi.config(address, gateway, subnet, dns1, dns2);
					}
					else if (dns1OK)
					{
						WiFi.config(address, gateway, subnet, dns1);
					}
					else
					{
						WiFi.config(address, gateway, subnet);
					}

					WiFi.begin(settings.WiFiSettings.SSID.c_str(), settings.WiFiSettings.PASS.c_str());
					status = wait4WiFi();
				}
			}
		}
		// Connect to the open WiFi network
		else
		{
			if (settings.WiFiSettings.DHCP)
			{
				WiFi.begin(settings.WiFiSettings.SSID.c_str());
				status = wait4WiFi();
			}
			else
			{
				IPAddress address;
				IPAddress gateway;
				IPAddress subnet;
				IPAddress dns1;
				IPAddress dns2;

				bool addressOK = address.fromString(settings.WiFiSettings.Address);
				bool gatewayOK = gateway.fromString(settings.WiFiSettings.Gateway);
				bool subnetOK = subnet.fromString(settings.WiFiSettings.Subnet);
				bool dns1OK = dns1.fromString(settings.WiFiSettings.DNS1);
				bool dns2OK = dns2.fromString(settings.WiFiSettings.DNS2);

				if (addressOK && gatewayOK && subnetOK)
				{
					if (dns1OK && dns2OK)
					{
						WiFi.config(address, gateway, subnet, dns1, dns2);
					}
					else if (dns1OK)
					{
						WiFi.config(address, gateway, subnet, dns1);
					}
					else
					{
						WiFi.config(address, gateway, subnet);
					}

					WiFi.begin(settings.WiFiSettings.SSID.c_str());
					status = wait4WiFi();
				}
			}
		}

		if (status == WL_CONNECTED)
		{
			wifiOK = true;

			WiFiInfoClass info(WiFi);
			info.print();
		}
		else
		{
			Serial.println("WiFi connection not successful");
		}
	}
}

/// <summary>
/// Try to create an WiFi access point using the APSettings.
/// </summary>
void createAP(void)
{
	apOK = false;
	Serial.print("Creating access point named: ");
	Serial.println(settings.ApSettings.SSID);

	// Check if the default SSID has been set (using the chip ID - MAC address).
	if (settings.ApSettings.SSID == ApSettingsClass::WIFI_SSID_AP) {
		SystemInfoClass info;

		settings.ApSettings.SSID = ApSettingsClass::WIFI_SSID_AP + info.ChipID;
		settings.ApSettings.save();
	}

	if (settings.ApSettings.Custom)
	{
		IPAddress address;
		IPAddress gateway;
		IPAddress subnet;

		bool addressOK = address.fromString(settings.ApSettings.Address);
		bool gatewayOK = gateway.fromString(settings.ApSettings.Gateway);
		bool subnetOK = subnet.fromString(settings.ApSettings.Subnet);

		WiFi.softAPConfig(address, gateway, subnet);
	}

	// Create WiFi access point with passphrase.
	if (settings.ApSettings.PASS != "")
	{
		apOK = WiFi.softAP(settings.ApSettings.SSID.c_str(), settings.ApSettings.PASS.c_str());
	}
	// Create open WiFi access point. 
	else
	{
		apOK = WiFi.softAP(settings.ApSettings.SSID.c_str());
	}

	delay(100);

	if (apOK)
	{
		WiFi.softAPsetHostname(settings.ApSettings.Hostname.c_str());
		ApInfoClass info(WiFi);
		info.print();
	}
	else
	{
		Serial.print("WiFi Access Point setup not successful");
	}
}

/// <summary>
/// Check if we should start the WiFi SmartConfig.
/// </summary>
void checkSmart(void)
{
	if (smartconfig)
	{
		smartconfig = false;

		if (WiFi.isConnected())
		{
			WiFi.disconnect();
		}

		//Init WiFi as Access Point and Station
		WiFi.mode(WIFI_OFF);
		WiFi.mode(WIFI_AP_STA);
		while (WiFi.getMode() != WIFI_AP_STA) {
			delay(50);
		}

		//Start SmartConfig
		WiFi.beginSmartConfig();

		int count = 120;

		//Wait for SmartConfig packet from mobile
		Serial.println("Waiting for SmartConfig.");
		while (!WiFi.smartConfigDone() && (--count > 0)) {
			delay(500);
			Serial.print(".");
		}
		Serial.println("");

		if (WiFi.smartConfigDone())
		{
			Serial.println("SmartConfig received.");
			wait4WiFi();

			if (wifiOK)
			{
				Serial.println("WiFi Connected.");

				wifiOK = true;

				WiFiInfoClass info(WiFi);
				info.print();

				// Save the smart config WiFi settings.
				settings.WiFiSettings.SSID = info.SSID;
				settings.WiFiSettings.PASS = info.PASS;
				settings.WiFiSettings.DHCP = true;
				settings.WiFiSettings.save();

				ESP.restart();
			}
			else
			{
				// Revert to default WiFi settings (default access point).
				settings.clear();
				ESP.restart();
			}
		}
	}
}

/// <summary>
/// Check if we should reboot the Knoblomat (5 sec delay).
/// </summary>
void checkReboot(void)
{
	// Reboot counter for a 5 seconds reboot delay.
	static int counter = 50;

	if (reboot)
	{
		if (--counter < 0)
		{
			reboot = false;
			ESP.restart();
		}
		else
		{
			delay(100);
		}
	}
}

/// <summary>
/// Check if watchdog timer has expired (goto deep sleep).
/// Note that since no wake-up sources have been configured the chip will
/// be in deep sleep mode indefinitely, until external reset is applied.
/// </summary>
void checkTimer(void)
{
	if (timer.done()) {
		Serial.println("Watchdog timer finished");
		esp_deep_sleep_start();
	}
}

/// <summary>
///  This is run only once after startup.
/// </summary>
void setup()
{
	// Initialize non volatile storage and settings.
	esp_err_t err = nvs_flash_init();

	if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		// NVS partition was truncated and needs to be erased
		// Retry nvs_flash_init
		ESP_ERROR_CHECK(nvs_flash_erase());
		err = nvs_flash_init();
	}

	// Set the log level for all components to WARNING
	esp_log_level_set("*", ESP_LOG_WARN);

	// Initialize serial and wait 1 second for port to open.
	Serial.begin(115200);
	delay(1000);

	// Print application startup info.
	Serial.println(HEADER);
	Serial.println(COPYRIGHT);
	Serial.println();

	// Print system info.
	SystemInfoClass info;
	info.print();

	// Initialize and print the settings.
	settings.init();
	Serial.println("Settings:");
	Serial.println(settings.serialize());

	// Mount the SPIFFS.
	if (!SPIFFS.begin())
	{
		Serial.println("An Error has occurred while mounting SPIFFS");
		return;
	}

	// Set the WiFi mode (allowing access point and station mode).
	WiFi.mode(WIFI_AP_STA);
	while (WiFi.getMode() != WIFI_AP_STA) {
		delay(50);
	}

	// Try to connect to WiFi network and create WiFi access point.
	connectWiFi();
	createAP();

	if (wifiOK || apOK)
	{
		// Setup NetBIOS name service
		NBNS.begin("ServerInfoClass::HOSTNAME");

		// Set up mDNS responder
		if (MDNS.begin(ServerInfoClass::HOSTNAME)) {
			Serial.println("mDNS responder started");

			// Add web service to MDNS-SD
			MDNS.addService("http", "tcp", ServerInfoClass::PORT);
		}
		else
		{
			Serial.println("Error setting up MDNS responder!");
		}

		// Setup handlers for bootstrap Web.

		server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.println("GET: /");
			request->send(SPIFFS, "/index.html", "text/html");
			timer.reset();
			});

		server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.println("GET: /favicon.ico");
			request->send(SPIFFS, "/images/favicon.png", "image/png");
			timer.reset();
			});

		server.on("/js/bootstrap.min.js", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.println("GET: /js/bootstrap.min.js");
			request->send(SPIFFS, "/js/bootstrap.min.js", "text/javascript");
			timer.reset();
			});

		server.on("/js/bootstrap.bundle.min.js", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.println("GET: /js/bootstrap.bundle.min.js");
			request->send(SPIFFS, "/js/bootstrap.bundle.min.js", "text/javascript");
			timer.reset();
			});

		server.on("/js/popper.min.js", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.println("GET: /js/popper.min.js");
			request->send(SPIFFS, "/js/popper.min.js", "text/javascript");
			timer.reset();
			});

		server.on("/js/jquery-3.4.1.min.js", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.println("GET: /js/jquery-3.4.1.min.js");
			request->send(SPIFFS, "/js/jquery-3.4.1.min.js", "text/javascript");
			timer.reset();
			});

		server.on("/css/bootstrap.min.css", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.println("GET: /css/bootstrap.min.css");
			request->send(SPIFFS, "/css/bootstrap.min.css", "text/css");
			timer.reset();
			});

		server.on("/css/knoblomat.min.css", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.println("GET: /css/knoblomat.min.css");
			request->send(SPIFFS, "/css/knoblomat.min.css", "text/css");
			timer.reset();
			});

		server.on("/index", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.println("GET: /index");
			request->send(SPIFFS, "/index.html", "text/html");
			timer.reset();
			});

		server.on("/help", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.println("GET: /help");
			request->send(SPIFFS, "/help.html", "text/html");
			timer.reset();
			});

		server.on("/config", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.println("GET: /config");
			request->send(SPIFFS, "/config.html", "text/html");
			timer.reset();
			});

		server.on("/about", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.println("GET: /about");
			request->send(SPIFFS, "/about.html", "text/html");
			timer.reset();
			});

		server.on("/js/state-machine.min.js", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.println("GET: /js/state-machine.min.js");
			request->send(SPIFFS, "/js/state-machine.min.js", "text/javascript");
			timer.reset();
			});

		server.on("/js/jquery.inputmask.min.js", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.println("GET: /js/jquery.inputmask.min.js");
			request->send(SPIFFS, "/js/jquery.inputmask.min.js", "text/javascript");
			timer.reset();
			});

		server.on("/images/picture0.jpg", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.println("GET: /images/picture0.jpg");
			request->send(SPIFFS, "/images/picture0.jpg", "image/jpg");
			timer.reset();
			});

		server.on("/images/picture1.jpg", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.println("GET: /images/picture1.jpg");
			request->send(SPIFFS, "/images/picture1.jpg", "image/jpg");
			timer.reset();
			});

		server.on("/images/picture2.jpg", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.println("GET: /images/picture2.jpg");
			request->send(SPIFFS, "/images/picture2.jpg", "image/jpg");
			timer.reset();
			});

		server.on("/images/picture3.jpg", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.println("GET: /images/picture3.jpg");
			request->send(SPIFFS, "/images/picture3.jpg", "image/jpg");
			timer.reset();
			});

		server.on("/ap", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.println("GET: /ap");

			if (apOK) {
				ApInfoClass info(WiFi);
				request->send(200, "application/json", info.serialize());
			}
			else {
				request->send(404, "text/html", "AP not available");
			}

			timer.reset();
			});

		server.on("/wifi", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.println("GET: /wifi");

			if (wifiOK) {
				WiFiInfoClass info(WiFi);
				request->send(200, "application/json", info.serialize());
			}
			else {
				request->send(404, "text/html", "WiFi not available");
			}

			timer.reset();
			});

		server.on("/game", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.println("GET: /game");
			request->send(200, "application/json", settings.GameSettings.serialize());
			timer.reset();
			});

		server.on("/server", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.println("GET: /server");
			ServerInfoClass info(WiFi);
			request->send(200, "application/json", info.serialize());
			timer.reset();
			});

		server.on("/system", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.println("GET: /server");
			SystemInfoClass info;
			request->send(200, "application/json", info.serialize());
			timer.reset();
			});

		server.on("/settings", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.println("GET: /settings");
			request->send(200, "application/json", settings.serialize());
			timer.reset();
			});

		server.on("/smart", HTTP_POST, [](AsyncWebServerRequest* request) {
			Serial.println("POST: /smart");
			request->send(202, "text/html", "Knoblomat running ESP32 SmartConfig for 1 minute");
			smartconfig = true;
			timer.reset();
			});

		server.on("/clear", HTTP_POST, [](AsyncWebServerRequest* request) {
			Serial.println("POST: /clear");
			request->send(202, "text/html", "Knoblomat clearing non volatile storage");
			settings.clear();
			timer.reset();
			});

		server.on("/reboot", HTTP_POST, [](AsyncWebServerRequest* request) {
			Serial.println("POST: /reboot");
			request->send(202, "text/html", "Knoblomat rebooting");
			led = JLed(LED_BUILTIN).Breathe(250).DelayAfter(250).Forever();
			reboot = true;
			});

		server.on("/ap", HTTP_POST, [](AsyncWebServerRequest* request) {}, NULL,
			[](AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
				Serial.println("POST: /ap");
				String json = String((char*)data).substring(0, len);
				settings.ApSettings.deserialize(json);
				settings.ApSettings.save();
				request->send(202, "application/json", settings.serialize());
				led = JLed(LED_BUILTIN).Breathe(250).DelayAfter(250).Forever();
				reboot = true;
			});

		server.on("/wifi", HTTP_POST, [](AsyncWebServerRequest* request) {}, NULL,
			[](AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
				Serial.println("POST: /wifi");
				String json = String((char*)data).substring(0, len);
				settings.WiFiSettings.deserialize(json);
				settings.WiFiSettings.save();
				request->send(202, "application/json", settings.serialize());
				led = JLed(LED_BUILTIN).Breathe(250).DelayAfter(250).Forever();
				reboot = true;
			});

		server.on("/game", HTTP_POST, [](AsyncWebServerRequest* request) {}, NULL,
			[](AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
				Serial.println("POST: /game");
				String json = String((char*)data).substring(0, len);
				settings.GameSettings.deserialize(json);
				settings.GameSettings.save();
				request->send(202, "application/json", settings.serialize());
				timer.reset();
			});

		server.onNotFound([](AsyncWebServerRequest* request) {
			Serial.println("404: Not Found");
			request->send(404);
			timer.reset();
			});

		// Start the HTTP server
		server.begin();

		ServerInfoClass info(WiFi);
		info.print();
	}
	else
	{
		Serial.print("No WiFi network - stopping");
		led = JLed(LED_BUILTIN).Breathe(250).DelayAfter(250).Forever();
	}

	// Start the watchdog timer
	timer.start();
}

/// <summary>
///	 The loop function runs over and over again until power down or reset
/// </summary>
void loop()
{
	led.Update();
	checkSmart();
	checkReboot();
	checkTimer();
}
