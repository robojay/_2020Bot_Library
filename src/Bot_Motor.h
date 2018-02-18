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
        #if defined(ARDUINO_ARCH_ESP32)
            const uint8_t LeftAPin = 16;
            const uint8_t LeftBPin = 17;
            const uint8_t RightAPin = 21;
            const uint8_t RightBPin = 22;

            const uint16_t PwmFreq = 50000;
            const uint8_t PwmResolution = 8;

            const uint8_t LeftApwm = 0;
            const uint8_t LeftBpwm = 1;
            const uint8_t RightApwm = 2;
            const uint8_t RightBpwm = 3;

        #else
            const uint8_t LeftAPin = 10;
            const uint8_t LeftBPin = 9;
            const uint8_t RightAPin = 6;
            const uint8_t RightBPin = 5;
        #endif

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
