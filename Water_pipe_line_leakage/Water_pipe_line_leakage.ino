#include "XBee.h"
#include <SoftwareSerial.h>

// xbee module config in API escapeing mode

uint8_t recv = 5; 
uint8_t trans = 6; 
SoftwareSerial soft_serial(recv, trans);

const int sensor_pin = A1;  /* Soil moisture sensor O/P pin */

XBee xbee = XBee(); 
ZBRxIoSampleResponse ioSample = ZBRxIoSampleResponse();

// Specify the address of the remote XBee (this is the SH + SL)
XBeeAddress64 addr64 = XBeeAddress64(0x13A200, 0x41AE02F1);//0013A200 41079C40--r     0013A200 415D1AE7---c  //0013A200  41ADFD84

int x=2000;
String xbee_received;
  float moisture_percentage;
  int sensor_analog;

volatile int  flow_frequency;  // Measures flow meter pulses
unsigned int  l_hour;          // Calculated litres/hour                      
unsigned char flowmeter = 3;  // Flow Meter Pin number
unsigned long currentTime;
unsigned long cloopTime;

void flow ()                  // Interruot function
{ 
   flow_frequency++;
} 

void setup() 
{
 // analogReference(EXTERNAL);
  pinMode(flowmeter, INPUT);
    digitalWrite(flowmeter, HIGH);
   Serial.begin(9600); 
   attachInterrupt(0, flow, RISING); // Setup Interrupt 
                                     // see http://arduino.cc/en/Reference/attachInterrupt
   sei();                            // Enable interrupts  
   currentTime = millis();
   cloopTime = currentTime;

  soft_serial.begin(9600);
  xbee.setSerial(soft_serial);
 
}
void loop()
{  
  currentTime = millis();
   // Every second, calculate and print litres/hour

   if(currentTime >= (cloopTime + 1000))
   {     
      cloopTime = currentTime;              // Updates cloopTime
      // Pulse frequency (Hz) = 7.5Q, Q is flow rate in L/min. (Results in +/- 3% range)
      l_hour = (flow_frequency * 60 / 7.5); // (Pulse frequency x 60 min) / 7.5Q = flow rate in L/hour 
      flow_frequency = 0;                   // Reset Counter
      Serial.print(l_hour);            // Print litres/hour
      Serial.println(" L/hour");
      
   }
    
    //delay(1000); 
      String payloadStr = buildJSON();
      publishZWaveTx(payloadStr);
     Serial.print("xbee transmit:");
     Serial.println(payloadStr);

}

void publishZWaveTx(String payloadStr) 
{
  char payload[payloadStr.length() + 1];
  payloadStr.toCharArray(payload, payloadStr.length() + 1);   

  ZBTxRequest zbTx = ZBTxRequest(addr64, (uint8_t *) payload, strlen(payload));
  delay(1000);
 // Send your request
  xbee.send(zbTx);
}

String buildJSON() 
{
  String payloadStr;
  payloadStr = "#";
  payloadStr +=  l_hour;
  payloadStr +=  "@";
  Serial.println(payloadStr);
  return (payloadStr);
}
