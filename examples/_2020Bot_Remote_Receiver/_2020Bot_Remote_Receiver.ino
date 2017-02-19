#include <Bot_IR.h>
#include <Bot_Motor.h>
#include <Bot_Remote.h>
#include <Bot_RemoteType.h>

Bot_IR ir;

// Arduino standard LED
const uint8_t Led = 13;

void setup() {
  Serial.begin(57600);

  ir.setup();

  pinMode(Led, OUTPUT);     
   
  for ( int x = 0; x < 3; x++ ) 
  {
    digitalWrite(Led,HIGH);
    delay(100);
    digitalWrite(Led,LOW);
    delay(100);
  }
}

void loop() {
  uint32_t irCode;

  if (ir.rxDataReady()) {
    
    irCode = ir.rxData();
    
    if (irCode != Remote_no_code) {
      Serial.print("Received: 0x");
      Serial.println(irCode, HEX);
    }
  }

}
