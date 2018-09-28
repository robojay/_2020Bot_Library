#include "WiFi.h"
#include "Esp.h"
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <netinet/in.h>
#include <Preferences.h>

#include <Bot_IR.h>
#include <Bot_Motor.h>
#include <Bot_RemoteType.h>

// Default remote will be the standard 2020 Bot remote
// If you have a different one that is defined in Bot_Remote.h
// select it here.
// Otherwise, look at Bot_Remote.h for the required constants,
// and use the Remote Receiver example to decode your specific
// remote control.  NOT ALL REMOTES WILL WORK!

//#define RemoteType VelikkaRemote

#include <Bot_Remote.h>

Bot_IR ir;
Bot_Motor motor;

// IR Frequency adjustment items
unsigned int irFrequency = 30000;
const unsigned int IrStep = 2000;
const unsigned int IrMax = 50000;
const unsigned int IrMin = 20000;
bool irDetect = false;

// define the speed of motor
int speed = 100;
int motorOffset = 0;
const int MotorOffsetMax = 10;
const int MotorOffsetMin = -10;
const int MotorOffsetStep = 2;
const unsigned long Timeout = 5000;

typedef enum {None, Forward, Backward, Left, Right, Stop} motion_t;
typedef enum {Remote, Autonomous} robotMode_t;

typedef enum {WebNone, WebForward, WebReverse, WebLeft, WebRight, WebStop, WebAuto, WebRemote} webCommand_t;

motion_t robotAction = None;
webCommand_t webCommand = WebNone;

// behavior requested by bumpers
motion_t bumperRequest;

robotMode_t robotMode = Remote;

// prefix to use for the default access point SSID
const char SsidPrefix[] = "2020Bot_";

// default password for the access point
const char DefaultPassword[] = "20202020";

// default local network name
const char DefaultLocalName[] = "2020bot";

// helper array to convert to a hex string
const char Hex[] = "0123456789ABCDEF";

// Arduino standard 
const uint8_t Led = 13;

const uint8_t ClearDefaultsPin = 0;

// Configuration Structure
struct Config {
  char ssid[32 + 1];      // 32 bytes plus null 
  char password[64 + 1];  // 64 bytes plus null
  bool apMode;            // true if access point, false if station
  char localName[32 + 1]; // advertised name
} robotConfig;

Preferences preferences;

// Create the web server on the normal port
WebServer server(80);

// Main web page for the robot 
void handleRoot() {
  digitalWrite(Led, HIGH);
  String root = ""
    "<!doctype html>"
    "<html lang=en>"
    "<head>"
    "<meta charset=utf-8>"
    "<title>2020 Bot</title>"
        "<script>"
        "function remote() {"
        "  var xhttp = new XMLHttpRequest();"
        "  xhttp.onreadystatechange = function() {"
        "    if (this.readyState == 4 && this.status == 200) {"
        "      "
        "    }"
        "  };"
        "  xhttp.open(\"GET\", \"remote\", true);"
        "  xhttp.send();"
        "}"
        "function auto() {"
        "  var xhttp = new XMLHttpRequest();"
        "  xhttp.onreadystatechange = function() {"
        "    if (this.readyState == 4 && this.status == 200) {"
        "      "
        "    }"
        "  };"
        "  xhttp.open(\"GET\", \"auto\", true);"
        "  xhttp.send();"
        "}"
        "function forward() {"
        "  var xhttp = new XMLHttpRequest();"
        "  xhttp.onreadystatechange = function() {"
        "    if (this.readyState == 4 && this.status == 200) {"
        "      "
        "    }"
        "  };"
        "  xhttp.open(\"GET\", \"forward\", true);"
        "  xhttp.send();"
        "}"
        "function reverse() {"
        "  var xhttp = new XMLHttpRequest();"
        "  xhttp.onreadystatechange = function() {"
        "    if (this.readyState == 4 && this.status == 200) {"
        "      "
        "    }"
        "  };"
        "  xhttp.open(\"GET\", \"reverse\", true);"
        "  xhttp.send();"
        "}"
        "function left() {"
        "  var xhttp = new XMLHttpRequest();"
        "  xhttp.onreadystatechange = function() {"
        "    if (this.readyState == 4 && this.status == 200) {"
        "      "
        "    }"
        "  };"
        "  xhttp.open(\"GET\", \"left\", true);"
        "  xhttp.send();"
        "}"
        "function right() {"
        "  var xhttp = new XMLHttpRequest();"
        "  xhttp.onreadystatechange = function() {"
        "    if (this.readyState == 4 && this.status == 200) {"
        "      "
        "    }"
        "  };"
        "  xhttp.open(\"GET\", \"right\", true);"
        "  xhttp.send();"
        "}"
        "function stop() {"
        "  var xhttp = new XMLHttpRequest();"
        "  xhttp.onreadystatechange = function() {"
        "    if (this.readyState == 4 && this.status == 200) {"
        "      "
        "    }"
        "  };"
        "  xhttp.open(\"GET\", \"stop\", true);"
        "  xhttp.send();"
        "}"
        "</script>"
    "</head>"
    "<body>"
    "<svg width=\"980\" height=\"980\">"
        "<polygon points=\"490,5 300,300 700,300 490,5\""
            "style=\"fill:lime;stroke:black;stroke-width:5;fill-rule:evenodd;\" onclick=\"forward();\" />"
        "<polygon points=\"5,500 300,300 300,700 5,500\""
            "style=\"fill:lime;stroke:black;stroke-width:5;fill-rule:evenodd;\" onclick=\"left();\" />"
        "<polygon points=\"975,500 700,300 700,700 975,500\""
            "style=\"fill:lime;stroke:black;stroke-width:5;fill-rule:evenodd;\" onclick=\"right();\" />"
        "<polygon points=\"490,975 300,700 700,700 490,975\""
            "style=\"fill:lime;stroke:black;stroke-width:5;fill-rule:evenodd;\" onclick=\"reverse();\" />"
        "<circle cx=\"498\" cy=\"498\" r=\"180\""
            "stroke=\"black\" stroke-width=\"5\" fill=\"red\" onclick=\"stop();\"/>"
        "<circle cx=\"150\" cy=\"150\" r=\"100\""
            "stroke=\"black\" stroke-width=\"5\" fill=\"lime\" onclick=\"remote();\"/>"
        "<circle cx=\"830\" cy=\"150\" r=\"100\""
            "stroke=\"black\" stroke-width=\"5\" fill=\"blue\" onclick=\"auto();\"/>"
        "Sorry, your browser does not support inline SVG."
    "</svg>"
    "</body>"
    "</html>";
    
  server.send(200, "text/html", root);
  digitalWrite(Led, LOW);
}

void handleLeft() {
  webCommand = WebLeft;
  Serial.println("Left");
  server.send(200, "text/plain", "left");
}

void handleRight() {
  webCommand = WebRight;
  Serial.println("Right");
  server.send(200, "text/plain", "right");
}

void handleForward() {
  webCommand = WebForward;
  Serial.println("Forward");
  server.send(200, "text/plain", "forward");
}

void handleReverse() {
  webCommand = WebReverse;
  Serial.println("Reverse");
  server.send(200, "text/plain", "reverse");
}

void handleStop() {
  webCommand = WebStop;
  Serial.println("Stop");
  server.send(200, "text/plain", "stop");
}

void handleAuto() {
  webCommand = WebAuto;
  Serial.println("Autonomous");
  server.send(200, "text/plain", "auto");
}

void handleRemote() {
  webCommand = WebRemote;
  Serial.println("Remote Control");
  server.send(200, "text/plain", "remote");
}

void handleConfig() {
  
  String config = ""
    "<!doctype html>"
    "<html lang=en>"
    "<head>"
    "<meta charset=utf-8>"
    "<title>2020 Bot</title>"
    "</head>"
    "<body>"
        "<form action=\"/save\" method=\"post\">"
            "SSID :"
            "<input type=\"text\" name=\"ssid\" value=\"";
  config += String(robotConfig.ssid);
  config += "\"><br>"
            "Password :"
            "<input type=\"text\" name=\"password\" value=\"";
  config += String(robotConfig.password);
  config += "\"><br>";
  config += "<input type=\"radio\" name=\"apmode\" value=\"true\"";
  if (robotConfig.apMode) {
    config += " checked";
  }
  config += ">Access Point"
            "<input type=\"radio\" name=\"apmode\" value=\"false\"";
  if (!robotConfig.apMode) {
    config += " checked";
  }
  config += ">WiFi Client<br>"
            "Local Name :"
            "<input type=\"text\" name=\"localname\" value=\"";
  config += String(robotConfig.localName);
  config += "\"><br>"
            "<input type=\"submit\" value=\"Submit\">"
        "</form>"
    "</body>"
    "</html>";

  digitalWrite(Led, HIGH);
  server.send(200, "text/html", config);
  digitalWrite(Led, LOW);
}

void handleSave() {
  char buf[65];
  String save = ""
    "<!doctype html>"
    "<html lang=en>"
    "<head>"
    "<head>"
    "<meta http-equiv=\"Refresh\" content=\"5; url=http://";
  save += String(robotConfig.localName);
  save += ".local/";
  save += "\">";
  save += "<body>Settings saved... rebooting...</body>";
  save += "</head>";
  
  if (server.args() != 0) {
    // clear, then set the ssid
    memset(robotConfig.ssid, '\0', sizeof(robotConfig.ssid));
    server.arg(0).toCharArray(buf, sizeof(buf));
    strcpy(robotConfig.ssid, buf);

    // clear, then set the password
    memset(robotConfig.password, '\0', sizeof(robotConfig.password));
    server.arg(1).toCharArray(buf, sizeof(buf));
    strcpy(robotConfig.password, buf);

    if (server.arg(2) == "true") {
      robotConfig.apMode = true;
    }
    else {
      robotConfig.apMode = false;
    }
    
    // clear, then set local name
    memset(robotConfig.localName, '\0', sizeof(robotConfig.localName));
    server.arg(3).toCharArray(buf, sizeof(buf));
    strcpy(robotConfig.localName, buf);

    Serial.println("Saving settings...");

    Serial.print("SSID: ");
    Serial.println(robotConfig.ssid);
    Serial.print("Password: ");
    Serial.println(robotConfig.password);
    Serial.print("Mode: ");
    if (robotConfig.apMode) {
      Serial.println("Access Point");
    }
    else {
      Serial.println("Station");
    }
    Serial.print("Local Network Name: ");
    Serial.print(robotConfig.localName);
    Serial.println(".local");

    ir.irTxIntDisable();
    preferences.begin("2020bot", false);
    preferences.putBytes("robotConfig", (unsigned char *)&robotConfig, sizeof(robotConfig));
    preferences.end();
    ir.irTxIntEnable();

    Serial.println("Settings saved");
  }
  server.send(200, "text/html", save);
  delay(2000);
  ESP.restart();    
}

void handleNotFound() {
  digitalWrite(Led, HIGH);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(Led, LOW);
}

void setDefaults() {
  // clear the ssid
  memset(robotConfig.ssid, '\0', sizeof(robotConfig.ssid));
  // copy in the default prefix
  strcpy(robotConfig.ssid, SsidPrefix);

  // get the NIC portion of the MAC address
  uint32_t macNic = ntohl((uint32_t)(ESP.getEfuseMac() >> 24)) >> 8;

  // extend with the hex digits of the NIC portion of the MAC address
  robotConfig.ssid[sizeof(SsidPrefix) - 1] = Hex[(macNic >> 20) & 0x0f];
  robotConfig.ssid[sizeof(SsidPrefix)] = Hex[(macNic >> 16) & 0x0f];
  robotConfig.ssid[sizeof(SsidPrefix) + 1] = Hex[(macNic >> 12) & 0x0f];
  robotConfig.ssid[sizeof(SsidPrefix) + 2] = Hex[(macNic >> 8) & 0x0f];
  robotConfig.ssid[sizeof(SsidPrefix) + 3] = Hex[(macNic >> 4) & 0x0f];
  robotConfig.ssid[sizeof(SsidPrefix) + 4] = Hex[macNic & 0x0f];
  robotConfig.ssid[sizeof(SsidPrefix) + 5] = 0x00;  

  // clear the password
  memset(robotConfig.password, '\0', sizeof(robotConfig.password));
  // copy in the default password
  strcpy(robotConfig.password, DefaultPassword);

  // we're going to start as a access point
  robotConfig.apMode = true;

  // clear the local name
  memset(robotConfig.localName, '\0', sizeof(robotConfig.localName));
  // set the default local name
  strcpy(robotConfig.localName, DefaultLocalName);
}

motion_t bumperBehavior() {
  static motion_t lastBumped = None;
  static motion_t bumpAction = None;
  motion_t bumped = None;

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

void setup() {
  pinMode(Led, OUTPUT);
  pinMode(ClearDefaultsPin, INPUT_PULLUP);
  
  digitalWrite(Led, LOW);
  Serial.begin(115200);

  for ( int x = 0; x < 3; x++ ) 
  {
    digitalWrite(Led,HIGH);
    delay(100);
    digitalWrite(Led,LOW);
    delay(100);
  }
  
  // attempt to read the configuration
  preferences.begin("2020bot", false);
  if (preferences.getBytes("robotConfig", (unsigned char *)&robotConfig, sizeof(robotConfig)) == 0) {
    // must be a fresh boot, create a default entry
    Serial.println("Setting default robot configuration");
    setDefaults();
    preferences.putBytes("robotConfig", (unsigned char *)&robotConfig, sizeof(robotConfig));
  }
  else {
    // they exist, good deal...
    Serial.println("Read robot configuration");
    preferences.getBytes("robotConfig", (unsigned char *)&robotConfig, sizeof(robotConfig));
  }
  preferences.end();

  Serial.print("SSID: ");
  Serial.println(robotConfig.ssid);
  Serial.print("Password: ");
  Serial.println(robotConfig.password);
  Serial.print("Mode: ");
  if (robotConfig.apMode) {
    Serial.println("Access Point");
  }
  else {
    Serial.println("Station");
  }
  Serial.print("Local Network Name: ");
  Serial.print(robotConfig.localName);
  Serial.println(".local");
   
  if (robotConfig.apMode) {
    WiFi.disconnect();
    WiFi.mode( WIFI_AP );
    WiFi.softAP(robotConfig.ssid, robotConfig.password);
  } 
  else {
    if (WiFi.status() == WL_CONNECTED) {
      WiFi.disconnect();
    }
    Serial.print("Connecting");
    WiFi.begin(robotConfig.ssid, robotConfig.password);
        while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());  
  }
  

  if (MDNS.begin(robotConfig.localName)) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/left", handleLeft);
  server.on("/right", handleRight);
  server.on("/forward", handleForward);
  server.on("/reverse", handleReverse);  
  server.on("/stop", handleStop);
  server.on("/auto", handleAuto);
  server.on("/remote", handleRemote);

  server.on("/config", handleConfig);
  server.on("/save", handleSave);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");

  ir.setup();
  motor.setup();

}
  
 
void loop() {
  static unsigned long timeoutTimer = 0;
  static unsigned long delayTimer = 0;
  uint32_t irCode;
  static unsigned long motionTimeout = Timeout;

  server.handleClient();

  if (digitalRead(ClearDefaultsPin) == LOW) {
    Serial.println("Setting default robot configuration");
    setDefaults();
    ir.irTxIntDisable();
    preferences.begin("2020bot", false);
    preferences.putBytes("robotConfig", (unsigned char *)&robotConfig, sizeof(robotConfig));
    preferences.end();
    ir.irTxIntEnable();
        
    delay(1000);
    ESP.restart();
  }

  if (webCommand != WebNone) {
    switch (webCommand) {
      case WebAuto:
        robotAction = None;
        robotMode = Autonomous;
        break;
      case WebRemote:
        robotAction = Stop;
        robotMode = Remote;
        motionTimeout = Timeout;
        break;
      case WebForward:
        robotAction = Forward;
        motionTimeout = Timeout;
        break;
      case WebReverse:
        robotAction = Backward;
        motionTimeout = Timeout;
        break;
      case WebLeft:
        robotAction = Left;
        motionTimeout = 100;
        break;
      case WebRight:
        robotAction = Right;
        motionTimeout = 100;
        break;
      case WebStop:
        robotAction = Stop;
        motionTimeout = Timeout;
        break;
      default:
        robotAction = None;
        break;
    }
    webCommand = WebNone;
    timeoutTimer = millis() + motionTimeout;
  }

  if (ir.rxDataReady()) {

    irCode = ir.rxData();
    
    if (irCode != Remote_no_code) {
      Serial.print("Received: 0x");
      Serial.println(irCode, HEX);
    }

    switch (irCode) {
      case Remote_play:
      case Remote_hash:
        robotAction = None;
        robotMode = Autonomous;
        break;
      case Remote_prev:
      case Remote_asterisk:
        robotAction = Stop;
        robotMode = Remote;
        motionTimeout = Timeout;
        break;
      case Remote_2:
      case Remote_up_arrow:
        robotAction = Forward;
        motionTimeout = Timeout;
        break;
      case Remote_8:
      case Remote_down_arrow:
        robotAction = Backward;
        motionTimeout = Timeout;
        break;
      case Remote_4:
      case Remote_left_arrow:
        robotAction = Left;
        motionTimeout = 100;
        break;
      case Remote_6:
      case Remote_right_arrow:
        robotAction = Right;
        motionTimeout = 100;
        break;
      case Remote_5:
      case Remote_ok:
        robotAction = Stop;
        motionTimeout = Timeout;
        break;
      case Remote_vol_plus:
      case Remote_3:
        irFrequency += IrStep;
        irFrequency = constrain(irFrequency, IrMin, IrMax);
        break;
      case Remote_vol_minus:
      case Remote_1:
        irFrequency -= IrStep;
        irFrequency = constrain(irFrequency, IrMin, IrMax);
        break;      
      case Remote_9:
        motorOffset += MotorOffsetStep;
        motorOffset = constrain(motorOffset, MotorOffsetMin, MotorOffsetMax);
        motor.motorOffset(motorOffset);
        break;
      case Remote_7:
        motorOffset -= MotorOffsetStep;
        motorOffset = constrain(motorOffset, MotorOffsetMin, MotorOffsetMax);
        motor.motorOffset(motorOffset);
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
          delayTimer = millis() + 100;
          break;
        case Right:
          motor.right(speed);
          delayTimer = millis() + 100;
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
 
 
