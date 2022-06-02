#include <Adafruit_NeoPixel.h>

#include <math.h>
#include <ac_LG.h>
#include <digitalWriteFast.h>
#include <IRProtocol.h>
#include <IRremote.h>
#include <IRremoteInt.h>
#include <LongUnion.h>
#include <TinyIRReceiver.h>

//******************************
// libiaries
//******************************
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiNINA.h>

#include "Adafruit_TCS34725.h"
#include <MPU6050_tockn.h>

//******************************
// pin constants
//******************************

// the remaning numbers are the pins not in use:
// Digital pins:    [/, /, 2, /,/, /, 6, 7, /, /, 10, /, /, /]
// Analog pins:     [A0, A1, A2, A3, /, /]

// the motor sheild uses the following ports: {A0, A1, 3, 8, 9, 11, 12, 13}

// line sensor pins:
#define lineSensorA A2
#define lineSensorB A3

// Line Sensor
#define lineSens1 A2
#define lineSens2 A3

// channel A:
#define directionA 12 // HIGH = forward, LOW = reverse
#define speedA 3
#define brakeA 9
//#define currentSensingA A0

// Stickerless motor, white to ground
const int currentSensingA = A0;

// channel B
#define directionB 13 // HIGH = forward, LOW = reverse
#define speedB 10
#define brakeB 8
#define currentSensingB A1

// Sticker motor, -
#define motorspeed 150
float turningTarget = 0.0;
bool isTurning = false;
bool setTurn = false;
bool checkDegree = true;
bool isOnColor = false;
bool hasTakenColor = false;
bool oppositeRotation = true;
bool lineSensed = false;
int hitWall = 0;
#define neoPixel 5
#define PIXELCOUNT 1 // amount neopixels currently in use
#define wifiConnectionLED 2

float yaw;

int receiver = 6; // Signal Pin of IR receiver to Arduino Digital Pin 11

/*-----( Declare objects )-----*/
IRrecv irrecv(receiver); // create instance of 'irrecv'
decode_results results;  // create instance of 'decode_results'
long remoteButtonDelay;

/*-----( Function )-----*/
// takes action based on IR code received

// describing Remote IR codes

//******************************
// variables
//******************************

// direction control for motor movement
enum control
{
  forward,
  reverse,
  left,
  right,
  halt
};

// Calibration
#define lineDetected 4
int buttonState;
bool pressed;

// color sensor
uint16_t r, g, b, c, colorTemp, lux;

// line sensor
int qreValueA;
int qreValueB;

// sonar sensor
int duration;
int distance;
MPU6050 mpu(Wire);
//******************************
// Declaration type
//******************************

long lastTime = 0;

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(PIXELCOUNT, neoPixel, NEO_GRB + NEO_KHZ800);

char host[] = "federicoshytte.dk";

WiFiClient client;

String postData;
String testPost;
String postVariable = "";

char *Network = "Hyggebulen-2G";
char *SSID = "L3C4PPCGX4";

volatile int numberOfColors = 0;
float red, green, blue;
float colors[3];
int x = 0;
int y = 0;
int xdirection = 0;
int ydirection = 0;
String directions[4] = {"north", "east", "west", "south"};
String currentDirection = "north";

void setupWifi()
{
  // Connecting to wifi
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
  irrecv.enableIRIn();

  // line Sensors
  pinMode(lineSensorA, INPUT);
  pinMode(lineSensorB, INPUT);

  // RGB led pins (to be determined, pins not assigned)
  pinMode(neoPixel, OUTPUT);
}

//******************************
// loop
//******************************
void translateIR() // takes action based on IR code received
{
  switch (results.value)
  {
  case 0xFFA25D:
    setColorLED();
    break;
  case 0xFFE21D:
    Serial.println("FUNC/STOP");
    break;
  case 0xFF629D:
    Serial.println("VOL+");
    break;
  case 0xFF22DD:
    Serial.println("FAST BACK");
    break;
  case 0xFF02FD:
    Serial.println("PAUSE");
    break;
  case 0xFFC23D:
    Serial.println("FAST FORWARD");
    break;
  case 0xFFE01F:
    Serial.println("DOWN");
    break;
  case 0xFFA857:
    Serial.println("VOL-");
    break;
  case 0xFF906F:
    Serial.println("UP");
    break;
  case 0xFF9867:
    Serial.println("EQ");
    break;
  case 0xFFB04F:
    Serial.println("ST/REPT");
    break;
  case 0xFF6897:
    Serial.println("0");
    break;
  case 0xFF30CF:
    Serial.println("1");
    motorDirection(forward, motorspeed);
    break;
  case 0xFF18E7:
    Serial.println("2");
    turn(-180.0);
    break;
  case 0xFF7A85:
    Serial.println("3");
    break;
  case 0xFF10EF:
    Serial.println("4");
    turn(90.0);
    break;
  case 0xFF38C7:
    Serial.println("5");
    motorDirection(halt, 0);
    break;
  case 0xFF5AA5:
    Serial.println("6");
    turn(-90.0);
    break;
  case 0xFF42BD:
    Serial.println("7");
    motorDirection(reverse, motorspeed);
    break;
  case 0xFF4AB5:
    Serial.println("8");
    turn(180.0);
    break;
  case 0xFF52AD:
    Serial.println("9");
    break;
  case 0xFFFFFFFF:
    Serial.println(" REPEAT");
    break;

  default:
    Serial.println(" other button   ");

  } // End Case
}
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

int freeRam()
{
  extern int __heap_start, *__brkval;
  int v;
  return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}

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
}

//******************************
// Functions
//******************************

// function for showing color on a normal RGB led
void setColorLED()
{
  delay(1000);
  Serial.print("R:\t");
  Serial.print(int(red));
  Serial.print("\tG:\t");
  Serial.print(int(green));
  Serial.print("\tB:\t");
  Serial.print(int(blue));
  Serial.print("\n");

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

void motorControls(int dir, bool dirValue, int brake, bool breakValue, int velocityName, int velocityValue)
{
  digitalWrite(dir, dirValue);
  digitalWrite(brake, breakValue);
  analogWrite(velocityName, velocityValue);
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

void motorDirection(control c, int s)
{
  switch (c)
  {
  case forward:
    motorControls(directionA, true, brakeA, false, speedA, s);
    motorControls(directionB, false, brakeB, false, speedB, s - 10);
    break;

  case reverse:
    motorControls(directionA, false, brakeA, false, speedA, s);
    motorControls(directionB, true, brakeB, false, speedB, s - 10);
    break;

  case right:

    break;

  case left:
    motorControls(directionA, false, brakeA, false, speedA, s);
    motorControls(directionB, false, brakeB, false, speedB, s);
    break;

  case halt:
    motorControls(directionA, false, brakeA, true, speedA, s);
    motorControls(directionB, false, brakeB, true, speedB, s);
    break;
  }
}

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
// robtek opgave formulering

/*
--- opgave formulering ---

* En bane udformes med sorte kanter således at robotten kan læse og se kanterne

* banen består af en grid med forskellige farver hvorfra disse farves position skal rapporteres tilbage.

* tidligere er der blevet brugt digital compas, og det er en mulighed at benytte dette

* der kan benyttes encoder eller tacometer til at bestelle motor rotation.

* line sensor kan erstattes af farve sensor.

* iot består af at melde kordinater tilbage til pc, og evt. starte robotten med et signal fra pc.

* robottens bevægelse skal være præsis nok til at, dens position kan bestemmes, ud fra et start punkt.

* 3d print er nice to have og ikke need to have. (men det er selvfølgelig bedre at have det, men det behøver ikke at være der.)

---- gode idere ---

* kan regulrererobottens position med line sonsore der sidder i parallel.

* ser ikke ud som om der skal benyttes obsicale avidnce alligevelle.

* kan evt benytte breath first search for at finde alle tiles i en grid.

*/
