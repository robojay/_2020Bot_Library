#ifndef Bot_IR_h
#define Bot_IR_h


#include "Arduino.h"
#if defined(ARDUINO_ARCH_ESP32)
    // TimerOne?
#else
    #include <TimerOne.h>
#endif

class Bot_IR {
    public:
        Bot_IR();
        Bot_IR(uint8_t txTonePin, uint8_t txDataPin, uint8_t rxPin);
        void setup();
        void setup(bool txInitialize);
        unsigned int txFrequency();
        unsigned int txFrequency(unsigned int setTxFrequency);
        uint8_t txDataPin();
        uint8_t txDataPin(uint8_t setTxDataPin);
        bool rxBusy();
        bool txBusy();
        bool rxDataReady();
        uint32_t rxData();
        void irRxIntEnable();
        void irRxIntDisable();
        bool txData(uint32_t data);
        bool txData(uint32_t data, bool wait);
        void txInit();
        void irTxIntEnable(); 
        void irTxIntDisable();
        void txPing();
        bool rxPing();
        bool rxPing(bool wait);
        bool ping();
        
        #if defined(ARDUINO_ARCH_ESP32)
            const uint8_t IrRxPin = 4;
            const uint8_t IrTxTonePin = 15;
            const uint8_t IrTxDataPin = 2;

            const uint8_t IrTonePwm = 4;
            const uint8_t IrToneResolution = 8;
        #else
            const uint8_t IrRxPin = 2;
            const uint8_t IrTxTonePin = 3;
            const uint8_t IrTxDataPin = 12;
        #endif

        const unsigned int IrTxFrequency = 40000;
        
        const int8_t PingTimeout = -1;
        const int8_t PingDetected = 1;
        const int8_t PingNotDetected = 0;

        static Bot_IR* _defaultInstance;
        static void irEdgeInterruptHandler();
        static void irTxInterruptHandler();
        
        
    private:

        void irEdge();
        void irTx();
        
        uint8_t _txTonePin;
        uint8_t _rxPin;
        uint8_t _txDataPin;
        unsigned int _txFrequency;
        
        volatile bool _irBusy = false;
        volatile bool _irEmpty = true;
        volatile uint32_t _irReceived = 0;
        volatile bool _irEnabled = false;
        volatile bool _irEdgeDetect = false;
        
        bool _txToneActive = false;

        const unsigned long StartMin = 3000;
        const unsigned long StartMax = 6000;
        const unsigned long ZeroOneThreshold = 1000;

        volatile uint32_t _irTxData = 0x00000000;
        volatile bool _irTxEnable = false;
        const unsigned long IrTxTime = 500;
        const uint8_t StartLowCount = 18;
        const uint8_t StartHighCount = 10;
        const uint8_t ZeroPulse = 0;
        const uint8_t OnePulse = 2;
        enum IrTxStates {Idle, StartLow, StartHigh, PulseHigh, PulseLow, Ping};
        
        const unsigned long PingTimeoutTime = 1000;
        volatile bool _irTxPingEnable = false;
        volatile bool _irTxPing = false;
        volatile bool _irRxPing = false;
        
};

#endif

