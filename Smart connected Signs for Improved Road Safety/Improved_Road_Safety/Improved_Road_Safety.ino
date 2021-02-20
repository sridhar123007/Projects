  #include <ESP8266WiFi.h> //Esp8266 wifi library
  #include <SPI.h>
  #include <Wire.h>
  #include <Adafruit_GFX.h>
  #include <Adafruit_SSD1306.h>
  
  const char* ssid     = "SSID";
  const char* password = "PASSWORD";
  
  const char* host = "website url";
  const char* writeAPIKey = "api key";

  const int httpPort = 80;
  
  // If using software SPI (the default case):
  #define OLED_MOSI   D7 //D1 in OLED
  #define OLED_CLK   D5 //D0 in OLED
  #define OLED_DC    D2 //DC in OLED
  #define OLED_CS    D8 //D8 in OLED
  #define OLED_RESET D1 //RST in OLED
  Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
  
  WiFiClient client;
  
  const int button1Pin = D0;     // the number of the pushbutton pin
  const int button2Pin = D3;      // the number of the pushbutton pin
  const int button3Pin = D4;     // the number of the pushbutton pin
 
  
  const int led = D9;      // the number of the LED pin
  
  // variables will change:
  int button1State = 0;
  int button2State = 0;
  int button3State = 0;  // variable for reading the pushbutton status
  String url,oled;
  
  void setup(){ 
  //Connect to Wifi
  
  Serial.begin(9600);
  Serial.println("");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3D);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Welcome");
  display.display();
  delay(3000);
  display.clearDisplay();

  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  // initialize the pushbutton pin as an input:
  pinMode(button1Pin, INPUT);
  pinMode(button2Pin, INPUT);
  pinMode(button3Pin, INPUT);
   
  }
  
  void loop() {
    
  delay(5000);
  // read the state of the pushbutton value:
  button1State = digitalRead(button1Pin);
  button2State = digitalRead(button2Pin);
  button3State = digitalRead(button3Pin);
      
   if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  } 
  
  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (button1State == HIGH) {
  // Button 1 is pressed:
  Serial.println("Button 1 is pressed");
  delay(1000);
  
  url = "your web url here"+"1";
  url+=writeAPIKey;
  Serial.println(url);
  Serial.println("doctor ");
  notification();
   
  }
  
  else if (button2State == HIGH) {
  // Button 2 is pressed:
  Serial.println("Button 2 is pressed");
  delay(1000);
  
  url = "your web url here"+"2";
  url+=writeAPIKey;
  Serial.println(url);
  Serial.println("pharmacy ");
  notification();
    }
  
  else if (button3State == HIGH) {
  // Button 3 is pressed:
  Serial.println("Button 3 is pressed");
  delay(1000);
  
  url = "your web url here"+"3";
  url+=writeAPIKey;
  Serial.println(url);
  Serial.println(" stationary ");
  notification();
   }
  
 
  else if (button2State == HIGH && button1State == HIGH && button3State == HIGH){
  Serial.println("status high"); 
  digitalWrite(led, LOW);
  delay(1000);
  }
  else{
  digitalWrite(led, LOW);
  Serial.println("Error with connection");
  } 
  }
  
 void notification()
  {
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
    
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
               
     unsigned long timeout = millis();
    while (client.available() == 0) {
    if (millis() - timeout > 1000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
    }
  // Read all the lines of the reply from server and print them to Serial
  while(client.find("@@")){
    oled = client.readStringUntil('\r');
    Serial.print(oled);
  }
  
  Serial.println();
  Serial.println("closing connection");
  
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println(oled);
  display.display();
  delay(3000);
  display.clearDisplay();
  url="";  
  }
  
 
