#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "captive_portal.h"
#include "handleHttp.h"

extern Scheduler myScheduler;
//=== CaptivePortal stuff ================

/* hostname for mDNS. Should work at least on windows. Try http://esp32.local */
const char *myHostname = "esp32";

// Settings
String ssid;
String password;
String hh_on,mm_on, hh_off,mm_off;
String softAP_ssid;
String softAP_password = APPSK;

// Web server
//extern ESP8266WebServer web_server;
extern WebServer web_server;

// NTP settings
//const char* ntpServer = "pool.ntp.org";
//const long  gmtOffset_sec = 3600; // GMT + 1
//Change the Daylight offset in milliseconds. If your country observes Daylight saving time set it to 3600. Otherwise, set it to 0.
//const int   daylightOffset_sec = 3600;


// DNS server
//const byte DNS_PORT = 53;
//DNSServer dnsServer;

// Web server
WebServer web_server(80);

/* Soft AP network parameters */
IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);
/** Should I connect to WLAN asap? */
boolean connect;

/** Last time I tried to connect to WLAN */
unsigned long lastConnectTry = 0;

/** Last time (milliseconds) I tried to ping an external IP address (usually the gateway) */
unsigned long lastPing = 0;

/** Current WLAN status */
unsigned int status = WL_IDLE_STATUS;

// =====================================================
wl_status_t connectWifi() {
    Serial.println("Connecting as wifi client...");
    WiFi.disconnect();
    //WiFi.mode(WIFI_STA);
    Serial.print("ssid=");Serial.println(ssid);
    Serial.print("password=");Serial.println(password);
    WiFi.begin(ssid.c_str(), password.c_str());
    return (wl_status_t) WiFi.waitForConnectResult();
}

// void printLocalTime()
// {
 
//   if(!timeClient.update()){
//     Serial.println("Failed to obtain time");
//     return;
//   }
//   Serial.println( timeClient.getFormattedTime());
//}

// =====================================================
// Callback for the TaskWIFI  
/**
   check WIFI conditions and try to connect to WIFI.
 * @return void
 */
void webserver_loop(void){
  //HTTP
  web_server.handleClient();
}
// =====================================================
// tosk run by Taskscheduler to handle WIFI  
class TaskWebServer : public Task {
  public:
    void (*_myCallback)();
    ~TaskWebServer() {};
    TaskWebServer(unsigned long interval, Scheduler* aS, void (* myCallback)() ) :  Task(interval, TASK_FOREVER, aS, true) {
      _myCallback = myCallback;
    };
    bool Callback(){
      _myCallback();
      return true;     
    };
};
Task * myTaskWebServer;

struct
{
  char ssid_chars[40] = "";
  char password_chars[40] = "";
  char th_ip_chars[20] ="0.0.0.0";
  int port = 0;
  int sleep_seconds = 5; 
  int was_set = 9090;
  // turn on hh:mm 
  char hh_on[3] = "00";
  char mm_on[3] = "00";
  // turn off hh:mm
  char hh_off[3] = "00";
  char mm_off[3] = "00";
}creds;
//===================================================
/** Load WLAN credentials from EEPROM */
void loadCredentials() {
  EEPROM.begin(2048);
  
  EEPROM.get(0,creds); // load everythng
  EEPROM.end();
  
  if (creds.was_set == 9090) {

    ssid = String(creds.ssid_chars);
    password = String(creds.password_chars);
    hh_on = String(creds.hh_on);  
    mm_on = String(creds.mm_on);  
    hh_off = String(creds.hh_off);  
    mm_off = String(creds.mm_off);  

  }else{
    Serial.printf("creds.was_set != 9090 -> set default settings\r\n");
    Serial.println("set default ssid");
    ssid = "";
    Serial.println("set default password");
    password = "";
  }
  
  Serial.println("Recovered settings:");
  Serial.println(ssid.length() > 0 ? ssid : "<no ssid>");
  Serial.println(password.length() > 0 ? password : "<no password>");
  Serial.printf("On time=%s:%s   Off time=%s:%s\r\n", hh_on.c_str(),mm_on.c_str(),hh_off.c_str(),mm_off.c_str()); 
}

/** Store WLAN credentials to EEPROM */
void saveCredentials() {

  Serial.println("Saving settings ...");
  
  Serial.printf("ssid=%s\r\n",ssid.c_str());
  Serial.printf("password=%s\r\n",password.c_str());
  Serial.printf("On time=%s:%s   Off time=%s:%s\r\n", hh_on.c_str(),mm_on.c_str(),hh_off.c_str(),mm_off.c_str()); 
  
  EEPROM.begin(2048);
  strcpy(creds.ssid_chars, ssid.c_str());
  strcpy(creds.password_chars, password.c_str());
  strcpy(creds.hh_on,hh_on.c_str());
  strcpy(creds.mm_on,mm_on.c_str());
  strcpy(creds.hh_off,hh_off.c_str());
  strcpy(creds.mm_off,mm_off.c_str());
  creds.was_set = 9090;

  EEPROM.put(0, creds);
  EEPROM.commit();
  EEPROM.end();
}

void AccessPointSetup(){

  softAP_ssid = "ESP32_" + WiFi.macAddress();

  // Access Point Setup
  if(WiFi.getMode() !=  WIFI_AP && WiFi.getMode() != WIFI_AP_STA){
      
    WiFi.mode(WIFI_AP);
    
    /* You can remove the password parameter if you want the AP to be open. */
    WiFi.softAP(softAP_ssid.c_str(), softAP_password.c_str());
    delay(2000); 
    WiFi.softAPConfig(apIP, apIP, netMsk);
    delay(100); // Without delay I've seen the IP address blank
    Serial.println("Access Point set:");
  }else
  {
    Serial.println("Access Point already set:");
  }
  Serial.printf("    SSID: %s\r\n", softAP_ssid.c_str());
  Serial.print("    IP address: ");
  Serial.println(WiFi.softAPIP());
}

void WebServerSetup(){

    /* Setup web pages: root, wifi config pages, SO captive portal detectors and not found. */
  web_server.on("/", handleRoot);
  web_server.on("/settingssave", handleSettingsSave);
  web_server.on("/wifi", handleWifi);
  web_server.on("/wifisave", handleWifiSave);
  web_server.on("/generate_204", handleRoot);  //Android captive portal. Maybe not needed. Might be handled by notFound handler.
  web_server.on("/fwlink", handleRoot);  //Microsoft captive portal. Maybe not needed. Might be handled by notFound handler.
  web_server.onNotFound(handleNotFound);
  web_server.begin(); // Web server start
  Serial.println("HTTP server started"); 
  
}
// =====================================================
// Callback for the TaskWIFI  
/**
   check WIFI conditions and keep connected to WIFI.
 * @return void
 */
void WiFi_loop(void){

  // handle a connect request (request is true)
  if (connect) {
    Serial.println("Connect requested");
    connect = false;
    connectWifi();
    lastConnectTry = millis();
  }
  // if not connected try to connect after 60 seconds from last attempt  
  unsigned int s = WiFi.status();
  if (s != WL_CONNECTED && millis() > (lastConnectTry + 60000))
  {
    /* If WLAN disconnected and idle try to connect */
    /* Don't set retry time too low as retry interfere the softAP operation */
    connect = true;
  }

  // if WLAN status changed
  if (status != s)
  {
    Serial.printf("Status changed from %d to %d:\r\n",status,s);
    status = s;
    if (s == WL_CONNECTED){
      /* Just connected to WLAN */
      Serial.printf("\r\nConnected to %s\r\n",ssid.c_str());
      _PP("IP address");
      Serial.println(WiFi.localIP());

      _PL("just connected -> Turn off the Access Point")
      WiFi.mode(WIFI_STA);
    }
    else if (s == WL_NO_SSID_AVAIL){
      _PL("no SSID available -> turn on the Access Point");
      AccessPointSetup();
    }
    else{
      _PL("not connected -> turn on the Access Point");
      AccessPointSetup();
    }
  }
}
// =====================================================
// tosk run by Taskscheduler to handle WIFI  
class MyWiFi {
  public:
    ~MyWiFi() {};
    MyWiFi() {
      
    };
    
};

void CaptivePortalSetup(){
    loadCredentials(); // Load WLAN credentials from network
}
