#include <Adafruit_NeoPixel.h>

#include <math.h>
#include <ac_LG.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiNINA.h>

#include "Adafruit_TCS34725.h"
#include <MPU6050_tockn.h>

// ================================================================
// ===                         Pins                             ===
// ================================================================

// Line Sensor
#define lineSens1 A2
#define lineSens2 A3
// channel A:
#define directionA 12 // HIGH = forward, LOW = reverse
#define speedA 3
#define brakeA 9
#define currentSensingA A0
// channel B
#define directionB 13 // HIGH = forward, LOW = reverse
#define speedB 10
#define brakeB 8
#define currentSensingB A1
#define neoPixel 5
#define wifiConnectionLED 2

// ================================================================
// ===                       Variabels                          ===
// ================================================================

#define PIXELCOUNT 1 // amount neopixels currently in use

// Sticker motor, -
#define motorspeed 150
float turningTarget = 0.0;

//States
bool isTurning = false;
bool setTurn = false;
bool checkDegree = true;
bool isOnColor = false;
bool hasTakenColor = false;
bool oppositeRotation = true;
bool lineSensed = false;

int hitWall = 0;
float yaw;


// direction control for motor movement
enum control
{
  forward,
  reverse,
  halt
};

// Calibration
#define lineDetected 4
int buttonState;
bool pressed;

// color sensor
uint16_t r, g, b, c, colorTemp, lux;

MPU6050 mpu(Wire);


Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(PIXELCOUNT, neoPixel, NEO_GRB + NEO_KHZ800);

char host[] = "federicoshytte.dk";

WiFiClient client;

String postData;
String testPost;
String postVariable = "";

char *Network = "NETWORK";
char *SSID = "PASSWORD";

volatile int numberOfColors = 0;
float red, green, blue;
float colors[3];
int x = 0;
int y = 0;
int xdirection = 0;
int ydirection = 0;
String directions[4] = {"north", "east", "west", "south"};
String currentDirection = "north";

// ================================================================
// ===                      Wifi Setup                          ===
// ================================================================
void setupWifi()
{
  // Connecting wifi
  Serial.println("Connecting to wifi: ");
  // wating for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    WiFi.begin(Network, SSID);
    delay(1000);
    Serial.print(". ");
  }
  Serial.println("");
  Serial.print("WiFi connected: "),
  Serial.print(WiFi.localIP()); // print the connected ip
}

// ================================================================
// ===                         Setup                            ===
// ================================================================
void setup()
{
  Serial.begin(9600);

  yaw = 0.0;

  Serial.println("=====================================");
  Serial.println("Starting calibration...");
  tcs.begin(); // color sensor
  mpu.begin();
  mpu.calcGyroOffsets(true);
  pixels.begin();

  // Button
  pinMode(lineDetected, OUTPUT);

  // Setup motor A
  pinMode(directionA, OUTPUT); // Initiates Motor Channel A pin
  pinMode(brakeA, OUTPUT);     // Initiates Brake Channel A pin
  pinMode(speedA, OUTPUT);

  // Setup motor B
  pinMode(directionB, OUTPUT); // Initiates Motor Channel B pin
  pinMode(brakeB, OUTPUT);     // Initiates Brake Channel B pin
  pinMode(speedB, OUTPUT);

  pinMode(wifiConnectionLED, OUTPUT);

  // line Sensors
  pinMode(lineSens1, INPUT);
  pinMode(lineSens2, INPUT);

  // RGB led pins (to be determined, pins not assigned)
  pinMode(neoPixel, OUTPUT);
}
// ================================================================
// ===                         Loop                             ===
// ================================================================
void loop()
{
  //gets current degrees from MPU
  mpu.update();
  yaw = mpu.getAngleZ();
  if (setTurn)
  {
    setTurningPoint(turningTarget, &yaw, motorspeed);
  }
  if (isTurning)
  {
    isTurning = !reachedTarget(&yaw, turningTarget);
    if (!isTurning)
    {
      motorDirection(halt, 0);
      if (hitWall > 0)
      {
        motorDirection(forward, motorspeed);
        hitWall--;
      }
    }
  }
  //Sets neopixel the current color
  uint32_t color = pixels.Color(red, green, blue);
  for (int i = 0; i < PIXELCOUNT; i++)
  {
    pixels.fill(color);
    pixels.setBrightness(100);
    pixels.show();
  }
  readLineSensor();
  getColor();
}

// ================================================================
// ===                         Functions                        ===
// ================================================================

void readLineSensor()
{
  int read1 = analogRead(lineSens1);
  int remapped1 = map(read1, 0, 1023, 0, 100);
  int read2 = analogRead(lineSens2);
  int remapped2 = map(read2, 0, 1023, 0, 100);
  if ((remapped1 > 59 || remapped2 > 59) && !lineSensed)
  {
    oppositeRotation = !oppositeRotation;
    motorDirection(halt, 0);
    motorDirection(reverse, motorspeed);
    hitWall = 2;
    lineSensed = true;
  }
  //If reached end of map
  if(hitWall > 0) {
    motorDirection(halt, 0);
    currentDirection = "stop";
    sendColor(red, green, blue);
  }
}

void setColorLED()
{
  delay(500);
  setupWifi();
  sendColor(red, green, blue);
}

void getColor()
{
  tcs.getRGB(&colors[0], &colors[1], &colors[2]);
  red = colors[0];
  green = colors[1];
  blue = colors[2];
  sortColorArray();
  if ((colors[0] - colors[2]) > 30)
  {
    isOnColor = true;
  }
  else
  {
    isOnColor = false;
    hasTakenColor = false;
  }
  if (isOnColor && !hasTakenColor)
  {
    motorDirection(halt, 0);
    if (hitWall != 2)
    {
      delay(100);
      setColorLED();
    }
    if (hitWall > 0)
    {
      oppositeRotation ? turn(90) : turn(-90);
    }
    if(hitWall == 0) {
      lineSensed = false;
    }
    motorDirection(forward, motorspeed);
    hasTakenColor = true;
  }
}

void sortColorArray()
{
  float temp = 0;
  if (colors[0] < colors[1])
  {
    temp = colors[0];
    colors[0] = colors[1];
    colors[1] = temp;
  }
  if (colors[0] < colors[2])
  {
    temp = colors[0];
    colors[0] = colors[2];
    colors[2] = temp;
  }
  if (colors[1] < colors[2])
  {
    temp = colors[1];
    colors[1] = colors[2];
    colors[2] = temp;
  }
}

// ==========================
// ===   Motor Controls   ===
// ==========================

void motorControls(int dir, bool dirValue, int brake, bool breakValue, int velocityName, int velocityValue)
{
  digitalWrite(dir, dirValue);
  digitalWrite(brake, breakValue);
  analogWrite(velocityName, velocityValue);
}

void motorDirection(control c, int s)
{
  int tolorance = 10; // one motor has more power than the other.

  switch (c)
  {
  case forward:
    motorControls(directionA, true, brakeA, false, speedA, s);
    motorControls(directionB, false, brakeB, false, speedB, s - tolorance);
    break;

  case reverse:
    motorControls(directionA, false, brakeA, false, speedA, s);
    motorControls(directionB, true, brakeB, false, speedB, s - tolorance);
    break;

  case halt:
    motorControls(directionA, false, brakeA, true, speedA, s);
    motorControls(directionB, false, brakeB, true, speedB, s);
    break;
  }
}

bool calculateRotationDirection(float turningPoint, float *currentDegrees)
{
  if (*currentDegrees > turningPoint)
  {
    return true;
  }
  else if (*currentDegrees < turningPoint)
  {
    return false;
  }
  else
  {
    return true;
  }
}

void turn(float target)
{
  turningTarget = yaw + target;
  setTurn = true;
  if (!oppositeRotation)
  {
    if (hitWall == 2)
    {
      currentDirection = directions[1];
    }
    else if (hitWall == 1)
    {
      currentDirection = directions[3];
    }
  }
  else
  {
    if (hitWall == 2)
    {
      currentDirection = directions[1];
    }
    else if (hitWall == 1)
    {
      currentDirection = directions[0];
    }
  }
}

void setTurningPoint(float turningPoint, float *currentDegrees, float speed)
{
  motorControls(directionA, calculateRotationDirection(turningPoint, currentDegrees), brakeA, false, speedA, speed);
  motorControls(directionB, calculateRotationDirection(turningPoint, currentDegrees), brakeB, false, speedB, speed);
  isTurning = true;
  setTurn = false;
}

bool reachedTarget(float *degree, float target)
{
  float degreeabs = fabs(*degree);
  float targetabs = fabs(target);
  float diff = fabs(targetabs - degreeabs);
  float margin = 7.5;
  if (margin > diff)
  {
    return true;
  }
  return false;
}

// ======================
// ===   HTTP calls   ===
// ======================

void sendColor(float red, float green, float blue)
{
  postData = String(red) + ":" + String(green) + ":" + String(blue) + ":" + currentDirection;
  // just for testing purpose
  String coords = "x " + String(x) + " : y " + String(y);
  Serial.println(coords);
  Serial.println(WiFi.status());
  digitalWrite(wifiConnectionLED, HIGH);
  if (WiFi.status() == WL_CONNECTED)
  {
    if (client.connect(host, 80))
    {
      Serial.println("connected to server");
      // Make a HTTP request:
      client.println("POST /api/colors HTTP/1.1");
      client.println("Host: " + String(host));
      client.println("Content-Type: text/plain");
      client.println("tester:"
                     "" +
                     postData);
      client.println("Connection: close");
      client.println();
      delay(1000);
      WiFi.disconnect();
      WiFi.end();
    }
    digitalWrite(wifiConnectionLED, LOW);
    checkDegree = true;
  }
}

