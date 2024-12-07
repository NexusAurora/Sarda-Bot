#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>

Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);
float degree;

void setup() 
{
  Serial.begin(9600);
  if(!mag.begin())
  {
    while(1);
  }
}

void loop(void) 
{
  getDegree();
  Serial.println(degree);
}

void getDegree()
{
  sensors_event_t event; 
  mag.getEvent(&event);
 
  //Serial.print("X: "); Serial.print(event.magnetic.x); Serial.print("  ");
  //Serial.print("Y: "); Serial.print(event.magnetic.y); Serial.print("  ");
  //Serial.print("Z: "); Serial.print(event.magnetic.z); Serial.print("  ");
 
  float heading = atan2(event.magnetic.y, event.magnetic.x);
  
  if(heading < 0)
    heading += 2*PI;
    
  if(heading > 2*PI)
    heading -= 2*PI;
   
  float headingDegrees = heading * 180/M_PI; 
  degree = headingDegrees;
}
