#include <WiFiClient.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <EEPROM.h>

// ===== TaskScheduler stuff =============
#define _TASK_SLEEP_ON_IDLE_RUN // Enable 1 ms SLEEP_IDLE powerdowns between tasks if no callback methods were invoked during the pass 
#define _TASK_STATUS_REQUEST    // Compile with support for StatusRequest functionality - triggering tasks on status change events in addition to time only
#define _TASK_WDT_IDS           // Compile with support for wdt control points and task ids
#define _TASK_PRIORITY          // Support for layered scheduling priority
#define _TASK_TIMEOUT           // Support for overall task timeout 
#define _TASK_OO_CALLBACKS
#include <TaskSchedulerDeclarations.h>


// ====== Access Point stuff ================
/* Set these to your desired softAP credentials. They are not configurable at runtime */
#ifndef APSSID
#define APPSK "12345678"
#endif


/* hostname for mDNS. Should work at least on windows. Try http://esp8266.local */
extern const char *myHostname;

// settings
/* Don't set these wifi credentials. They are configurated at runtime and stored on EEPROM */
extern String ssid;
extern String password;
extern String hh_on,mm_on, hh_off,mm_off;
extern int no_conn_count;

// Web server
//extern ESP8266WebServer web_server;
extern WebServer web_server;


/* Soft AP network parameters */
extern String softAP_ssid;
extern String softAP_password;
extern IPAddress apIP;
extern IPAddress netMsk;

/** Should I connect to WLAN asap? */
extern boolean connect;

/** Last time I tried to connect to WLAN */
extern unsigned long lastConnectTry;

/** Current WLAN status */
extern unsigned int status;


wl_status_t connectWifi(); 
void WebServerSetup();
void AccessPointSetup();
void CaptivePortalSetup();

#define _DEBUG_
//#define _TEST_

#ifdef _DEBUG_
#define _PP(a) Serial.print(a);
#define _PL(a) Serial.println(a);
#else
#define _PP(a)
#define _PL(a)
#endif

