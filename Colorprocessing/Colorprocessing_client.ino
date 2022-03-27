#include <WiFi.h>
#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include "wifi_pass.h"

// declaration
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

// constant variables
const int port = 8080;
const char* host ="192.168.0.29";

WiFiClient client;

String postData;
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

    postData = String(red) + " " + String(green) + " " + String(red);
    // just for testing purpose
    Serial.println(postData);

  if (client.connect(host,8080)){
    client.println("POST / HTTP/1.1");
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(postData.length());
    client.println();
    client.print(postData);
    delay(1000);
  }

  if (client.connected()){
    client.stop();
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

