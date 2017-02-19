#ifndef Bot_Motor_h
#define Bot_Motor_h

#include "Arduino.h"

class Bot_Motor {
    public:
        Bot_Motor();
        Bot_Motor(uint8_t leftAPin, uint8_t leftBPin, uint8_t rightAPin, uint8_t rightBPin);

        void motor(bool motor, int v);
        void forward(int speed);
        void backward(int speed);
        void left(int speed);
        void right(int speed);
        void stop();
        void moveForward(int speedLeft, int speedRight);
        int motorOffset();
        int motorOffset(int offset);
        void setup();
        
        // Default Motor Pins
        const uint8_t LeftAPin = 10;
        const uint8_t LeftBPin = 9;
        const uint8_t RightAPin = 6;
        const uint8_t RightBPin = 5;
        
        const bool Left = false;
        const bool Right = true;

        
    private:

        uint8_t _leftAPin;
        uint8_t _leftBPin;
        uint8_t _rightAPin;
        uint8_t _rightBPin;
        int _motorOffset = 0;
        
};


#endif
