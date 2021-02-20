
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>
#include <SPI.h> //Call SPI library so you can communicate with the nRF24L01+
#include <nRF24L01.h> //nRF2401 libarary found at https://github.com/tmrh20/RF24/
#include <RF24.h> //nRF2401 libarary found at https://github.com/tmrh20/RF24/

// CHANGE TO YOUR WIFI CREDENTIALS
const char* ssid = "iotgyan";
const char* password = "iotgyan4u";


// CHANGE TO YOUR DEVICE CREDENTIALS AS PER IN IBM BLUMIX

#define ORG "kulwgd"
#define DEVICE_TYPE "nodemcu123"
#define DEVICE_ID "12345"
#define TOKEN "123456789"  //  Authentication Token OF THE DEVICE

//-------- Customise the below values --------
const char publishTopic[] = "iot-2/evt/kitchen/fmt/json";
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;


//Servo myservo;
WiFiClient wifiClient;
PubSubClient client(server, 1883, wifiClient);
//void publishData(int jar1, int jar2, int gas_value, int weight);
char text1[32]="";
//char text2[32]="";
RF24 radio(D4,D8);             /* Creating instance 'radio'  ( CE , CSN )   CE -> D7 | CSN -> D8 */                               
const byte Address[][6] = {"00009", "00008"}; /* Address from which data to be received */
int publishInterval = 5000; // 30 seconds
long lastPublishMillis;
unsigned int flowRate1;
unsigned int flowRate2;
 byte pipeNum = 0;
 byte newPacket = false;
void setup()
{

//  pinMode(gas, OUTPUT);
  //myservo.attach(D0);//pin attached to servo motor
  Serial.begin(9600);  //start serial to communication
  Serial.println();
  radio.begin();   
  /* Activate the modem*/
  radio.openReadingPipe(1, Address[0]);                // Start listening for messages
  radio.openReadingPipe(2, Address[1]);
  radio.startListening();
  wifiConnect();
  mqttConnect();
}

void loop()
{
  radio.startListening();          /*Setting modem in Receiver mode*/
if (radio.available(&pipeNum))
{
  newPacket = true;
while (radio.available())              /* Loop until receiving valid data*/
{
  if (pipeNum == 1) {
      
// flowRate1;    
// flowRate2; /* Variable to store received data */
radio.read(&flowRate1, sizeof(flowRate1));/* Read the received data and store in ' rx_data ' */
Serial.print("Received Data : ");
Serial.println(flowRate1);           /* Print received value on Serial Monitor */
  }
  if (pipeNum == 2) {
radio.read(&flowRate2, sizeof(flowRate2));
Serial.print("Received Data1 : ");
Serial.println(flowRate2);
//analogWrite(led_pin , rx_data);/* Write received value to PWM pin 3 to which LED is connected */
  }
}
}
  
else
{
Serial.println("Not Receiving !!!"); /* If not receiving valid data print " Not Receiving !!! " on Serial Monitor  */
  }
  
    if (millis() - lastPublishMillis > publishInterval)
  {
    publishData();
    lastPublishMillis = millis();
  }
  delay(1000);
  if (!client.loop()) {
    mqttConnect();
  }
}

void wifiConnect() {
  Serial.print("Connecting to "); Serial.print(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nWiFi connected, IP address: "); Serial.println(WiFi.localIP());
}

void mqttConnect() {
  if (!client.connected()) {
    Serial.print("Reconnecting MQTT client to "); Serial.println(server);
    while (!client.connect(clientId, authMethod, token)) {
      Serial.print(".");
      delay(500);
    }

    Serial.println();
  }
}

void publishData()
{

  //Generating payload for publishing the values
  String payload = "{\"d\":{\"flowRate\":";
  payload += flowRate1;
 payload += ",""\"flowRate2\":";
  payload +=  flowRate2;
 /* payload += ",""\"Gas_level\":";
  payload +=  "300";
  payload += ",""\"Weight\":";
  payload +=  "400";*/
  payload += "}}";

  Serial.print("\n");
  Serial.print("Sending payload: "); Serial.println(payload);

  if (client.publish(publishTopic, (char*) payload.c_str())) {
    Serial.println("Publish OK");
  } else {
    Serial.println("Publish FAILED");
  }
}
