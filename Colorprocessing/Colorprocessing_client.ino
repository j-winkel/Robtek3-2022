#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiNINA.h>

#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include "wifi_pass.h"


// declaration
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
boolean status = false;
// constant variables
char host[] = "federicoshytte.dk";

WiFiClient client;

String postData;
String testPost;
String postVariable = "";

void setupWifi()
{
  // Connecting to wifi
  Serial.println("Connecting to wifi: ");
  WiFi.begin(SSID, PASSWORD);

  // wating for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    WiFi.begin(SSID, PASSWORD);

    delay(1000);
    Serial.print(". ");
  }
  Serial.println("");
  Serial.print("WiFi connected: "),
      Serial.print(WiFi.localIP()); // print the connected ip
}

void setup()
{
  Serial.begin(9600);
  setupWifi();
  if (tcs.begin())
  {
    Serial.println("Found sensor");
  }
  else
  {
    Serial.println("No TCS34725 found ... check your connections");
    while (1)
      ;
  }
}

void loop()
{

  float red, green, blue;
  tcs.setInterrupt(false); // turn on LED
  delay(60);               // takes 50ms to read
  tcs.getRGB(&red, &green, &blue);
  tcs.setInterrupt(true); // turn off LED

  postData = String(red) + ":" + String(green) + ":" + String(blue);
  testPost = "{test: 20}";
  // just for testing purpose
  Serial.println(postData);

  if(WiFi.status() == WL_CONNECTED) {
   
    if (client.connect(host, 80)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    client.println("POST /api/colors HTTP/1.1");
    client.println("Host: " + String(host));
    client.println("Content-Type: text/plain");
    client.println("tester:" "" + postData);
    client.println("Connection: close");
    client.println();
    delay(1000);
  }


  //   int httpCode = client.POST(String(jsonOutput));
  //   Serial.println(httpCode);
  //   if(httpCode > 0) {
  //     client.end();
  //   }

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
