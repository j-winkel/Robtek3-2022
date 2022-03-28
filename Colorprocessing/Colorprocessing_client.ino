#include <WiFi.h>
#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include "wifi_pass.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>

// declaration
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

// constant variables
const int port = 8080;
const char* host ="192.168.0.119";

WiFiClient client;

String postData;
String testPost;
String postVariable = "";


void setupWifi(){
  // Connecting to wifi
  Serial.println("Connecting to wifi: ");
  WiFi.begin(SSID, PASSWORD);
  // wating for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(". ");
  }
  Serial.println("");
  Serial.print("WiFi connected: "),
  Serial.print(WiFi.localIP()); // print the connected ip
}

void setup() {
  Serial.begin(9600);
  tcs.begin();
  setupWifi();
}

void loop(){

    float red, green, blue;
    tcs.setInterrupt(false);  // turn on LED
    delay(60);  // takes 50ms to read
    tcs.getRGB(&red, &green, &blue);
    tcs.setInterrupt(true);  // turn off LED

    postData = String(red) + " " + String(green) + " " + String(blue);
    testPost = "{test: 20}";
    // just for testing purpose
    Serial.println(postData);

  if(WiFi.status() == WL_CONNECTED) {
    char jsonOutput[256];
    HTTPClient client;
    client.begin("http://192.168.0.119:3000/test");
    client.addHeader("Content-Type", "application/json");
    
    //Sets up the JSON file
    const size_t CAPACITY = JSON_OBJECT_SIZE(6);
    StaticJsonDocument<CAPACITY> doc;

    JsonObject object = doc.to<JsonObject>();
    object["red"] =  String(red);
    object["green"] = String(green);
    object["blue"] = String(blue);
    
    //Serializes it
    serializeJson(doc, jsonOutput);


    int httpCode = client.POST(String(jsonOutput));
    Serial.println(httpCode);
    if(httpCode > 0) {
      client.end();
    }

  }

  
}


// // refrence function not in use
// void getColor(){

//   float red, green, blue;
  
//   tcs.setInterrupt(false);  // turn on LED
//   delay(60);  // takes 50ms to read
//   tcs.getRGB(&red, &green, &blue);
//   tcs.setInterrupt(true);  // turn off LED

//   Serial.print("R:\t"); Serial.print(int(red)); 
//   Serial.print("\tG:\t"); Serial.print(int(green)); 
//   Serial.print("\tB:\t"); Serial.print(int(blue));
//   Serial.print("\n");
// }
