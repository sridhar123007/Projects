#include <SPI.h> //Call SPI library so you can communicate with the nRF24L01+
#include <nRF24L01.h> //nRF2401 libarary found at https://github.com/tmrh20/RF24/
//#include <RF24Network.h>
#include <RF24.h> //nRF2401 libarary found at https://github.com/tmrh20/RF24/

RF24 radio(9,10);                    /* Creating instance 'radio'  ( CE , CSN )   CE -> D7 | CSN -> D8 */                              
//const byte Address[6] = {"00005"} ;     /* Address to which data to be transmitted*/
const uint64_t Address = 0xB00B1E50D5LL; 
int flowPin = 2;    //This is the input pin on the Arduino
unsigned int flowRate;    //This is the value we intend to calculate. 
volatile int count; //This integer needs to be set as volatile to ensure it updates correctly during the interrupt process.  
 
void setup() {
  // put your setup code here, to run once:
  pinMode(flowPin, INPUT);           //Sets the pin as an input
  attachInterrupt(0, Flow, RISING);  //Configures interrupt 0 (pin 2 on the Arduino Uno) to run the function "Flow"  
  Serial.begin(9600);  //Start Serial
  radio.begin ();                 /* Activate the modem*/
  radio.openWritingPipe (Address); /* Sets the address of transmitter to which program will send the data */
  radio.stopListening();
}
void loop() {
  // put your main code here, to run repeatedly:  
  count = 0;      // Reset the counter so we start counting from 0 again
  interrupts();   //Enables interrupts on the Arduino
  delay (1000);   //Wait 1 second 
  noInterrupts(); //Disable the interrupts on the Arduino
   
 /* //Start the math
  flowRate = (count * 2.25);        //Take counted pulses in the last second and multiply by 2.25mL 
  flowRate = flowRate * 60;         //Convert seconds to minutes, giving you mL / Minute
  flowRate = flowRate / 1000;       //Convert mL to Liters, giving you Liters / Minute
 
  Serial.println(flowRate); */        //Print the variable flowRate to Serial
flowRate = (count * 60/7.5);
Serial.print(flowRate, DEC);
Serial.println(" Liter/hour");
radio.stopListening (); 
unsigned int flowRate2=flowRate;
radio.write(&flowRate2, sizeof(flowRate2));            /* Sending data over NRF 24L01*/
Serial.print("Transmitting Data : ");
Serial.println(flowRate);  
delay(1000);
}
 
void Flow()
{
   count++; //Every time this function is called, increment "count" by 1
}
