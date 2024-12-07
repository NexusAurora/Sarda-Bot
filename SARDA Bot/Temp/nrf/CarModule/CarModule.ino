#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 6); // CE, CSN

const byte address[6] = "00001";
const byte address2[6] = "00002";

void setup() {
  Serial.begin(9600);
  Serial.println(radio.begin());
  radio.openWritingPipe(address);
  radio.openReadingPipe(1,address2);
  radio.setPALevel(RF24_PA_LOW);
  radio.stopListening();
}

void loop() {
  const char text[] = "Hello World";
  Serial.println(radio.write(&text, sizeof(text)));
  delay(1000);
}