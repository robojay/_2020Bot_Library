#include <Bot_IR.h>
#include <Bot_Motor.h>


Bot_IR ir;
Bot_Motor motor;

 //Velikka Remote
const int32_t V_ch_minus = 0x00FFA25D;
const int32_t V_ch = 0x00FF629D;
const int32_t V_ch_plus = 0x00FFE21D;
const int32_t V_prev = 0x00FF22DD;
const int32_t V_next = 0x00FF02FD;
const int32_t V_play = 0x00FFC23D;
const int32_t V_vol_minus = 0x00FFE01F;
const int32_t V_vol_plus = 0x00FFA857;
const int32_t V_eq = 0x00FF906F;
const int32_t V_0 = 0x00FF6897;
const int32_t V_100 = 0x00FF9867;
const int32_t V_200 = 0x00FFB04F;
const int32_t V_1 = 0x00FF30CF;
const int32_t V_2 = 0x00FF18E7;
const int32_t V_3 = 0x00FF7A85;
const int32_t V_4 = 0x00FF10EF;
const int32_t V_5 = 0x00FF38C7;
const int32_t V_6 = 0x00FF5AA5;
const int32_t V_7 = 0x00FF42BD;
const int32_t V_8 = 0x00FF4AB5;
const int32_t V_9 = 0x00FF52AD;
const int32_t No_Code = 0x00000000;


// Arduino standard LED
const uint8_t Led = 13;

// IR Frequency adjustment items
unsigned int irFrequency = 30000;
const unsigned int IrStep = 2000;
const unsigned int IrMax = 50000;
const unsigned int IrMin = 20000;
bool irDetect = false;

// define the speed of motor
int speed = 100;
const unsigned long Timeout = 5000;

typedef enum {None, Forward, Backward, Left, Right, Stop} motion_t;
typedef enum {Remote, Autonomous} robotMode_t;

motion_t robotAction = None;

// behavior requested by bumpers
motion_t bumperRequest;

robotMode_t robotMode = Remote;


 
void setup() {
  Serial.begin(57600);

  ir.setup();
  motor.setup();

  pinMode(Led, OUTPUT);     
   
  for ( int x = 0; x < 3; x++ ) 
  {
    digitalWrite(Led,HIGH);
    delay(100);
    digitalWrite(Led,LOW);
    delay(100);
  }
}

motion_t bumperBehavior() {
  static motion_t lastBumped = None;
  static motion_t bumpAction = None;
  motion_t bumped = None;

//  // interpret the sensors
//  if ((leftSense > LeftThreshold) && (rightSense > RightThreshold)) {
//    bumped = Forward;    
//  }
//  else if (leftSense > LeftThreshold) {
//    bumped = Left;
//  }
//  else if (rightSense > RightThreshold) {
//    bumped = Right;
//  }

  if (irDetect) {
    bumped = Forward;
  }

  // decide on action
  if (bumped != None) {  
    if ((bumpAction != None) && (lastBumped == bumped)) {
      // stay on target...
      // leave bumpAction alone
    }
    else {
      if (bumped == Forward) {
        bumpAction = random(0,2) ? Left : Right;
      }
      else if (bumped == Left) {
        bumpAction = Right;
      }
      else if (bumped == Right) {
        bumpAction = Left;
      }
    }
  }
  else {
    bumpAction = None;
  }

  lastBumped = bumped;
    
  return bumpAction;
}

motion_t arbitrate() {
  motion_t arb = Forward;
  
  if (bumperRequest != None) {
    arb = bumperRequest;
  }

  return arb;
}



 
void loop() {
  static unsigned long timeoutTimer = 0;
  static unsigned long delayTimer = 0;
  uint32_t irCode;
  static unsigned long motionTimeout = Timeout;

  if (ir.rxDataReady()) {
    
    irCode = ir.rxData();
    
    if (irCode != No_Code) {
      Serial.print("Received: 0x");
      Serial.println(irCode, HEX);
    }

    switch (irCode) {
      case V_play:
        robotAction = None;
        robotMode = Autonomous;
        break;
      case V_prev:
        robotAction = Stop;
        robotMode = Remote;
        motionTimeout = Timeout;
        break;
      case V_2:
        robotAction = Forward;
        motionTimeout = Timeout;
        break;
      case V_8:
        robotAction = Backward;
        motionTimeout = Timeout;
        break;
      case V_4:
        robotAction = Left;
        motionTimeout = 100;
        break;
      case V_6:
        robotAction = Right;
        motionTimeout = 100;
        break;
      case V_5:
        robotAction = Stop;
        motionTimeout = Timeout;
        break;
      case V_vol_plus:
        irFrequency += IrStep;
        irFrequency = constrain(irFrequency, IrMin, IrMax);
        break;
      case V_vol_minus:
        irFrequency -= IrStep;
        irFrequency = constrain(irFrequency, IrMin, IrMax);
        break;      
      default:
        robotAction = None;
        break;
    }
    timeoutTimer = millis() + motionTimeout;
  }
  
  if (robotMode == Remote) {
    if (millis() >= timeoutTimer) {
      robotAction = Stop;
      timeoutTimer = millis() + motionTimeout;
    }

    switch (robotAction) {
      case Forward:
        motor.forward(speed);
        break;
      case Backward:
        motor.backward(speed);
        break;
      case Left:
        motor.left(speed);
        break;
      case Right:
        motor.right(speed);
        break;
      case None:
        break;
      default:
        motor.stop();
        break;
    }
  }
  else {
    // autonomous mode

   if (millis() >= delayTimer) {
      irDetect = ir.ping();
  
      bumperRequest = bumperBehavior();
      motion_t move = arbitrate();
  
      switch (move) {
        case Forward:
          motor.forward(speed);
          delayTimer = millis() + 50;
          break;
        case Backward:
          motor.backward(speed);
          delayTimer = millis() + 50;
          break;        
        case Left:
          motor.left(speed);
          delayTimer = millis() + 250;
          break;
        case Right:
          motor.right(speed);
          delayTimer = millis() + 250;
          break;        
        case Stop:
          motor.stop();
          delayTimer = millis() + 50;
          break;
        default:
          break;
      }
    }        
  }

  
} // end loop
 
 
