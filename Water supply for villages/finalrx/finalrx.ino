
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h> //Call SPI library so you can communicate with the nRF24L01+
#include <nRF24L01.h> //nRF2401 libarary found at https://github.com/tmrh20/RF24/
#include <RF24.h> //nRF2401 libarary found at https://github.com/tmrh20/RF24/

// CHANGE TO YOUR WIFI CREDENTIALS
const char* ssid = "Realme";
const char* password = "password";


// CHANGE TO YOUR DEVICE CREDENTIALS AS PER IN IBM BLUMIX

#define ORG "raisa9"
#define DEVICE_TYPE "atri"
#define DEVICE_ID "Anusha12345"
#define TOKEN "@*7AT3DgX&r+Cd&LMA"  //  Authentication Token OF THE DEVICE

//-------- Customise the below values --------
const char publishTopic[] = "iot-2/evt/Data/fmt/json";
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;
WiFiClient wifiClient;
PubSubClient client(server, 1883, wifiClient);
//void publishData(int jar1, int jar2, int gas_value, int weight);
int id;
const int pinCE = D4; //This pin is used to set the nRF24 to standby (0) or active mode (1)
const int pinCSN = D8; //This pin is used to tell the nRF24 whether the SPI communication is a command or message to send out
#define gas A0
int publishInterval = 5000; // 30 seconds
long lastPublishMillis;
byte pipeNum = 0;
byte newPacket = false;
char text[150] = "";
char text1[150] = "";
RF24 wirelessSPI(pinCE, pinCSN); // Declare object from nRF24 library (Create your wireless SPI)
const uint64_t rAddress[] = {0xB00B1E50D5LL, 0xB00B1E50C5LL};  //Create pipe addresses for the 2 nodes to recieve data, the "LL" is for LongLong type

void setup()
{
  Serial.begin(9600);  //start serial to communication
  Serial.println();
  wirelessSPI.begin();  //Start the nRF24 module
  wirelessSPI.openReadingPipe(1, rAddress[0]);     //open pipe o for recieving meassages with pipe address
  wirelessSPI.openReadingPipe(2, rAddress[1]);     //open pipe o for recieving meassages with pipe address
  wirelessSPI.startListening();                 // Start listening for messages
  wifiConnect();
  mqttConnect();
}

void loop()
{
  

  if (wirelessSPI.available(&pipeNum)) {
    newPacket = true;
    if (pipeNum == 1) {
      
      wirelessSPI.read(&text, sizeof(text));
      id=1;
      Serial.println("-----------------");
          
    Serial.println(" node id");
    Serial.println(id);
      Serial.println("text");
      Serial.println(text);
      Serial.println("-----------------");
    
    }
    if (pipeNum == 2) {
     
      wirelessSPI.read(&text1,sizeof(text1));
      id=2;
      Serial.println("-----------------");
          
    Serial.println(" node id");
    Serial.println(id);
      Serial.println("text1");
      Serial.println(text1);
      Serial.println("-----------------");
    }
    
  }

//  if (newPacket) {
//    Serial.print(text);
//    Serial.print(F(", "));
//    Serial.println(text1);
//  }
     
  
  delay(1000);
  if (millis() - lastPublishMillis > publishInterval)
  {
    publishData();
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
 String payload = "{\"d\":{\"id\":";
  payload +=id;
    payload += ",";
  payload +=text;
  payload += "}}";
  Serial.print("\n");
  Serial.print("Sending payload: "); Serial.println(payload);

  if (client.publish(publishTopic, (char*) payload.c_str())) {
    Serial.println("Publish OK");
  } else {
    Serial.println("Publish FAILED");
  }
}
