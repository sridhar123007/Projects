#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#define led1 3
#define led2 5

RF24 radio(9, 10); // CE, CSN
const byte addresses[][6] = {"00001", "00002", "00003"};

struct dataStruct {
  int volt;
} text;


void setup() {

  Serial.begin(9600);
  radio.begin();
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_250KBPS);
  //  radio.openWritingPipe(addresses[2]); // 00003
  radio.openReadingPipe(1, addresses[2]); // 00003
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  digitalWrite(led1, LOW);
}
void loop() {

  // receiving  2
  delay(1000);

  radio.startListening();
  if (radio.available())
  {
    while (radio.available())
    {
      // char text[32] = "";
      Serial.println("start receiving...from node1");
      radio.read(&text, sizeof(text));
      Serial.println("Got message:");
      Serial.print("volt: ");
      Serial.println(text.volt);
      Serial.println("-----------");
      if (text.volt < 185)
      {
        Serial.println(".......................lights on........");
        digitalWrite(led1, HIGH);
        //analogWrite(led2,sensor_data.volt);
      }
      else
      {
        Serial.println(".......................lights off........");
        digitalWrite(led1, LOW);
        //analogWrite(led2,sensor_data.volt);
      }
    }
    // transmitting  2

    delay(1000);
    radio.stopListening();
  }
}
