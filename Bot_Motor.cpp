/*
 * Bot_Motor.cpp
 *
 * 2020Bot Brush Motor Library
 *
 */
#include "Arduino.h"
#include "Bot_Motor.h"

Bot_Motor::Bot_Motor() {
  _leftAPin = LeftAPin;
  _leftBPin = LeftBPin;
  _rightAPin = RightAPin;
  _rightBPin = RightBPin;  
}

Bot_Motor::Bot_Motor(uint8_t leftAPin, uint8_t leftBPin, uint8_t rightAPin, uint8_t rightBPin) {

  _leftAPin = leftAPin;
  _leftBPin = leftBPin;
  _rightAPin = rightAPin;
  _rightBPin = rightBPin;
  

}

void Bot_Motor::setup() {
  analogWrite(_leftAPin, 0);     
  analogWrite(_leftBPin, 0);     
  analogWrite(_rightAPin, 0);     
  analogWrite(_rightBPin, 0);
}

  
// low level motor control
void Bot_Motor::motor(bool motor, int v) {
  static int lastLeft = 0;
  static int lastRight = 0;

  uint8_t a;
  uint8_t b;
  int16_t last; 

  v = (v * 255) / 100;

  if (motor == Left) {
    a = _leftAPin;
    b = _leftBPin;
    last = lastLeft;
  }
  else {
    a = _rightAPin;
    b = _rightBPin;
    last = lastRight;
  }

  v = constrain(v, -255, 255);
  
  if (v != last) {
    if (v > 0) {
      analogWrite(a, v);
      analogWrite(b, 0);
    }
    else {
      analogWrite(a, 0);
      analogWrite(b, -v);
    }
    if (motor == Left) {
      lastLeft = v;
    }
    else {
      lastRight = v;
    }
  }

}

// simplified motor commands, using speed global
void Bot_Motor::forward(int speed) {
  motor(Left, speed + _motorOffset);
  motor(Right, speed - _motorOffset);
}

void Bot_Motor::backward(int speed) {
  motor(Left, -speed - _motorOffset);
  motor(Right, -speed + _motorOffset);
}

void Bot_Motor::left(int speed) {
  motor(Left, -speed - _motorOffset);
  motor(Right, speed + _motorOffset);
}

void Bot_Motor::right(int speed) {
  motor(Left, speed + _motorOffset);
  motor(Right, -speed - _motorOffset);
}

void Bot_Motor::stop() {
  motor(Left, 0);
  motor(Right, 0);
}

void Bot_Motor::moveForward(int speedLeft, int speedRight) {
  motor(Left, speedLeft);
  motor(Right, speedRight);
}

int Bot_Motor::motorOffset() {
  return(_motorOffset);
}

int Bot_Motor::motorOffset(int offset) {
  _motorOffset = offset;
  return(_motorOffset);
}

