
#include <SPI.h> //Call SPI library so you can communicate with the nRF24L01+
#include <nRF24L01.h> //nRF2401 libarary found at https://github.com/tmrh20/RF24/
#include <RF24.h> //nRF2401 libarary found at https://github.com/tmrh20/RF24/



const int pinCE = 9; //This pin is used to set the nRF24 to standby (0) or active mode (1)
const int pinCSN = 10; //This pin is used to tell the nRF24 whether the SPI communication is a command or message to send out
//#define gas A0
#define led1 3

RF24 wirelessSPI(pinCE, pinCSN); // Declare object from nRF24 library (Create your wireless SPI)
//const uint64_t rAddress = 0xB00B1E50D2LL;  //Create pipe addresses for the 2 nodes to recieve data, the "LL" is for LongLong type
const byte address[6] = "00001";
// int gas_value, pos, weight = 10;
//byte pipeNum = 0;
//byte newPacket = false;
//byte jar1, jar2,jar;
//int id = 0;
void setup()
{
  pinMode(led1, OUTPUT);
  //pinMode(gas, OUTPUT);
  //myservo.attach(D0);//pin attached to servo motor
  Serial.begin(115200);  //start serial to communication
  Serial.println(); 
  wirelessSPI.begin();  //Start the nRF24 module
  wirelessSPI.openReadingPipe(0,address);     //open pipe o for recieving meassages with pipe address
  wirelessSPI.startListening();                 // Start listening for messages

}
void loop()
{
  if (wirelessSPI.available()) {
    //newPacket = true;
    char command[32] = "";
    wirelessSPI.read(&command, sizeof(command));
    Serial.println("command   ");
    Serial.print(command);
    String cmd=String(command);
    delay(1000);

if(cmd=="lighton")
{
  digitalWrite(led1,HIGH);
  Serial.println(".......lights are on..........");
}
else if(cmd== "lightoff"){
   digitalWrite(led1,LOW);
   Serial.println(".......lights are off..........");   
  }
  else
  {
    Serial.println(".....PLEASE PASS SOME COMMANDS.........");
    }
  }  }
