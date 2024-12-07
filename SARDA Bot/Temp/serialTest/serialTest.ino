float degree = 0;
void setup() 
{
  Serial.begin(74880);
  Serial.setTimeout(5);
}

void loop() 
{
  if (!Serial.available()) return;
  String str = Serial.readString();
  
  if (str[0] == 'd')
  {
    degree++;
    if(degree>360)
    {
      degree = 0;
    }
    byte* fBuffer = reinterpret_cast<byte*>(&degree);
    Serial.write(fBuffer, 4);
  }
}
