#include "main.h"

#include "soc/soc.h"          // For brownout detector registers
#include "soc/rtc_cntl_reg.h" // For RTC_CNTL_BROWN_OUT_ENA

// =============================================
//   servo and audio declarations and settings
// =============================================
//int servo_pin = 9;  // for Arduino microcontroller
//int servo_pin = D7;  // for ESP8266 microcontroller
int servo_pin = D4;  // GPIO 2 for  D1 mini microcontroller

// Define PWM channel, frequency, and resolution
#define PWM_CHANNEL 0
#define PWM_FREQUENCY 50 // 50 Hz for servo
#define PWM_RESOLUTION 10 // 10-bit resolution (0-1023)


//Servo myservo;
int angle = 0; 

MyAudio * myAudioTask;

// On and Off Hours and Minutes
extern String hh_on, mm_on, hh_off, mm_off;
bool audio_light_on = false;

//extern NTPClient timeClient;
// =============================================
//   WS2812 ring declarations and settings
// ===========================================
Scheduler myScheduler;

//  GPIO4 D2 for D1 mini - SERIAL DATA PIN
int WS2812Ring_pin = 4;
Adafruit_NeoPixel * strip;

TaskWS2812Ring * MyTaskWS2812Ring;
// =============================================
int audio_task_interval_msecs = 100;  
int ten_secs_intervals = 10000/audio_task_interval_msecs;
int count_intervals =0;
const char* ntpServer = "europe.pool.ntp.org";
tm timeinfo;
bool got_local_time = false;

extern void WiFi_loop();

void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  //Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void AudioTask_loop(){
  if(audio_light_on){
    float sigma = myAudioTask->measure(100);
    int br;
    int motor_pwm;
    if(sigma > 50.0){  
      motor_pwm = 220 ;
      // if(motor_pwm > 255)
      //   motor_pwm = 255;
    
      Serial.printf("%lu sigma=%.1f motor_pwm=%d \r\n",millis(),sigma, motor_pwm);
      
    }else{
      motor_pwm = 0;
    }

    if(sigma > 5.0){  
      br = sigma;
      if(br> 255)
        br=255;
      // if(br>0)
      //   count_intervals = 0;
      Serial.printf("%lu sigma=%.1f br=%d \r\n",millis(), sigma, br);
        
    }else{
      br = 0;
    }
    strip->setBrightness(br);
    analogWrite(servo_pin, motor_pwm);
    //Serial.printf("sigma=%.1f br=%d motor_pwm=%d\r\n", sigma, br, motor_pwm);
    
    
    // if(count_intervals==ten_secs_intervals){
    //   strip->setBrightness(16);
    //   Serial.println("10 seconds of silence");
    //   count_intervals = 0;
    // }else{
    //   count_intervals++;
    // }
  }else
  {
    strip->setBrightness(0);
    analogWrite(servo_pin, 0);
  }

  // check if we ar ON or OFF
  uint16_t mmm_on = hh_on.toInt() * 60 + mm_on.toInt();
  uint16_t mmm_off = hh_off.toInt() * 60 + mm_off.toInt();

  //init and get the time
  //configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  #define TZ "CET-1CEST,M3.5.0/2,M10.5.0/3"
  configTzTime( TZ, ntpServer); //sets TZ and starts NTP sync
  printLocalTime();

  if(!getLocalTime(&timeinfo, 5000U)){ // 500msecs timeout
    Serial.println("Failed to obtain time");
    got_local_time = false;
  
  }else
    got_local_time = true;


  int16_t now_t = timeinfo.tm_hour * 60 + timeinfo.tm_min;
  //Serial.printf("now_t=%d mmm_on=%d mmm_off=%d\r\n",now_t,mmm_on,mmm_off);
  if ((mmm_on < mmm_off && now_t >= mmm_on && now_t < mmm_off) || (mmm_on > mmm_off && (now_t >= mmm_on || now_t < mmm_off)))
  {
    if (audio_light_on == false)
    {
      Serial.println("Turn on radio and lights");
      audio_light_on = true;
    }
  }
  else
  {
    if (audio_light_on)
    {
      Serial.println("Turn off radio and lights");
      audio_light_on = false;
    }
  }
}

void setup() 
{ 
  // Disable brownout detector
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  
  Serial.begin(921600);
  delay(1000);
  Serial.printf("servo_pin is %d",servo_pin);
  
  // Configure PWM for ESP32
  ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION); // Set frequency and resolution
  ledcAttachPin(servo_pin, PWM_CHANNEL); // Attach the servo pin to the PWM channel
 
  strip = new Adafruit_NeoPixel(12, WS2812Ring_pin, NEO_GRB + NEO_KHZ800);

  strip->begin();
  strip->setBrightness(50);
  strip->show(); // Initialize all pixels to 'off'

  //MyTaskWS2812Ring = new TaskWS2812Ring(25,&myScheduler, WS2812Ring_loop); 

  // Configure ADC attenuation to allow 0–3.3V range
  analogSetPinAttenuation(34, ADC_11db); // ADC_11db allows 0–3.3V input

  //myAudioTask = new AudioTask(audio_task_interval_msecs,&myScheduler,AudioTask_loop); 
  
  CaptivePortalSetup();
  
  connectWifi(); // Connect to WLAN if credentials are available
  WebServerSetup(); 


} 

int ws2812_ndx = 35;
unsigned long wifi_elapsed = 0UL;
unsigned long ws2812_elapsed = 0UL;
unsigned long audio_elapsed = 0UL; 
unsigned long web_elapsed = 0UL;
unsigned long onesec = 0UL;
unsigned long wifi_avg = 0UL;
unsigned long ws2812_avg = 0UL;
unsigned long audio_avg = 0UL; 
unsigned long web_avg = 0UL;
int count = 0;
void loop(){
    wifi_elapsed = millis();
    WiFi_loop();
    wifi_elapsed = millis() - wifi_elapsed;
    wifi_avg += wifi_elapsed;

    ws2812_elapsed = millis();
    WS2812Ring_loop(& ws2812_ndx);
    ws2812_elapsed = millis() - ws2812_elapsed; 
    ws2812_avg += ws2812_elapsed;

    audio_elapsed = millis();
    AudioTask_loop();
    audio_elapsed = millis() - audio_elapsed;
    audio_avg += audio_elapsed;

    web_elapsed = millis();
    web_server.handleClient();
    web_elapsed = millis() - web_elapsed;
    web_avg += web_elapsed;
    
    count ++;

    if(millis() - onesec > 1000UL){
      onesec = millis();
      wifi_avg /= count;
      ws2812_avg /= count;  
      audio_avg /= count;
      web_avg /= count;  
      Serial.printf("%lu wifi_avg=%d ws2812_avg=%d audio_avg=%d web_avg=%d\r\n",
            millis(), wifi_avg,ws2812_avg,audio_avg,web_avg);
      wifi_avg = 0UL;
      ws2812_avg = 0UL;   
      audio_avg = 0UL;
      web_avg = 0UL;  
      count = 0;   
    }
    
}
  



