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

// Set the software version for the SystemInfoClass.
char* SystemInfoClass::SOFTWARE_VERSION = "V1.2.3 2019-12-01";

// Message lines to be printed on the serial line during startup.
auto HEADER = "KNOBLOMAT - a DTV classic since the 1970s";
auto COPYRIGHT = "Copyright (c) 2019 - Dr. Peter Trimmel";

// Watchdog timer - 10 minute timer
Neotimer timer = Neotimer(10 * 60 * 1000); 

// On board LED.
JLed led = JLed(LED_BUILTIN).Blink(1000, 1000).Forever();

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
/// Try to change the WiFi mode to AP_STA.
/// After every try wait for 50 msec.
/// </summary>
/// <param name="count">The number of tries</param>
/// <returns>True if sucessful</returns>
bool wait4Mode(int count = 10)
{
	//Wait for WiFi mode
	Serial.println("Waiting for WiFi Mode");

	WiFi.mode(WIFI_OFF);
	WiFi.mode(WIFI_AP_STA);

	while ((WiFi.getMode() != WIFI_AP_STA) && (--count > 0)) {
		delay(50);
	}

	return (WiFi.getMode() == WIFI_AP_STA);
}

/// <summary>
/// Try to connect to WiFi (returns true if the WiFi status == WL_CONNECTED).
/// After every try wait for 500 msec.
/// </summary>
/// <param name="count">The number of tries</param>
/// <returns>True if sucessful</returns>
bool wait4WiFi(int count = 20)
{
	//Wait for WiFi to connect
	Serial.println("Waiting for WiFi");
	while ((WiFi.status() != WL_CONNECTED) && (--count > 0)) {
		delay(500);
		Serial.print(".");
	}
	Serial.println("");

	return (WiFi.status() == WL_CONNECTED);
}

/// <summary>
/// Try to run ESP SmartConfig.
/// After every try wait for 500 msec.
/// </summary>
/// <param name="count">The number of tries</param>
/// <returns>True if sucessful</returns>
bool wait4Done(int count = 120)
{
	//Wait for SmartConfig packet from mobile
	Serial.println("Waiting for SmartConfig.");

	while (!WiFi.smartConfigDone() && (--count > 0)) {
		delay(500);
		Serial.print(".");
	}
	Serial.println("");

	return WiFi.smartConfigDone();
}

/// <summary>
/// Try to connect to WiFi network using the WiFiSettings.
/// </summary>
void connectWiFi(void)
{
	wifiOK = false;
	WiFi.setAutoReconnect(true);
	WiFi.setHostname(settings.WiFiSettings.Hostname.c_str());

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
				wifiOK = wait4WiFi();
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
					wifiOK = wait4WiFi();
				}
			}
		}
		// Connect to the open WiFi network
		else
		{
			if (settings.WiFiSettings.DHCP)
			{
				WiFi.begin(settings.WiFiSettings.SSID.c_str());
				wifiOK = wait4WiFi();
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
					wifiOK = wait4WiFi();
				}
			}
		}

		if (wifiOK)
		{
			Serial.println("WiFi connection successful");
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
		Serial.print("WiFi Access Point setup successful");
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
		if (wait4Mode())
		{
			//Start SmartConfig
			WiFi.beginSmartConfig();

			if (wait4Done())
			{
				Serial.println("SmartConfig received.");
				wifiOK = wait4WiFi();
			}
		}

		if (wifiOK)
		{
			Serial.println("WiFi Connected.");
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
/// WiFi connect event handler. 
/// </summary>
/// <param name="event">The WiFi event</param>
/// <param name="info">The WiFi event info</param>
void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info)
{

	Serial.println("Station connected");

	for (int i = 0; i < 6; i++) {

		Serial.printf("%02X", info.sta_connected.mac[i]);
		if (i < 5)Serial.print(":");
	}

	Serial.println("\n------------");
}

/// <summary>
/// WiFi disconnect event handler. 
/// </summary>
/// <param name="event">The WiFi event</param>
/// <param name="info">The WiFi event info</param>
void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info)
{

	Serial.println("Station disconnected");

	for (int i = 0; i < 6; i++) {

		Serial.printf("%02X", info.sta_disconnected.mac[i]);
		if (i < 5)Serial.print(":");
	}

	Serial.println("\n------------");
}

/// <summary>
/// WiFi station lost event handler. 
/// </summary>
/// <param name="event">The WiFi event</param>
/// <param name="info">The WiFi event info</param>
void WiFiStationLostIP(WiFiEvent_t event, WiFiEventInfo_t info)
{
	Serial.println("Station lost IP");
	Serial.println("\n------------");
	ESP.restart();
}

/// <summary>
/// WiFi stopped event handler. 
/// </summary>
/// <param name="event">The WiFi event</param>
/// <param name="info">The WiFi event info</param>
void WiFiStopped(WiFiEvent_t event, WiFiEventInfo_t info)
{
	Serial.println("AP stopped");
	Serial.println("\n------------");
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

	Serial.println("SPIFFS content:");
	File root = SPIFFS.open("/");
	File file = root.openNextFile();

	while (file) {
		Serial.println("\t" + String(file.name()) + ", " + String(file.size()) + " bytes");
		file = root.openNextFile();
	}

	// Set the WiFi event handler.
	WiFi.onEvent(WiFiStationConnected, SYSTEM_EVENT_AP_STACONNECTED);
	WiFi.onEvent(WiFiStationDisconnected, SYSTEM_EVENT_AP_STADISCONNECTED);
	WiFi.onEvent(WiFiStationConnected, SYSTEM_EVENT_STA_CONNECTED);
	WiFi.onEvent(WiFiStationDisconnected, SYSTEM_EVENT_STA_DISCONNECTED);
	WiFi.onEvent(WiFiStationLostIP, SYSTEM_EVENT_STA_LOST_IP);
	WiFi.onEvent(WiFiStopped, SYSTEM_EVENT_AP_STOP);

	// Set the WiFi mode (allowing access point and station mode).
	if (wait4Mode())
	{
		// Try to connect to WiFi network and create WiFi access point.
		connectWiFi();
		createAP();
	}

	if (wifiOK || apOK)
	{
		// Show Web server info.
		ServerInfoClass info(WiFi);
		info.print();

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

		// Setup handlers for bootstrap Web pages.

		server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.print("GET Request() url: "); Serial.println(request->url());
			request->send(SPIFFS, "/index.html", "text/html");
			timer.reset();
			});

		server.on("/home", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.print("GET Request() url: "); Serial.println(request->url());
			request->send(SPIFFS, "/index.html", "text/html");
			timer.reset();
			});

		server.on("/help", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.print("GET Request() url: "); Serial.println(request->url());
			request->send(SPIFFS, "/help.html", "text/html");
			timer.reset();
			});

		server.on("/config", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.print("GET Request() url: "); Serial.println(request->url());
			request->send(SPIFFS, "/config.html", "text/html");
			timer.reset();
			});

		server.on("/about", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.print("GET Request() url: "); Serial.println(request->url());
			request->send(SPIFFS, "/about.html", "text/html");
			timer.reset();
			});

		server.on("/error", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.print("GET Request() url: "); Serial.println(request->url());
			request->send(SPIFFS, "/error.html", "text/html");
			timer.reset();
			});

		// Setup handlers for various resources.

		server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.print("GET Request() url: "); Serial.println(request->url());
			request->send(SPIFFS, "/images/favicon.png", "image/png");
			timer.reset();
			});

		server.on("/js/bootstrap.min.js", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.print("GET Request() url: "); Serial.println(request->url());
			request->send(SPIFFS, "/js/bootstrap.min.js", "text/javascript");
			timer.reset();
			});

		server.on("/js/bootstrap.bundle.min.js", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.print("GET Request() url: "); Serial.println(request->url());
			request->send(SPIFFS, "/js/bootstrap.bundle.min.js", "text/javascript");
			timer.reset();
			});

		server.on("/js/popper.min.js", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.print("GET Request() url: "); Serial.println(request->url());
			request->send(SPIFFS, "/js/popper.min.js", "text/javascript");
			timer.reset();
			});

		server.on("/js/jquery-3.4.1.min.js", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.print("GET Request() url: "); Serial.println(request->url());
			request->send(SPIFFS, "/js/jquery-3.4.1.min.js", "text/javascript");
			timer.reset();
			});

		server.on("/css/bootstrap.min.css", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.print("GET Request() url: "); Serial.println(request->url());
			request->send(SPIFFS, "/css/bootstrap.min.css", "text/css");
			timer.reset();
			});

		server.on("/css/knoblomat.min.css", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.print("GET Request() url: "); Serial.println(request->url());
			request->send(SPIFFS, "/css/knoblomat.min.css", "text/css");
			timer.reset();
			});

		server.on("/js/state-machine.min.js", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.print("GET Request() url: "); Serial.println(request->url());
			request->send(SPIFFS, "/js/state-machine.min.js", "text/javascript");
			timer.reset();
			});

		server.on("/js/jquery.inputmask.min.js", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.print("GET Request() url: "); Serial.println(request->url());
			request->send(SPIFFS, "/js/jquery.inputmask.min.js", "text/javascript");
			timer.reset();
			});

		server.on("/images/picture0.jpg", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.print("GET Request() url: "); Serial.println(request->url());
			request->send(SPIFFS, "/images/picture0.jpg", "image/jpg");
			timer.reset();
			});

		server.on("/images/picture1.jpg", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.print("GET Request() url: "); Serial.println(request->url());
			request->send(SPIFFS, "/images/picture1.jpg", "image/jpg");
			timer.reset();
			});

		server.on("/images/picture2.jpg", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.print("GET Request() url: "); Serial.println(request->url());
			request->send(SPIFFS, "/images/picture2.jpg", "image/jpg");
			timer.reset();
			});

		server.on("/images/picture3.jpg", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.print("GET Request() url: "); Serial.println(request->url());
			request->send(SPIFFS, "/images/picture3.jpg", "image/jpg");
			timer.reset();
			});

		server.on("/sounds/vista.mp3", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.print("GET Request() url: "); Serial.println(request->url());
			request->send(SPIFFS, "/sounds/vista.mp3", "audio/mpeg");
			timer.reset();
			});

		server.on("/sounds/click.mp3", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.print("GET Request() url: "); Serial.println(request->url());
			request->send(SPIFFS, "/sounds/click.mp3", "audio/mpeg");
			timer.reset();
			});

		server.on("/sounds/win.mp3", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.print("GET Request() url: "); Serial.println(request->url());
			request->send(SPIFFS, "/sounds/win.mp3", "audio/mpeg");
			timer.reset();
			});

		server.on("/sounds/tie.mp3", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.print("GET Request() url: "); Serial.println(request->url());
			request->send(SPIFFS, "/sounds/tie.mp3", "audio/mpeg");
			timer.reset();
			});

		server.on("/sounds/loss.mp3", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.print("GET Request() url: "); Serial.println(request->url());
			request->send(SPIFFS, "/sounds/loss.mp3", "audio/mpeg");
			timer.reset();
			});

		server.on("/js/popper.min.js.map", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.print("GET Request() url: "); Serial.println(request->url());
			request->send(404);
			});
		
		server.on("/js/bootstrap.min.js.map", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.print("GET Request() url: "); Serial.println(request->url());
			request->send(404);
			});

		server.on("/css/bootstrap.min.css.map", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.print("GET Request() url: "); Serial.println(request->url());
			request->send(404);
			});

		// Setup handlers for JSON GET requests.

		server.on("/ap", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.print("GET Request() url: "); Serial.println(request->url());

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
			Serial.print("GET Request() url: "); Serial.println(request->url());

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
			Serial.print("GET Request() url: "); Serial.println(request->url());
			request->send(200, "application/json", settings.GameSettings.serialize());
			timer.reset();
			});

		server.on("/server", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.print("GET Request() url: "); Serial.println(request->url());
			ServerInfoClass info(WiFi);
			request->send(200, "application/json", info.serialize());
			timer.reset();
			});

		server.on("/system", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.print("GET Request() url: "); Serial.println(request->url());
			SystemInfoClass info;
			request->send(200, "application/json", info.serialize());
			timer.reset();
			});

		server.on("/settings", HTTP_GET, [](AsyncWebServerRequest* request) {
			Serial.print("GET Request() url: "); Serial.println(request->url());
			request->send(200, "application/json", settings.serialize());
			timer.reset();
			});

		// Setup handlers for JSON POST requests.

		server.on("/smart", HTTP_POST, [](AsyncWebServerRequest* request) {
			Serial.print("POST Request() url: "); Serial.println(request->url());
			request->send(202, "text/html", "Knoblomat running ESP32 SmartConfig for 1 minute");
			smartconfig = true;
			timer.reset();
			});

		server.on("/clear", HTTP_POST, [](AsyncWebServerRequest* request) {
			Serial.print("POST Request() url: "); Serial.println(request->url());
			request->send(202, "text/html", "Knoblomat clearing non volatile storage");
			settings.clear();
			timer.reset();
			});

		server.on("/reset", HTTP_POST, [](AsyncWebServerRequest* request) {
			Serial.print("POST Request() url: "); Serial.println(request->url());
			request->send(202, "text/html", "Knoblomat reset timer");
			timer.reset();
			});

		server.on("/reboot", HTTP_POST, [](AsyncWebServerRequest* request) {
			Serial.print("POST Request() url: "); Serial.println(request->url());
			request->send(202, "text/html", "Knoblomat rebooting");
			led = JLed(LED_BUILTIN).Blink(250, 250).Forever();
			reboot = true;
			});

		server.on("/ap", HTTP_POST, [](AsyncWebServerRequest* request) {}, NULL,
			[](AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
				Serial.print("POST Request() url: "); Serial.println(request->url());
				String json = String((char*)data).substring(0, len);
				settings.ApSettings.deserialize(json);
				settings.ApSettings.save();
				request->send(202, "application/json", settings.serialize());
				led = JLed(LED_BUILTIN).Blink(250, 250).Forever();
				reboot = true;
			});

		server.on("/wifi", HTTP_POST, [](AsyncWebServerRequest* request) {}, NULL,
			[](AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
				Serial.print("POST Request() url: "); Serial.println(request->url());
				String json = String((char*)data).substring(0, len);
				settings.WiFiSettings.deserialize(json);
				settings.WiFiSettings.save();
				request->send(202, "application/json", settings.serialize());
				led = JLed(LED_BUILTIN).Blink(250, 250).Forever();
				reboot = true;
			});

		server.on("/game", HTTP_POST, [](AsyncWebServerRequest* request) {}, NULL,
			[](AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
				Serial.print("POST Request() url: "); Serial.println(request->url());
				String json = String((char*)data).substring(0, len);
				settings.GameSettings.deserialize(json);
				settings.GameSettings.save();
				request->send(202, "application/json", settings.serialize());
				timer.reset();
			});

		// Setup handler for not found - redirects to error page.

		server.onNotFound([](AsyncWebServerRequest* request) {
			Serial.print(request->methodToString());
			Serial.print(" Request() url: ");
			Serial.print(request->url());
			Serial.println(" 404: Not Found");
			request->redirect("/error");
			timer.reset();
			});

		// Start the HTTP server
		server.begin();
		Serial.print("Listening on port "); Serial.println(ServerInfoClass::PORT);
	}
	else
	{
		Serial.print("No WiFi network - stopping");
		led = JLed(LED_BUILTIN).Blink(1000, 2000).Forever();
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
