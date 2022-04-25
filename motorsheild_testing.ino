

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
//#include <TinyMPU6050.h>
#include <MPU6050_tockn.h>

//******************************
// pin constants
//******************************

// the remaning numbers are the pins not in use:
// Digital pins:    [/, /, 2, /,/, /, 6, 7, /, /, 10, /, /, /]
// Analog pins:     [A0, A1, A2, A3, /, /]

// the motor sheild uses the following ports: {A0, A1, 3, 8, 9, 11, 12, 13}

// line sensor pins:
// #define lineSensorA A5
// #define lineSensorB A4

// sonar pins
// #define echoPin 4
// #define triggerPin 5

// Line Sensor
#define lineSens1 A2
#define lineSens2 A3

// channel A:
#define directionA 12 // HIGH = forward, LOW = reverse
#define speedA 3
#define brakeA 9
//#define currentSensingA A0

const int currentSensingA = A0;

// channel B
#define directionB 13 // HIGH = forward, LOW = reverse
#define speedB 10
#define brakeB 8
#define currentSensingB A1

float turningTarget = 0.0;
bool isTurning = false;
bool setTurn = false;
bool checkDegree = true;
// neopixels pins
#define neoPixel 9
#define PIXELCOUNT 5 // amount neopixels currently in use

// RGB led pins
//#define redPin 6
#define greenPin 5
//#define bluePin 10
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
byte gammatable[256];

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
// Adafruit_NeoPixel pixels = Adafruit_NeoPixel(PIXELCOUNT, neoPixel, NEO_GRB + NEO_KHZ800);

char host[] = "federicoshytte.dk";

WiFiClient client;

String postData;
String testPost;
String postVariable = "";

char* Network = "Ricosnet";
char* SSID = "rocket123";

void setupWifi()
{
  // Connecting to wifi
  Serial.println("Connecting to wifi: ");
  WiFi.begin(Network, SSID);
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
  // pixels.begin(); // neopixels

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

  // Setup Line Sensor
  // sonar sensor
  // pinMode(triggerPin, OUTPUT);
  // pinMode(echoPin, INPUT);
  irrecv.enableIRIn();
  for (int i = 0; i < 256; i++)
  {
    float x = i;
    x /= 255;
    x = pow(x, 2.5);
    x *= 255;

    gammatable[i] = x;

    // Serial.println(gammatable[i]);
  }

  // line Sensors
  // pinMode(lineSensorA, INPUT);
  // pinMode(lineSensorB, INPUT);

  // RGB led pins (to be determined, pins not assigned)
  // pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  // pinMode(bluePin, OUTPUT);
  analogWrite(greenPin, 0);
  // analogWrite(redPin, 255);
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
    motorDirection(forward, 255);
    break;
  case 0xFF18E7:
    Serial.println("2");
    turn(360.0);
    break;
  case 0xFF7A85:
    Serial.println("3");
    break;
  case 0xFF10EF:
    Serial.println("4");
    turn(270.0);
    break;
  case 0xFF38C7:
    Serial.println("5");
    motorDirection(halt, 0);
    break;
  case 0xFF5AA5:
    Serial.println("6");
    turn(90.0);
    break;
  case 0xFF42BD:
    Serial.println("7");
    motorDirection(reverse, 255);
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
  mpu.update();
  yaw = mpu.getAngleZ();
  if((millis() - lastTime) > 1000) {
    lastTime = millis();
    
    Serial.println(mpu.getAngleZ());

  }


    // delay(1000);
    // setColorLED();


  if (setTurn)
  {
    setTurningPoint(turningTarget, &yaw, 255);
  }
  if (isTurning)
  {
    isTurning = !reachedTarget(&yaw, turningTarget);
    if (!isTurning)
    {
      // motorDirection(halt, 0);
      //  Test LED from color sensor
      analogWrite(greenPin, 0);
      // analogWrite(redPin, 255);
    }
  }

  //readLineSensor();

  // if (buttonState == LOW && !pressed)
  // {
  //   turn(360.0);
  //   pressed = true;

  //   // stuff
  // }
  // else if (pressed && buttonState == HIGH)
  // {
  //   Serial.println("awdawd");
  //   pressed = false;
  // }

  if (irrecv.decode(&results) && !pressed) // have we received an IR signal?
  {
    Serial.println("test");
    translateIR();
    irrecv.resume(); // receive the next value
    remoteButtonDelay = millis() + 500;
    pressed = true;
  }
  if (millis() > remoteButtonDelay)
  {
    pressed = false;
  }

  // setColorNeopixel(100);
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
  int remapped = map(read1, 0, 1023, 0, 100);
  int read2 = analogRead(lineSens2);
  // Serial.print("Line Sensor 1: ");
  // Serial.println(remapped);
  // Serial.print("Line Sensor 2: ");
  // Serial.println(read2);
  if (remapped > 59)
  {
    digitalWrite(lineDetected, LOW);
  }
  else
  {
    digitalWrite(lineDetected, HIGH);
  }
}

// const double currentFactor = 2/3.3; // 2 Amps at 9 Volts: 0.22

//******************************
// Functions
//******************************

// probably needs to be revised as it currently is a not great with all the numbers and such.
// double getCurrentA(int motorCurrent){
//   const double currentFactor = 2/3.3; // 2 Amps at 3.3 Volts
//   double currentVoltage = map(analogRead(motorCurrent), 0, 1024, 0, 9000); // 9000 = 9V battery
//   double current = currentVoltage*currentFactor;
//   return current;
// }

// function for showing color in neopixels
void setColorNeopixel(int brightness)
{

  float red, green, blue;
  tcs.setInterrupt(false); // turn on LED             // takes 50ms to read
  tcs.getRGB(&red, &green, &blue);
  tcs.setInterrupt(true); // turn off LED

  // pixels.begin();
  // uint32_t color = pixels.Color(red, green, blue);
  // for (int i = 0; i < PIXELCOUNT; i++)
  // {
  //   pixels.fill(color);
  //   pixels.setBrightness(brightness);
  //   pixels.show();
  // }
}

// function for showing color on a normal RGB led
void setColorLED()
{

  // float red, green, blue;
  // tcs.getRGB(&red, &green, &blue);

  // Serial.print("R:\t");
  // Serial.print(int(red));
  // Serial.print("\tG:\t");
  // Serial.print(int(green));
  // Serial.print("\tB:\t");
  // Serial.print(int(blue));
  // Serial.print("\n");
  setupWifi();
  sendColor(200.f, 255.f, 255.f);
  // pins not assigned
  // analogWrite(redPin, gammatable[(int)red]);
  //analogWrite(greenPin, gammatable[(int)green]);
  // analogWrite(bluePin, gammatable[(int)blue]);
}

// Test functions

// rewrite function cause it sucks
// int readLineSensorA(){
//   qreValueA = analogRead(lineSensorA);
//   return qreValueA;
// }

// int readLineSensorB(){
//   qreValueB = analogRead(lineSensorB);
//   return qreValueB;
// }

// get distance for sonar sensor
// int getDistance()
// {

//   digitalWrite(triggerPin, LOW);
//   delayMicroseconds(2);
//   digitalWrite(triggerPin, HIGH);
//   delayMicroseconds(10);
//   digitalWrite(triggerPin, LOW);

//   duration = pulseIn(echoPin, HIGH);
//   distance = duration * 0.034 / 2;

//   return distance;
// }

void motorControls(int dir, bool dirValue, int brake, bool breakValue, int velocityName, int velocityValue)
{
  digitalWrite(dir, dirValue);
  digitalWrite(brake, breakValue);
  analogWrite(velocityName, velocityValue);
}

void turn(float target)
{
  turningTarget = target;
  setTurn = true;
}

void setTurningPoint(float turningPoint, float *currentDegrees, float speed)
{

  motorControls(directionA, calculateRotationDirection(turningPoint, currentDegrees), brakeA, false, speedA, speed);
  motorControls(directionB, calculateRotationDirection(turningPoint, currentDegrees), brakeB, false, speedB, speed);
  //analogWrite(greenPin, 255);
  // analogWrite(redPin, 0);
  isTurning = true;
  setTurn = false;
}

bool calculateRotationDirection(float turningPoint, float *currentDegrees)
{
  if (*currentDegrees > turningPoint)
  {
    int dif1 = *currentDegrees - turningPoint;
    int dif2 = 360.0 - *currentDegrees + turningPoint;
    if (dif1 > dif2)
    {
      return false;
    }
    else
    {
      return true;
    }
  }
  else if (*currentDegrees < turningPoint)
  {
    int dif1 = turningPoint - *currentDegrees;
    int dif2 = 360.0 - turningPoint + *currentDegrees;
    if (dif1 < dif2)
    {
      return false;
    }
    else
    {
      return true;
    }
  }
  else
  {
    return true;
  }
}

bool reachedTarget(float *degree, float target)
{
  float margin = 5;
  if (target == 360.0)
  {
    if ((*degree >= (target - margin)) || (*degree <= (target - (360 - margin))))
    {
      return true;
    }
  }
  else
  {
    if (*degree >= (target - margin) && (target + margin) >= *degree)
    {
      return true;
    }
  }
  return false;
}
// void turnDirection(control c) {
//   switch (c)
//   {
//     case north:

//   }
// }

void motorDirection(control c, int s)
{
  switch (c)
  {
  case forward:
    motorControls(directionA, true, brakeA, false, speedA, s);
    motorControls(directionB, false, brakeB, false, speedB, s);
    break;

  case reverse:
    motorControls(directionA, false, brakeA, false, speedA, s);
    motorControls(directionB, true, brakeB, false, speedB, s);
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
  postData = String(red) + ":" + String(green) + ":" + String(blue);
  // just for testing purpose
    Serial.println(WiFi.status());

  if (WiFi.status() == WL_CONNECTED)
  {
    if(client.connect(host, 80)) {
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
    }

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
