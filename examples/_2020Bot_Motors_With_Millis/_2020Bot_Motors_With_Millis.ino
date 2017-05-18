#include <Bot_Motor.h>

//
// Simple example showing how to initialize the motors,
// then use basic library commands to drive the robot.
//
// This example uses the millis() function instead of delay()
// By using millis(), we can do other things while waiting
// for a motor action to finish.
//
// To do this, we have to be a little smarter, and also keep track
// of which "step" we're on in our desired motion pattern. 
//

// Create a motor instance
// Whenever we want to do something with the motors, we'll
// use a command that starts with "motor."
Bot_Motor motor;

// Arduino standard LED
const uint8_t Led = 13;

// setup() always runs once when the Arduino starts
void setup() {

  // set up the motors
  // this configures the pins connected to the motor driver
  motor.setup();

  // set the LED pin as an output
  pinMode(Led, OUTPUT);     

}

// The motor commands we're going to use here are:
// motor.forward(speed)
// motor.backward(speed)
// motor.left(speed)
// motor.right(speed)
// motor.stop()
//
// speed is an integer from 0 to 100, which means 0% to 100%
//
// forward and backward do what they say
// left and right will spin the robot in place to the left (counter clockwise)
// and right (clockwise)
// stop turns off the motors

// loop() runs over and over again
void loop() {
  // use variables for the speeds we want to use 
  int driveSpeed = 75;
  int turnSpeed = 50;

  // use variables for the time delays
  // delay() uses milliseconds (there are 1000 milliseconds in one second)
  int driveDelay = 1000;
  int turnDelay = 500;
  int loopDelay = 3000;

  // We'll use this to keep track of when our next action
  // should take place.  Note the "static" at the beginning.
  // This means that the variable will only be initialized once, and
  // will keep its value even though loop() executes over and over.
  static unsigned long motionTimer = 0;

  // We need to keep track of which step in the pattern we're on,
  // and it also needs to keep its value every time loop() executes 
  static unsigned int motionStep = 0;

  // take a snapshot of the current time
  unsigned long rightNow = millis();

  // check to see if the current time is past the motorDelay,
  // if so, then it's time to take the next step

  if (rightNow >= motionTimer) {
      
    // Do the proper action based on motionStep
    // This examples uses the if...else format for clarity.
    // Often the switch...case structure is used for this type
    // of control

    // for each step:
    //   we'll start the motion
    //   calculate a new time delay equal to the current time plus a delay
    //   set the next step
  
    if (motionStep == 0) {
      motor.forward(driveSpeed);
      motionTimer = rightNow + driveDelay;
      motionStep = 1;
    }
    else if (motionStep == 1) {
      motor.backward(driveSpeed);
      motionTimer = rightNow + driveDelay;
      motionStep = 2;
    }
    else if (motionStep == 2) {
      motor.left(turnSpeed);
      motionTimer = rightNow + turnDelay;
      motionStep = 3;
    }
    else if (motionStep == 3) {
      motor.right(turnSpeed);
      motionTimer = rightNow + turnDelay;
      motionStep = 4;
    }
    else if (motionStep == 4) {
      motor.stop();
      motionTimer = rightNow + loopDelay;
      motionStep = 0;
    }
    else {
      // it's a good idea to catch any unexpected states
      // and do something reasonable
      motor.stop();
      motionTimer = rightNow + loopDelay;
      motionStep = 0;
    }
  }

  // Now we're going to add something tricky...
  // This shows how you can have multiple things going on
  // within the loop(), as long as you don't "block" your
  // code using delay()

  // see if you can figure this out...
  int ledDelay = 250;
  static unsigned int ledStep = 0;
  static unsigned long ledTimer = 0;

  if (rightNow >= ledTimer) {
    switch(ledStep) {
      case 0:
        digitalWrite(Led, LOW);
        ledTimer = rightNow + ledDelay;
        ledStep = 1;
        break;
      case 1:
        digitalWrite(Led, HIGH);
        ledTimer = rightNow + ledDelay;
        ledStep = 0;
        break;
      default:
        digitalWrite(Led, HIGH);
        ledTimer = rightNow + ledDelay;
        ledStep = 0;
        break;          
    }
  }

}
