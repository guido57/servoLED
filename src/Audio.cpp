#include "Audio.h"


// ===================================================
// tosk run by Taskscheduler to handle the WS2812 LED ring

MyAudio::~MyAudio(){};
MyAudio::MyAudio(){
};


float MyAudio::measure(int read_time_msecs){

  int numsamples = 100;
  int start_millis = millis();
  int prev_millis = start_millis;
  double sigma = 0.0;
  double mean = 0.0;
  for(int i=0;i<numsamples;i++){
    int sensorValue = analogRead(34);  // read the analog in value:
    sigma += (float) sensorValue * (float) sensorValue;
    mean += sensorValue;
    vTaskDelay(1); // 1ms delay to allow other tasks to run
  }
  mean = mean / (float) numsamples;
  sigma = sqrtf(sigma/numsamples - mean*mean);
  
  //int stop_millis = millis();
  //Serial.print("measure lasted ");Serial.println(stop_millis-start_millis);
      
  return sigma;
}

