//******************************
// libiaries
//******************************
#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include "WiFi.h"
#include <Adafruit_NeoPixel.h>
#include "digital_compass.h"

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

// channel A:
#define directionA 12 // HIGH = forward, LOW = reverse
#define speedA 3
#define brakeA 9
//#define currentSensingA A0

const int currentSensingA = A0;

// channel B
#define directionB 13 // HIGH = forward, LOW = reverse
#define speedB 11
#define brakeB 8
#define currentSensingB A1

// neopixels pins
#define neoPixel 9
#define PIXELCOUNT 5 // amount neopixels currently in use

// RGB led pins
#define redPin 6
#define greenPin 5
#define bluePin 10

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
#define button 4
int buttonState = 0;
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

//******************************
// Declaration type
//******************************

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_614MS, TCS34725_GAIN_1X);
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(PIXELCOUNT, neoPixel, NEO_GRB + NEO_KHZ800);

//******************************
// setup
//******************************
void setup()
{

  Serial.begin(115200);

  startMPU();
  tcs.begin();    // color sensor
  pixels.begin(); // neopixels

  // Button
  pinMode(button, INPUT_PULLUP);

  // Setup motor A
  pinMode(directionA, OUTPUT); // Initiates Motor Channel A pin
  pinMode(brakeA, OUTPUT);     // Initiates Brake Channel A pin
  pinMode(speedA, OUTPUT);

  // Setup motor B
  pinMode(directionB, OUTPUT); // Initiates Motor Channel B pin
  pinMode(brakeB, OUTPUT);     // Initiates Brake Channel B pin
  pinMode(speedB, OUTPUT);

  // sonar sensor
  // pinMode(triggerPin, OUTPUT);
  // pinMode(echoPin, INPUT);

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
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
}

//******************************
// loop
//******************************

void loop()
{
  int speed = 255 * 0.5;
  float yaw, pitch, roll;

  getRotationContinuous(&yaw, &pitch, &roll);



  {
    motorDirection(halt, 255);
  }
  buttonState = digitalRead(button);

  if (millis() % 200 == 0)
  {
    setColorLED();
  }

  if (buttonState == LOW && !pressed)
  {
    Serial.println("HAOIDAJIWODJIAWODA FUCK");
    pressed = true;

    // stuff
  }
  else if (pressed && buttonState == HIGH)
  {
    pressed = false;
  }
  // setColorNeopixel(100);
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

  pixels.begin();
  uint32_t color = pixels.Color(red, green, blue);
  for (int i = 0; i < PIXELCOUNT; i++)
  {
    pixels.fill(color);
    pixels.setBrightness(brightness);
    pixels.show();
  }
}

// function for showing color on a normal RGB led
void setColorLED()
{

  float red, green, blue;
  tcs.setInterrupt(false); // turn on LED             // takes 50ms to read
  tcs.getRGB(&red, &green, &blue);
  tcs.setInterrupt(true); // turn off LED

  Serial.print("R:\t");
  Serial.print(int(red));
  Serial.print("\tG:\t");
  Serial.print(int(green));
  Serial.print("\tB:\t");
  Serial.print(int(blue));
  Serial.print("\n");

  // pins not assigned
  analogWrite(redPin, gammatable[(int)red]);
  analogWrite(greenPin, gammatable[(int)green]);
  analogWrite(bluePin, gammatable[(int)blue]);
}

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
    motorControls(directionA, true, brakeA, false, speedA, s);
    motorControls(directionB, true, brakeB, false, speedB, s);
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
