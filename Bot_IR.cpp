/*
 * Bot_IR.cpp
 *
 * 2020Bot Infrared Remote Control and Sensor Library
 *
 */


#include "Arduino.h"
#include "Bot_IR.h"

#if defined(ARDUINO_ARCH_ESP32)
  hw_timer_t* timer = NULL;
#else
  #include <TimerOne.h>
#endif

// Initializers
// If no parameters are given, will default to the standard pins
// used on the 2020Bot
Bot_IR::Bot_IR() {
    _txTonePin = IrTxTonePin;
    _txDataPin = IrTxDataPin;
    _rxPin = IrRxPin;
    _txFrequency = IrTxFrequency;
}

// Use this initializer if you want different pins
Bot_IR::Bot_IR(uint8_t txTonePin, uint8_t txDataPin, uint8_t rxPin) {
    _txTonePin = txTonePin;
    _txDataPin = txDataPin;
    _rxPin = rxPin;
    _txFrequency = IrTxFrequency;    
}

// This is a little tricky...
// Interrupt routines can only be attached to static methods
// This keeps track of which instance of the IR class will handle
// interrupts.  Advanced programmers could manipulate these for
// their own purposes.
Bot_IR* Bot_IR::_defaultInstance;

void Bot_IR::irEdgeInterruptHandler() {
    Bot_IR::_defaultInstance->irEdge();
}

void Bot_IR::irTxInterruptHandler() {
    Bot_IR::_defaultInstance->irTx();
}

// Read back the current transmit frequency
unsigned int Bot_IR::txFrequency() {
    return _txFrequency;
}

// Change the transmit frequency tone
// Useful for matching with different receivers
// and also for taking advantage of the receiver bandpass filters
// to create a simple range finder.
// If the tranmitter is turned on, the new frequency will immediately
// be generated.
unsigned int Bot_IR::txFrequency(unsigned int setTxFrequency) {
    _txFrequency = setTxFrequency;
    if (_txToneActive) {
        #if defined(ARDUINO_ARCH_ESP32)
          ledcSetup(IrTonePwm, _txFrequency, IrToneResolution);
          ledcWrite(IrTonePwm, 1<<(IrToneResolution - 1));
        #else
          tone(_txTonePin, _txFrequency);
        #endif
    }
    return _txFrequency;
}

// Read back the current transmit data pin
uint8_t Bot_IR::txDataPin() {
    return _txDataPin;
}

// Set and read back the current transmit data pin
uint8_t Bot_IR::txDataPin(uint8_t setTxDataPin) {
    _txDataPin = setTxDataPin;
    return _txDataPin;
}

// Enable receiver interrupts
void Bot_IR::irRxIntEnable() {
  attachInterrupt(digitalPinToInterrupt(_rxPin), Bot_IR::irEdgeInterruptHandler, CHANGE);
}

// Disable receiver interrupts
void Bot_IR::irRxIntDisable() {
  detachInterrupt(digitalPinToInterrupt(_rxPin));
}

// Configure pins, receiver, and transmitter
void Bot_IR::setup() {
    setup(true);
}

// If the transmitter isn't going to be used, can pass false
// to setup
void Bot_IR::setup(bool txInitialize) {
    pinMode(_rxPin, INPUT);
    #if defined(ARDUINO_ARCH_ESP32)
      ledcSetup(IrTonePwm, _txFrequency, IrToneResolution);
      ledcAttachPin(_txTonePin, IrTonePwm);
      ledcWrite(IrTonePwm, 0);
    #else
      pinMode(_txTonePin, OUTPUT);
      digitalWrite(_txDataPin, LOW);
    #endif
    pinMode(_txDataPin, INPUT);
    Bot_IR::_defaultInstance = this;
    irRxIntEnable();
    if (txInitialize) {
        txInit();
    }
}

// Edge detect interrupt routing for the receiver
// This decodes start pulses, then 32 bits of data
// It is designed to work with the basic IR Remotes supplied with the 2020Bot
// and also to listen to other 2020Bots
// Different remotes may not (probably won't) work!
void Bot_IR::irEdge() {
  static unsigned long _irStartTime = 0;
  static unsigned long _irHighTime = 0;
  static uint8_t _irCount = 0;
    
  // what edge are we dealing with?
  // a high means we saw a rising edge,
  // a low means we saw a falling edge
  // the pulses we care about are high pulses
  // keep track of when we saw a rising edge
  if (digitalRead(_rxPin) == HIGH) {

    // rising edge, capture start time
    _irStartTime = micros();
  }
  else {
    // falling edge
    // calculate the duration of the high pulse    
    _irHighTime = micros() - _irStartTime;

    // if transmitting a ping, indicate that the
    // pulse was received
    if (_irTxPing) {
        _irRxPing = true;
    }

    // was this a start pulse, with a duration between StartMin and StartMax?
    // if so, and if the receive buffer is empty, prepare to receive a new word
    if (_irHighTime < StartMax) {
      if ( _irHighTime > StartMin ) {
        // if so, and if the receive buffer is empty, prepare to receive a new word
        // note that we are now busy, and that the buffer is not empty
        if (_irEmpty) {
          _irBusy = true;
          _irReceived = 0;
          _irEmpty = false;
          _irCount = 0;
        }
      }
      else {
        // if it wasn't a start pulse, and we're busy receiving pulses
        // shift the received word, put the next bit in place,
        // and check to see if we've received 32 bits
        // if we hit 32 bits, we aren't busy any longer, and the buffer
        // is not empty
        if (_irBusy) {
          _irReceived = _irReceived<<1;
          _irReceived |= (_irHighTime > ZeroOneThreshold) ? 1 : 0;
          _irCount++;
          if (_irCount >= 32) {
            _irBusy = false;
          }
        }
      }
    }
  }
}

// Check the busy status of the receiver
bool Bot_IR::rxBusy() {
    return (_irBusy);
}

// Data is ready to be read from the receive buffer
// when the receiver is not busy and the buffer is not empty
bool Bot_IR::rxDataReady() {
    return (!_irBusy && !_irEmpty);    
}

// Retrieve the data in the receive buffer
// If this is called when the receive data is not ready, it will return 0
// Most IR transmissions are not all zero, so it is relatively safe to check
// for this condition
// The receive buffer is marked as empty once the data is read
uint32_t Bot_IR::rxData() {
    uint32_t currentData;
    
    if (!rxDataReady()){
        return((uint32_t)(0));
    }
    else {
        currentData = _irReceived;
        _irEmpty = true;
        return(currentData);
    }
}

// Transmitter interrupt state machine
// This is called using the Timer1 timer interrupt every 500us
void Bot_IR::irTx() {
  static uint8_t _irTxCount = 0;
  static uint8_t _irOneBit = 0;
  static IrTxStates _irTxState = Idle;

  switch(_irTxState) {
    // Idle - transmitter is waiting to do something
    // it will leave Idle if there is a word to transmit (_irTxEnable set)
    // or if a ping pulse has been requested (_irTxPingEnable set)
    // As it leaves Idle, it will configure flags and counters as well as
    // setting the transmit data pin
    // note: setting the data pin HIGH generates a burst of IR at the
    // transmit frequency, which is received as a LOW at the IR receiver
    case Idle:
      if (_irTxEnable) {
        pinMode(_txDataPin, OUTPUT);
        _irTxState = StartLow;
        digitalWrite(_txDataPin, HIGH);
        _irTxCount = StartLowCount;
      }
      else if (_irTxPingEnable) {
        _irTxPingEnable = false;
        _irRxPing = false;
        _irTxPing = true;
        pinMode(_txDataPin, OUTPUT);
        digitalWrite(_txDataPin, HIGH);
        _irTxState = Ping;
      }
      else {
        _irTxState = Idle;
      }
      break;
    // StartLow - continue to generate a low pulse as part of the start pulse
    // before the actual data.  Once it has been generated long enough, configure
    // the data pin to generate a high pulse for the proper duration, and transition
    // to the StartHigh state
    case StartLow:
      if (_irTxCount > 0) {
        _irTxCount--;
        _irTxState = StartLow;
      }
      else {
        _irTxState = StartHigh;
        digitalWrite(_txDataPin, LOW);
        _irTxCount = StartHighCount;        
      }
      break;
    // StartHigh - continue to generate a high pulse as part of the start pulse
    // once that's done, transition to PulseLow and prepare to send actual data
    case StartHigh:
      if (_irTxCount > 0) {
        _irTxCount--;
        _irTxState = StartHigh;
      }
      else {
        _irTxState = PulseLow;
        digitalWrite(_txDataPin, HIGH);
        _irTxCount = 32;        
      }
      break;
    // PulseLow - generate a low pulse for the proper duration
    // once the duration is hit, look at the high bit in the transmit data
    // and start the proper duration high pulse
    // a zero is sent for ZeroPulse count, a one is sent for OnePulse counts
    // if this was the last pulse, go back to Idle
    case PulseLow:
      digitalWrite(_txDataPin, LOW);
      if (_irTxCount > 0) {
        _irTxState = PulseHigh;
        if (_irTxData & 0x80000000) {
          _irOneBit = OnePulse;
        }
        else {
          _irOneBit = ZeroPulse;
        }
      }
      else {
        _irTxState = Idle;
        _irTxEnable = false;
        pinMode(_txDataPin, INPUT);
      }
      break;    
    // PulseHigh - continue to generate a high pulse for the proper duration
    // once done, shift the transmit data to the next bit and send low pulse
    case PulseHigh:
      if (_irOneBit > 0) {
        _irOneBit--;
        _irTxState = PulseHigh;
      }
      else {
        _irTxCount--;
        _irTxData = _irTxData << 1;
        digitalWrite(_txDataPin, HIGH);
        _irTxState = PulseLow;
      }
      break;
    // Ping - "one ping only, please"
    // finishes off the ping that started in Idle
    case Ping:
        _irTxState = Idle;
        _irTxPing = false;
        pinMode(_txDataPin, INPUT);
    // Should never get here!
    default:
      _irTxState = Idle;
      _irTxEnable = false;
      break;
  }
}

// Check if the transmitter is busy either sending data or generating a ping
bool Bot_IR::txBusy() {
    return (_irTxEnable || _irTxPing);
}

// Transmit a 32 bit word
// By default, this will block until the transmitter is ready to send data
// Use the more explicit send routine if you don't want to block and prefer
// to handle waits yourself
bool Bot_IR::txData(uint32_t data) {
    return(txData(data, true));
}

bool Bot_IR::txData(uint32_t data, bool wait) {
    while(txBusy() && wait);
    if (txBusy()) {
        return(false);
    }
    else {
        _irTxData = data;
        _irTxEnable = true;
    }
    return(true);
}

// Initialize the transmitter
// set up the tone and data pins
// enable the interrupts
void Bot_IR::txInit() {
  pinMode(_txTonePin, OUTPUT);
  digitalWrite(_txDataPin, LOW);
  pinMode(_txDataPin, INPUT);
  #if defined(ARDUINO_ARCH_ESP32)
    timer = timerBegin(0, 80, true);  
    timerAttachInterrupt(timer, Bot_IR::irTxInterruptHandler, true);
    timerAlarmWrite(timer, 500, true);
  #else
    Timer1.initialize(IrTxTime);
  #endif
  irTxIntEnable();
  #if defined(ARDUINO_ARCH_ESP32)
    ledcWrite(IrTonePwm, 1<<(IrToneResolution - 1));
  #else
    tone(_txTonePin, _txFrequency);
  #endif
  _txToneActive = true;
}

// Enable the transmitter interrupts
void Bot_IR::irTxIntEnable() {
  #if defined(ARDUINO_ARCH_ESP32)
    timerAlarmEnable(timer);  
  #else
    Timer1.attachInterrupt(Bot_IR::irTxInterruptHandler);
  #endif
}

// Disable the transmitter interrupts
void Bot_IR::irTxIntDisable() {
  #if defined(ARDUINO_ARCH_ESP32)
    timerAlarmDisable(timer);  
  #else
    Timer1.detachInterrupt();
  #endif
}

// Send a ping pulse
void Bot_IR::txPing() {
    if (_irTxPing)
        return;
    _irTxPingEnable = true;
}

// Check to see if a ping pulse was received
// by default, this will wait for up to PingTimeoutTime duration
// before returning
// Use the  more explicit routine if you don't want to wait
// The wait version will return as soon as a pulse is detected,
// or if the ping pulse finished, or if the timer expires
bool Bot_IR::rxPing() {
    return(rxPing(true));
}

bool Bot_IR::rxPing(bool wait) {
    unsigned long rxPingTimer = micros() + PingTimeoutTime;
    if (wait) {
        while ((_irTxPingEnable || _irTxPing) && (micros() < rxPingTimer) && !_irRxPing);
    }
    return(_irRxPing);
}

// Convenience function to send a ping and wait for it's return
bool Bot_IR::ping() {
    txPing();
    return(rxPing());
}

