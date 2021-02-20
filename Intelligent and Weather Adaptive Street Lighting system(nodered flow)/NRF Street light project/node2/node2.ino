#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define led1 3
#define led2 5

RF24 radio(9,10);                // Set up nRF24L01 radio on SPI bus plus pins 7 & 8

const byte addresses[][6] = {"00001", "00002","00003"};

struct dataStruct{
  int volt;
}sensor_data;



void setup() {

  Serial.begin(9600);
  Serial.println("starting......");
  restartRadio();                    // turn on and configure radio  
  Serial.println("restarting radio");
  radio.startListening();
  Serial.println("Listening for sensor values..."); 
  pinMode(led1,OUTPUT);
  pinMode(led2,OUTPUT);
}

void loop() {

  delay(1000);
  radio.startListening();
  if (radio.available()) 
  {
    while (radio.available()) {
      radio.read(&sensor_data, sizeof(sensor_data));
      Serial.println("Got message:");
      Serial.print("volt: ");
      Serial.println(sensor_data.volt);
      Serial.println("-----------");
      if(sensor_data.volt<185) 
      {
      Serial.println(".......................lights on........");
      digitalWrite(led1,HIGH);
      //analogWrite(led2,sensor_data.volt);
     }
     else 
     {
      Serial.println(".......................lights off........");
      digitalWrite(led1,LOW);
      //analogWrite(led2,sensor_data.volt);
     }

    }
  
       
        radio.stopListening();
         delay(2000);
      //  sensor_data.volt=100;
        radio.write(&sensor_data, sizeof(sensor_data));
        
  }
}


void restartRadio(){
  radio.begin(); // Start up the radio
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_250KBPS);  
  //radio.openReadingPipe(1,pipes[1]);  
  //radio.openWritingPipe(pipes[0]);
  radio.openWritingPipe(addresses[2]); // 00003
  radio.openReadingPipe(1, addresses[1]); // 00001
}
