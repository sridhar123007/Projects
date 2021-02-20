#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h> //Call SPI library so you can communicate with the nRF24L01+
#include <nRF24L01.h> //nRF2401 libarary found at https://github.com/tmrh20/RF24/
#include <RF24.h> //nRF2401 libarary found at https://github.com/tmrh20/RF24/

const int pinCE = D4; //This pin is used to set the nRF24 to standby (0) or active mode (1)
const int pinCSN = D8; //This pin is used to tell the nRF24 whether the SPI communication is a command or message to send out
String command;
String data = "";
void callback(char* topic, byte* payload, unsigned int payloadLength);
RF24 wirelessSPI(pinCE, pinCSN); // Create your nRF24 object or wireless SPI connection
//const uint64_t wAddress = 0xB00B1E50D2LL;              // Pipe to write or transmit on
const byte address[6] = "00001";

// CHANGE TO YOUR WIFI CREDENTIALS
const char* ssid = "Sridhar007";
const char* password = "Sridhar07@";

// CHANGE TO YOUR DEVICE CREDENTIALS AS PER IN IBM BLUMIX
#define ORG "zv2k2t"
#define DEVICE_TYPE "hello"
#define DEVICE_ID "1234"
#define TOKEN "12345678" //  Authentication Token OF THE DEVICE

//  PIN DECLARATIONS
#define led1 D0
const int ldrPin = A0;
//#define led2 D1
//-------- Customise the above values --------
const char publishTopic[] = "iot-2/evt/Data/fmt/json";
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char topic[] = "iot-2/cmd/home/fmt/String";// cmd  REPRESENT command type AND COMMAND IS TEST OF FORMAT STRING
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;
WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);
int publishInterval = 5000; // 30 seconds
long lastPublishMillis;
void publishData();
int ldrStatus;
void setup() {
  Serial.begin(115200);
  pinMode(ldrPin, INPUT);
  Serial.println();
  pinMode(led1, OUTPUT);
  //pinMode(led2,OUTPUT);
  //dht.begin();
  wirelessSPI.begin();            //Start the nRF24 module
  wirelessSPI.openWritingPipe(address);        //open writing or transmit pipe
  wirelessSPI.stopListening(); //go into transmit mode
  wifiConnect();
  mqttConnect();
}

void loop() {
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

void initManagedDevice() {
  if (client.subscribe(topic)) {
    Serial.println("subscribe to cmd OK");
  } else {
    Serial.println("subscribe to cmd FAILED");
  }
}

void callback(char* topic, byte* payload, unsigned int payloadLength) {
  Serial.print("callback invoked for topic: ");
  Serial.println(topic);
  for (int i = 0; i < payloadLength; i++) {
    command += (char)payload[i];
  }
  Serial.print("data: " + command);
  control_func();
  char cmd[10];
  command.toCharArray(cmd, 10);
  wirelessSPI.write( &cmd, sizeof(cmd));
  Serial.print("DONE");
  //wirelessSPI.stopListening(); //go back to transmit mode
  delay(1000);
  command = "";
}

void control_func()
{
  char cmd[10];
  if (command == "lightoff" || ldrStatus > 700)
  {
    digitalWrite(led1, LOW);
    //digitalWrite(led2,LOW);
    String command="lightoff";
    command.toCharArray(cmd, 10);
    wirelessSPI.write( &cmd, sizeof(cmd));
    Serial.print("DONE");
    //wirelessSPI.stopListening(); //go back to transmit mode
    delay(1000);
    Serial.println(".......lights are off..........");
    command = "";
  }
  else if (command == "lighton" || ldrStatus < 700)
  {
    digitalWrite(led1, HIGH);
    String command="lighton";
    command.toCharArray(cmd, 10);
    wirelessSPI.write( &cmd, sizeof(cmd));
    Serial.print("DONE");
    //wirelessSPI.stopListening(); //go back to transmit mode
    delay(1000);
    //digitalWrite(led2,HIGH);
    Serial.println(".......lights are on..........");
    command = "";
  }
  else
  {
    Serial.println("......no commands have been subscribed..........");
  }
  
}

void publishData()
{
  // humidity = dht.readHumidity();
  //temperature = dht.readTemperature();
  //if (isnan(humidity) || isnan(temperature)) {
  // Serial.println("Failed to read from DHT sensor!");
  //return;
  //}
  ldrStatus = analogRead(ldrPin);
  //String astr(a);
  //ldr = String(ldrStatus);
  Serial.print("LDR VALUE IS: ");
  Serial.println(ldrStatus);
  control_func();
  String payload = "{\"d\":{\"ldr\":";
  payload += ldrStatus;
  //payload += ",""\"humidity\":";
  //payload +=  humidity;
  payload += "}}";
  Serial.print("\n");
  Serial.print("Sending payload: "); Serial.println(payload);
  if (client.publish(publishTopic, (char*) payload.c_str())) {
    Serial.println("Publish OK");
  } else {
    Serial.println("Publish FAILED");
  }

}
