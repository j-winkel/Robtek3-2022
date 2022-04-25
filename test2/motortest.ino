
// channel A:
#define directionA 12 // HIGH = forward, LOW = reverse
#define speedA 3
#define brakeA 9


// channel B
#define directionB 13 // HIGH = forward, LOW = reverse
#define speedB 10
#define brakeB 8
#define currentSensingB A1

enum control
{
  forward,
  reverse,
  left,
  right,
  halt
};
void setup() {
    // Setup motor A
  pinMode(directionA, OUTPUT); // Initiates Motor Channel A pin
  pinMode(brakeA, OUTPUT);     // Initiates Brake Channel A pin
  pinMode(speedA, OUTPUT);

  // Setup motor B
  pinMode(directionB, OUTPUT); // Initiates Motor Channel B pin
  pinMode(brakeB, OUTPUT);     // Initiates Brake Channel B pin
  pinMode(speedB, OUTPUT);
}
void loop() {
    delay(1000);
    motorDirection(forward, 150);
     delay(1000);
    motorDirection(left, 150);
     delay(1000);
    motorDirection(reverse, 150);
}

void motorControls(int dir, bool dirValue, int brake, bool breakValue, int velocityName, int velocityValue)
{
  digitalWrite(dir, dirValue);
  digitalWrite(brake, breakValue);
  analogWrite(velocityName, velocityValue);
}

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