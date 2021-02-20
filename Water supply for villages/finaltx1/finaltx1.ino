

#include <SPI.h> //Call SPI library so you can communicate with the nRF24L01+
#include <nRF24L01.h> //nRF2401 libarary found at https://github.com/tmrh20/RF24/
#include <RF24.h> //nRF2401 libarary found at https://github.com/tmrh20/RF24/
#include "DHT.h"

#define DHTPIN 2     // what pin we're connected to
#define flame 3
#define DHTTYPE DHT11
#define buzzer 6
const int pinCE = 9; //This pin is used to set the nRF24 to standby (0) or active mode (1)
const int pinCSN = 10; //This pin is used to tell the nRF24 whether the SPI communication is a command or message to send out

RF24 wirelessSPI(pinCE, pinCSN); // Create your nRF24 object or wireless SPI connection
char text[150]="";
const uint64_t wAddress = 0xB00B1E50D5LL;              // Pipe to write or transmit on
DHT dht(DHTPIN, DHTTYPE);
void setup()
{
  Serial.begin(9600);   //start serial to communicate process
 //create unique seed value for random number generation
 pinMode(flame,INPUT);
 pinMode(buzzer,OUTPUT);
  dht.begin();
  wirelessSPI.begin();            //Start the nRF24 module
  wirelessSPI.openWritingPipe(wAddress);        //open writing or transmit pipe
  wirelessSPI.stopListening(); //go into transmit mode
}


void loop()
{
   
  int h = dht.readHumidity();
  int t = dht.readTemperature();
  int smokeval=analogRead(A0);
  int flameval=digitalRead(flame);
  int id=1;
  int buzzerval =digitalRead(buzzer);
  Serial.println("flameval");
  Serial.println(flameval);
  Serial.println("id=");
  Serial.println(id);
  
    String payload = "\"t\":";
  payload +=  String(t);
  payload += ",""\"h\":";
  payload +=  String(h);
  payload += ",""\"s\":";
  payload +=  String(smokeval);
 payload += ",""\"f\":";
  payload +=  String(flameval);
  payload +=  "}";
 
  delay(1000);
  int s=payload.length();
  delay(10);
payload.toCharArray(text, s);
  delay(1000);

Serial.println("text");
Serial.println(text);

  wirelessSPI.write( &text, sizeof(text) );
  wirelessSPI.stopListening(); //go back to transmit mode
  delay(1000);
}
