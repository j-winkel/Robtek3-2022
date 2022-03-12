//******************************
// libiaries
//******************************
#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include "WiFi.h"

//******************************
// pin constants
//******************************

// the remaning numbers are the pins not in use:
// Digital pins:    [/, /, 2, /,/, /, 6, 7, /, /, 10, /, /, /]
// Analog pins:     [A0, A1, A2, A3, /, /]

"
the motor sheild uses the following ports: {A0, A1, 3, 8, 9, 11, 12, 13}
"

  // line sensor pins:
  #define lineSensorA A5
  #define lineSensorB A4

  // sonar pins
  #define echoPin 4
  #define triggerPin 5

  // channel A:
  int directionA = 12; // HIGH = forward, LOW = reverse
  int speedA = 3; 
  int brakeA = 9;
  int currentSensingA = 0; // not in use

  // channel B
  int directionB = 13; // HIGH = forward, LOW = reverse
  int speedB = 11;
  int brakeB = 8;
  int currentSensingB = 1; // not in use

//******************************
// variables
//****************************** 

// directio control for motor movement
  enum control{
    forward,
    reverse,
    left,
    right,
    halt
  };

// color sensor
  uint16_t r, g, b, c, colorTemp, lux;

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

//******************************
// setup
//******************************
void setup() {

  Serial.begin(9600);
  
  //Setup motor A
  pinMode(directionA, OUTPUT); //Initiates Motor Channel A pin
  pinMode(brakeA, OUTPUT); //Initiates Brake Channel A pin
  pinMode(speedA, OUTPUT); 

  //Setup motor B
  pinMode(directionB, OUTPUT); //Initiates Motor Channel B pin
  pinMode(brakeB, OUTPUT);  //Initiates Brake Channel B pin
  pinMode(speedB, OUTPUT); 

  // sonar sensor
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // line Sensors
  pinMode(lineSensorA, INPUT);
  pinMode(lineSensorB, INPUT);
}

//******************************
// loop
//******************************
void loop(){

//  motorDirection(forward);
//  delay(1000);
//  motorDirection(halt);
//  delay(1000);
//  motorDirection(reverse);
//  delay(1000);
//  motorDirection(halt);
//  delay(1000); 


// only for initial testing
  readLineSensorA();
  Serial.println(qreValue);

  if(qreValue > 100){
    motorDirection(forward);
  } else {
    motorDirection(reverse);
  }
}

//******************************
// Functions
//******************************

tuple getColorsensor(){

  tcs.getRawData(&r, &g, &b, &c);

  rgbValue = tcs.getRGB(r,g,b);
  colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
  lux = tcs.calculateLux(r, g, b);

  return {rgbValue, colorTemp, lux};
}

// rewrite function cause it sucks 
int readLineSensorA(){
  qreValueA = analogRead(lineSensorA);
  return qreValueA;
}

int readLineSensorB(){
  qreValueB = analogRead(lineSensorB);
  return qreValueB;
}

// get distance for ultrasound sensor
int getDistance(){

  digitalWrite(triggerPin,LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin,HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  return distance;
}

void motorControls(int dir, bool dirValue, int brake, bool breakValue, int velocityName, int velocityValue){
  digitalWrite(dir,dirValue);
  digitalWrite(brake,breakValue);
  digitalWrite(velocityName,velocityValue);
}

void motorDirection(control c){
  switch(c){
    case forward:
      motorControls(directionA, true, brakeA, false, speedA, 255);
      motorControls(directionB, true, brakeB, false, speedB, 255);
      break;

    case reverse:
      motorControls(directionA, false, brakeA, false, speedA, 255);
      motorControls(directionB, false, brakeB, false, speedB, 255);
      break;

     case left:
      motorControls(directionA, true, brakeA, true, speedA, 0);
      motorControls(directionB, true, brakeB, true, speedB, 0);
      break;

    case right:
      motorControls(directionA, true, brakeA, true, speedA, 255);
      motorControls(directionB, false, brakeB, true, speedB, 255);
      break;

    case halt:
      motorControls(directionA, false, brakeA, true, speedA, 255);
      motorControls(directionB, true, brakeB, true, speedB, 255);
      break;
  }
}

// robtek opgave formulering
"
--- opgave formulering ---

* En bane udformes med sorte kanter således at robotten kan læse og se kanterne

* banen består af en grid med forskellige farver hvorfra disse farves position skal rapporteres tilbage. 

* tidligere er der blevet brugt digital compas, og det er en mulighed at benytte dette 

* der kan benyttes encoder eller tacometer til at bestelle motor rotation. 

* line sensor kan erstattes af farve sensor. 

* iot består af at melde kordinater tilbage til pc, og evt. starte robotten med et signal fra pc. 

* robottens bevægelse skal være præsis nok til at, dens position kan bestemmes, ud fra et start punkt. 

* print er nice to have og ikke need to have. (men det er selvfølgelig bedre at have det, men det behøver ikke at være der.) 

---- gode idere ---

* kan regulrererobottens position med line sonsore der sidder i parallel. 

* ser ikke ud som om der skal benyttes obsicale avidnce alligevelle. 

* kan evt benytte breath first search for at finde alle tiles i en grid. 

"