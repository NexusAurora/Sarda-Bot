
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>

Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);

float calibrated_values[3];
float uncalibrated_values[3];   

void setup(void) 
{
  //Serial.begin(9600);
  
  if(!mag.begin())
  {
    while(1);
  }
}

void loop(void) 
{
  sensors_event_t event; 
  mag.getEvent(&event);
 
  //Serial.print("X: "); Serial.print(event.magnetic.x); Serial.print("  ");
  //Serial.print("Y: "); Serial.print(event.magnetic.y); Serial.print("  ");
  //Serial.print("Z: "); Serial.print(event.magnetic.z); Serial.print("  ");Serial.println("uT");

  uncalibrated_values[0] = event.magnetic.x;
  uncalibrated_values[1] = event.magnetic.y;
  uncalibrated_values[2] = event.magnetic.z;

  /*Serial.print("uncalibrated : ");
  Serial.flush(); 
  Serial.print(uncalibrated_values[0]); 
  Serial.print(",");
  Serial.print(uncalibrated_values[1]);
  Serial.print(",");
  Serial.print(uncalibrated_values[2]);
  Serial.print(" ");*/

  transformation(uncalibrated_values);

  /*Serial.print("calibrated : ");
  Serial.flush(); 
  Serial.print(calibrated_values[0]); 
  Serial.print(",");
  Serial.print(calibrated_values[1]);
  Serial.print(",");
  Serial.print(calibrated_values[2]);
  Serial.println(" ");*/


  float heading = atan2(calibrated_values[0], calibrated_values[1]);
    
  if(heading < 0)
    heading += 2*PI;
    
  if(heading > 2*PI)
    heading -= 2*PI;
   
  float headingDegrees = heading * 180/M_PI;
  Serial.println(headingDegrees);
  delay(100);
  
}

void transformation(float uncalibrated_values[3])    
{
  
  double calibration_matrix[3][3] = 
  {
    {2.834, -0.044, 0.066},
    {0.002, 2.528, -0.292},
    {0.127, -0.549, 3.201}  
  };
  double bias[3] = 
  {
    34.345,
    -10.353,
    50.164
  };  
  for (int i=0; i<3; ++i) uncalibrated_values[i] = uncalibrated_values[i] - bias[i];
  float result[3] = {0, 0, 0};
  for (int i=0; i<3; ++i)
    for (int j=0; j<3; ++j)
      result[i] += calibration_matrix[i][j] * uncalibrated_values[j];
  for (int i=0; i<3; ++i) calibrated_values[i] = result[i];
}
