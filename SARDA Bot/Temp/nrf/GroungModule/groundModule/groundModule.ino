/*
* Arduino Wireless Communication Tutorial
*       Example 1 - Receiver Code
*                
* by Dejan Nedelkovski, www.HowToMechatronics.com
* 
* Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 8); // CE, CSN

const byte address[6] = "00001";
const byte address2[6] = "00002";

void setup() {
  Serial.begin(9600);
  Serial.println(radio.begin());
  radio.openWritingPipe(address2);
  radio.openReadingPipe(1, address);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    char text[32] = "";
    radio.read(&text, sizeof(text));
    /*Serial.println(text);*/
    delay(100);
  }
  
}
