#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <ArduinoJson.h>

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

RF24 radio(9, 8);  // CE, CSN
float degree;

String jsonString = "";

const byte sending[6] = "00001";
const byte receive[6] = "00002";

int Buzzer = 3;
int yellow = 5;
int green = 6;

void setup()
{
  Serial.begin(74880);
  
  pinMode(Buzzer,OUTPUT);
  pinMode(yellow,OUTPUT);
  pinMode(green,OUTPUT);
  
  
  if(radio.begin()==0)
  {
    digitalWrite(Buzzer,HIGH);
    digitalWrite(yellow,HIGH);
    
    while(true);
  }
  else
  {
    analogWrite(green,50);
  }
  radio.setPALevel(RF24_PA_LOW);
  radio.openWritingPipe(sending);
  radio.openReadingPipe(1,receive);
  radio.stopListening();
}
void loop()
{
  if (Serial.available())
  {
    String str = Serial.readString();
    
    if (str[0] == 'd')
    {
      char c[4] = "d";

      boolean sent = false;
      
      while(!sent)
      {
        sent = radio.write(&c, sizeof(c));
        analogWrite(yellow,100);
      }
      analogWrite(yellow,0);
      
      radio.startListening();

      while (!radio.available());
      radio.read(&degree, sizeof(degree));
  
      byte* fBuffer = reinterpret_cast<byte*>(&degree);
      Serial.write(fBuffer, 4);
    }

    else if (str[0] == 'j')
    {
      char c[4] = "j";

      boolean sent = false;
      
      while(!sent)
      {
        sent = radio.write(&c, sizeof(c));
        analogWrite(yellow,100);
      }
      analogWrite(yellow,0);
      
      radio.startListening();

      jsonString = "";
      while (!radio.available());
      radio.read(&data, sizeof(Data_Package));
      getJson();
      Serial.println(jsonString);
    }
    
    else if (str[0] == 'm')
    {
      char c[4];

      for(int i=0;i<4;i++)
      {
        c[i]= str[i];
      }

      boolean sent = false;
      
      while(!sent)
      {
        sent = radio.write(&c, sizeof(c));
        analogWrite(yellow,100);
      }
      analogWrite(yellow,0);
    }
    digitalWrite(yellow,HIGH);
    digitalWrite(Buzzer,HIGH);
    delay(100);
    digitalWrite(Buzzer,LOW);
    digitalWrite(yellow,LOW);
    radio.stopListening();
  }
}

void getJson()
{
  StaticJsonDocument<64> doc;
  jsonString = "";

  doc["t"] = data.t;
  doc["h"] = data.h;
  doc["s"] = data.s;
  doc["df"] = data.df;
  doc["db"] = data.db;
  doc["dl"] = data.dl;
  doc["dr"] = data.dr;

  serializeJson(doc, jsonString);
}
