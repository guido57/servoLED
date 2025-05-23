#include <Arduino.h>
#include "tools.h"
#include <captive_portal.h>
#include <NTPClient.h>

/** Show the settings form */
void handleRoot(); 

/** Handle the settings form and redirect to WLAN config page again */
void handleSettingsSave();

/** Redirect to captive portal if we got a request for another domain. Return true in that case so the page handler do not try to handle the request again. */
boolean captivePortal();

/** Wifi config page handler */
void handleWifi();

/** Handle the WLAN save form and redirect to WLAN config page again */
void handleWifiSave();

boolean captivePortal();

void handleHygroTempPower();
void handleHygrostat();

void handleNotFound();

extern void saveCredentials();
extern void loadCredentials();
extern float readTemperature();
extern float readVoltage();
// extern NTPClient timeClient;
