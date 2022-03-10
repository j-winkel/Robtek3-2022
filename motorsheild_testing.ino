
//******************************
// pin constants
//******************************

  // analog pins:
  int lineSensorA = 5;
  int lineSensorB = 4;

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

  enum control{
    forward,
    reverse,
    left,
    right,
    halt
  };

  int qreValue;

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

// rewrite function caause it sucks 

int readLineSensorA(){
  qreValue = analogRead(lineSensorA);
  return qreValue;
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
