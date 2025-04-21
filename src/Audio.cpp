#include "Audio.h"


// ===================================================
// tosk run by Taskscheduler to handle the WS2812 LED ring

AudioTask::~AudioTask(){};
AudioTask::AudioTask(unsigned long interval, Scheduler *aS, void (*myCallback)()) : Task(interval, TASK_FOREVER, aS, true)
{
  _myCallback = myCallback;
};

bool AudioTask::Callback()
{
  lastMeasure = measure(100);
  _myCallback();
  return true;
};


float AudioTask::measure(int read_time_msecs){

  float samples_per_sec = 1000.0;
  int numsamples = (read_time_msecs*samples_per_sec)/1000;
  //inputStats.setInitialValue(0.0, 0.0);
  int start_millis = millis();
  int prev_millis = start_millis;
  double sigma = 0.0;
  double mean = 0.0;
  for(int i=0;i<numsamples;i++){
    int sensorValue = analogRead(34);  // read the analog in value:
    sigma += (float) sensorValue * (float) sensorValue;
    mean += sensorValue;
    float local_delay = 1000.0* (float) i / samples_per_sec - (prev_millis-start_millis);
    if(local_delay > 0)
      delay(local_delay );
    prev_millis=millis();
  }
  mean = mean / (float) numsamples;
  sigma = sqrtf(sigma/numsamples - mean*mean);
  
  int stop_millis = millis();
  //Serial.print("measure lasted ");Serial.println(stop_millis-start_millis);
      
  //Serial.print( "\tmean: " ); Serial.print( mean );
   
  // output sigma or variation values associated with the inputValue itsel
  //Serial.print( "\tsigma: " ); Serial.println( sigma );
  return sigma;
}

/*

float Audio::measure(int read_time_msecs){

  float samples_per_sec = 1000.0;
  int numsamples = (read_time_msecs*samples_per_sec)/1000;
  //inputStats.setInitialValue(0.0, 0.0);
  int start_millis = millis();
  int prev_millis = start_millis;
  double sigma = 0.0;
  double mean = 0.0;
  for(int i=0;i<numsamples;i++){
    int sensorValue = analogRead(A0);  // read the analog in value:
    sigma += (float) sensorValue * (float) sensorValue;
    mean += sensorValue;
    float local_delay = 1000.0* (float) i / samples_per_sec - (prev_millis-start_millis);
    if(local_delay > 0)
      delay(local_delay );
    prev_millis=millis();
  }
  mean = mean / (float) numsamples;
  sigma = sqrtf(sigma/numsamples - mean*mean);
  
  int stop_millis = millis();
  //Serial.print("measure lasted ");Serial.println(stop_millis-start_millis);
      
  Serial.print( "\tmean: " ); Serial.print( mean );
   
  // output sigma or variation values associated with the inputValue itsel
  Serial.print( "\tsigma: " ); Serial.println( sigma );
  return sigma;
}

*/
