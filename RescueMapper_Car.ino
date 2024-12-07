#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
#include "NewPing.h"
#include <SimpleDHT.h>
#include <Servo.h>

Servo MainServo;

struct Data_Package 
{
  int t;
  int h;
  int s;
  float df;
  float db;
  float dl;
  float dr;

};

Data_Package data;

int leftMotor1 = 4;
int leftMotor2 = 2;
int rightMotor1 = 7;
int rightMotor2 = 8;
int motorSpeedpin = 3;
int motorspeed = 40;
int turnspeed = 200;

int smokePin = A6;
int smoke;

int pinDHT11 = 5;
int ServoPin = A0;
SimpleDHT11 dht11(pinDHT11);
int temp;
int humid;
int pos1 = 0;
int pos2 = 88;

float duration1; 
float duration2; 
float soundsp;
float soundcm;
int iterations = 5;

float distancef; 
float distanceb;
float distancel; 
float distancer;

float calibrated_values[3];
float uncalibrated_values[3];   

#define TRIGGER_PIN_1  A1
#define ECHO_PIN_1     A1
#define TRIGGER_PIN_2  A2
#define ECHO_PIN_2     A2
#define MAX_DISTANCE 400
NewPing sonar1(TRIGGER_PIN_1, ECHO_PIN_1, MAX_DISTANCE);
NewPing sonar2(TRIGGER_PIN_2, ECHO_PIN_2, MAX_DISTANCE);

Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);
float degree;

RF24 radio(9, 6);  // CE, CSN

const byte receive[6] = "00001";
const byte sending[6] = "00002";

void setup()
{
  Serial.begin(9600);

  pinMode(smokePin,INPUT);

  MainServo.attach(ServoPin);
  pinMode(leftMotor1, OUTPUT); 
  pinMode(leftMotor2, OUTPUT);
  pinMode(rightMotor1, OUTPUT);
  pinMode(rightMotor2, OUTPUT);
  pinMode(motorSpeedpin, OUTPUT);

  if(!mag.begin())
  {
    while(1)
    {
      Serial.println("Stuck at mag");
    }
  }

  MainServo.write(pos1);

  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.openWritingPipe(sending);
  radio.openReadingPipe(1, receive);
  radio.startListening();

}

void loop()
{
  if (radio.available())
  {
    Serial.println("Something is avalable");
    char c[4] = "";
    radio.read(&c, sizeof(c));
    Serial.println(c);

    if(c[0]=='d')
    {
      radio.stopListening();

      getDegree();
      boolean sent = false;

      while(!sent)
      {
        sent = radio.write(&degree, sizeof(degree));
        Serial.println("One time allowed");
      }

      Serial.print("degree is sent : ");
      Serial.println(degree);  
    }

    else if(c[0]=='j')
    {
      radio.stopListening();
      getDistence();
      getData();
      boolean sent = false;

      while(!sent)
      {
        sent = radio.write(&data, sizeof(Data_Package));
        Serial.println("One time allowed");
      }

      Serial.print("Package is sent");
    }

    else if(c[0]=='m' && c[1]=='f')
    {
      Serial.print("Wait, moving : ");
      int temp = ((((int)c[2]-48)*10)+((int)c[3])-48)*10;
      Serial.println(temp);
      moveForward(temp);
    }

    else if(c[0]=='m' && c[1]=='b')
    {
      Serial.print("Wait, moving : ");
      int temp = ((((int)c[2]-48)*10)+((int)c[3])-48)*10;
      Serial.println(temp);
      moveBackward(temp);
    }

    else if(c[0]=='m' && c[1]=='l')
    {
      Serial.print("Wait, moving : ");
      int temp = ((((int)c[2]-48)*10)+((int)c[3])-48);
      Serial.println(temp);
      moveLeft(temp);
    }

    else if(c[0]=='m' && c[1]=='r')
    {
      Serial.print("Wait, moving : ");
      int temp = ((((int)c[2]-48)*10)+((int)c[3])-48);
      Serial.println(temp);
      moveRight(temp);
    }
    
    Serial.println("OK");
    delay(100);
    radio.startListening();

  }
  
}

void getSmoke()
{
  smoke=analogRead(smokePin);
}

float getDegree()
{
  sensors_event_t event; 
  mag.getEvent(&event);
 
  uncalibrated_values[0] = event.magnetic.x;
  uncalibrated_values[1] = event.magnetic.y;
  uncalibrated_values[2] = event.magnetic.z;
  transformation(uncalibrated_values);

  float heading = atan2(calibrated_values[0], calibrated_values[1]);
  
  if(heading < 0)
    heading += 2*PI;
    
  if(heading > 2*PI)
    heading -= 2*PI;
   
  degree = heading * 180/M_PI; 
  return degree;
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

void moveForward(int x)
{
  analogWrite(motorSpeedpin,motorspeed);
  digitalWrite(leftMotor1, HIGH);
  digitalWrite(leftMotor2, LOW);
  digitalWrite(rightMotor1, HIGH);
  digitalWrite(rightMotor2, LOW);

  delay(x);
  moveStop();

}

void moveBackward(int x)
{
  analogWrite(motorSpeedpin,motorspeed);
  digitalWrite(leftMotor1, LOW);
  digitalWrite(leftMotor2, HIGH);
  digitalWrite(rightMotor1, LOW);
  digitalWrite(rightMotor2, HIGH);

  delay(x);
  moveStop();

}

void moveLeft(float x)
{
  analogWrite(motorSpeedpin,turnspeed);
  digitalWrite(leftMotor1, LOW);
  digitalWrite(leftMotor2, HIGH);
  digitalWrite(rightMotor1, HIGH);
  digitalWrite(rightMotor2, LOW);

  delay(getLeftTime(x));

  moveStop();
}

void moveRight(float x)
{
  analogWrite(motorSpeedpin,turnspeed);
  digitalWrite(leftMotor1, HIGH);
  digitalWrite(leftMotor2, LOW);
  digitalWrite(rightMotor1, LOW);
  digitalWrite(rightMotor2, HIGH);

  delay(getRightTime(x));

  moveStop();
}

void moveStop()
{
  analogWrite(motorSpeedpin,0);
  digitalWrite(leftMotor1, LOW);
  digitalWrite(leftMotor2, LOW);
  digitalWrite(rightMotor1, LOW);
  digitalWrite(rightMotor2, LOW);
}

float getLeftTime(float x)
{
  return 22.9 + (3.89*x) - (7.78*pow(10.0,-3.0)*pow(x,2.0));
}

float getRightTime(float x)
{
  return 62.9 + (3.01*x) - (4.41*pow(10.0,-3.0)*pow(x,2.0));
}

void getTemparature()
{
  byte temperature = 0;
  byte humidity = 0;

  dht11.read(&temperature, &humidity, NULL);

  temp = (int)temperature; 
  humid = (int)humidity;
  delay(100);
}

void getDistence()
{
    getTemparature();
    getSmoke();
    MainServo.write(pos1);
    delay(500);

    soundsp = 331.4 + (0.606 * temp) + (0.0124 * humid);
    soundcm = soundsp / 10000;

    duration1 = sonar1.ping_median(iterations);
    duration2 = sonar2.ping_median(iterations);
    delay(100);
    distancef = (duration1 / 2) * soundcm;
    distanceb = (duration2 / 2) * soundcm;

    MainServo.write(pos2);
    delay(500);

    duration1 = sonar1.ping_median(iterations);
    duration2 = sonar2.ping_median(iterations);
    delay(100);
    distancel = (duration1 / 2) * soundcm;
    distancer = (duration2 / 2) * soundcm;
}

void getData()
{
  data.t = temp;
  data.h = humid;
  data.s = smoke;
  data.df = distancef;
  data.db = distanceb;
  data.dl = distancel;
  data.dr = distancer; 
}
