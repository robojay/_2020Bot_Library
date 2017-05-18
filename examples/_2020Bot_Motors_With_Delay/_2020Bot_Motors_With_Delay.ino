#include <Bot_Motor.h>

//
// Simple example showing how to initialize the motors,
// then use basic library commands to drive the robot.
//
// Using the delay() function as shown here is not
// the best way to program a robot.  Your program will
// wait for the delay to finish before running any 
// other code.  The delay() function "blocks" code
// until it finishes.  
//
// Once you understand and have experimented with this program,
// look at the _2020Bot_Motors_With_Millis for a better method.
//

// Create a motor instance
// Whenever we want to do something with the motors, we'll
// use a command that starts with "motor."
Bot_Motor motor;

// setup() always runs once when the Arduino starts
void setup() {

  // set up the motors
  // this configures the pins connected to the motor driver
  motor.setup();
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

  motor.forward(driveSpeed);
  delay(driveDelay);
  motor.backward(driveSpeed);
  delay(driveDelay);
  motor.left(turnSpeed);
  delay(turnDelay);
  motor.right(turnSpeed);
  delay(turnDelay);
  motor.stop();
  delay(loopDelay);
    
}
