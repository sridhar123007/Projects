#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

const byte addresses[][6] = {"00001", "00002","00003"};
String data3;

String command;

RF24 radio(D4, D8); // Set up nRF24L01 radio on SPI bus plus pins 7 & 8
 
//-------- Customise these values -----------
const char* ssid = "Sridhar007";
const char* password = "Sridhar07@";

// CHANGE TO YOUR DEVICE CREDENTIALS AS PER IN IBM BLUMIX
#define ORG "zv2k2t"
#define DEVICE_TYPE "hello"
#define DEVICE_ID "1234"
#define TOKEN "12345678"
//-------- Customise the above values --------
 
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char topic[] = "iot-2/evt/Data/fmt/json";
char subtopic[] = "iot-2/cmd/home/fmt/String";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;

int ldr = A0;
int val,val1;

struct dataStruct {
  int volt;
} data;
int publishInterval = 5000; // 30 seconds
long lastPublishMillis;
void PublishData();
void callback(char* topic, byte* payload, unsigned int payloadLength);
WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);

void setup() {
 Serial.begin(9600);
 Serial.println();
 Serial.print("Sending sensor values ...\n\r");
 restartRadio();
 wifiConnect();
 mqttConnect();
}
 
void loop() {
  if (millis() - lastPublishMillis > publishInterval)
  {
    PublishData();
    lastPublishMillis = millis();
  }

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
  Serial.print("WiFi connected, IP address: ");
  Serial.println(WiFi.localIP());
}

void mqttConnect() {
  if (!client.connected())
  {
    Serial.print("Reconnecting MQTT client to "); Serial.println(server);
    while (!client.connect(clientId, authMethod, token)) {
      Serial.print(".");
      delay(500);
    }

    initManagedDevice();
    Serial.println();
  }
}


void PublishData(){
delay(1000);
  val1 = analogRead(ldr);
  val = map(val1,0,1023,0,255);
  Serial.print("LDR VALUE IS " );
  Serial.println(val1);
  Serial.print("AFTER MAPPING LDR VALUE IS " );
  Serial.println(val);
 if(val>0) 
{
  Serial.println(".......................send........");
  data.volt = val;
  restartRadio();                    // turn on and configure radio
  printf("Now sending %d byte payload... ", sizeof(data));
  radio.write(&data , sizeof(data) );
}
else 
{
  Serial.println(".......................send........");
  data.volt =0;
  val=0;
  restartRadio();                    // turn on and configure radio
  printf("Now sending %d byte payload... ", sizeof(data));
  radio.write(&data , sizeof(data) );
}
 
  String payload = "{\"d\":{\"ldr\":";
  payload += val;
  //payload+="," "\"humidity\":";
  //payload += humid;
  payload += "}}";
 Serial.print("Sending payload: ");
 Serial.println(payload);
  
 if (client.publish(topic, (char*) payload.c_str())) {
 Serial.println("Publish ok");
 } else {
 Serial.println("Publish failed");
 }
}

void initManagedDevice() {
  if (client.subscribe(subtopic)) {
    Serial.println("subscribe to cmd OK");
  } else {
    Serial.println("subscribe to cmd FAILED");
  }
}

void restartRadio()
{

  radio.begin(); // Start up the radio
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(addresses[1]);
  radio.stopListening();
  
}

void callback(char* subtopic, byte* payload, unsigned int payloadLength) {
  Serial.print("callback invoked for topic: ");
  Serial.println(subtopic);
  for (int i = 0; i < payloadLength; i++) {
    command += (char)payload[i];
  }
  Serial.print("data: " + command);
  char cmd[10];
  command.toCharArray(cmd, 10);
  restartRadio();
  radio.write( &cmd, sizeof(cmd));
  Serial.print("DONE");
  //radio.stopListening(); //go back to transmit mode
  delay(1000);
  command = "";
}
