//#include <EEPROM.h>   // EEPROM library
#include <Joystick.h>

#define LED 13
#define STEERING A2
#define RPM 2
#define BTN_1 7
#define BTN_2 8
#define BTN_3 9
#define BTN_4 10
#define BTN_5 11
#define BTN_6 12
#define BTN_7 13

// Create Joystick
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, 
  JOYSTICK_TYPE_JOYSTICK, 7, 0,
  false, false, false, false, false, false,
  false, true, false, false, true);

unsigned int min_speed = 0;  // minimal speed to display on scale, km/h
unsigned int max_speed = 80; // maximum speed to display on scale, km/h

// Set to true to test "Auto Send" mode or false to test "Manual Send" mode.
//const bool testAutoSendMode = true;
const bool testAutoSendMode = false;

volatile unsigned long lastturn, time_press;
volatile float curSpeed;
volatile float distance = 0;
volatile boolean eeprom_flag;
float w_length = 2.050;
boolean flag;
boolean state, button;
int16_t pos = 0;

void sens(void) {
  if (millis() - lastturn > 80) {    // simple noise cut filter (based on fact that you will not be ride your bike more than 120 km/h =)
    curSpeed = w_length / ((float)(millis() - lastturn) / 1000)*3.6;   // calculate speed
    lastturn = millis();                                              // remember time of last revolution
    distance = distance + w_length / 1000;                                    // calculate distance
  }
  eeprom_flag = 1;
}

void setup() {
  // put your setup code here, to run once:
  pinMode (LED, OUTPUT);
  pinMode (STEERING, INPUT);
  pinMode (RPM, INPUT_PULLUP);
  pinMode (BTN_1, INPUT_PULLUP);
  pinMode (BTN_2, INPUT_PULLUP);
  pinMode (BTN_3, INPUT_PULLUP);
  pinMode (BTN_4, INPUT_PULLUP);
  pinMode (BTN_5, INPUT_PULLUP);
  pinMode (BTN_6, INPUT_PULLUP);
  pinMode (BTN_7, INPUT_PULLUP);
  
  // Initialize Joystick Library
  Joystick.begin(testAutoSendMode);
  
  //Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(RPM), sens, FALLING);  // hall sensor interrupt  
}

void loop() {
  // put your main code here, to run repeatedly:
  // convert speed value to servo angle
  pos = map(curSpeed, min_speed, max_speed, 0, 1023);


  if ((millis() - lastturn) > 1000) {       // if there is no signal more than 2 seconds
    curSpeed = 0;                              // so, speed is 0
    if (eeprom_flag) {                      // if eeprom flag is true
      //EEPROM.write(0, (float)DIST * 10.0);  // write ODO in EEPROM
      eeprom_flag = 0;                      // flag down. To prevent rewritind
    }
  }

  Joystick.setSteering(map(analogRead(STEERING), 40, 685, 0, 1023));
  Joystick.setThrottle(pos);
  Joystick.setButton(0, !digitalRead(BTN_1));
  Joystick.setButton(1, !digitalRead(BTN_2));
  Joystick.setButton(2, !digitalRead(BTN_3));
  Joystick.setButton(3, !digitalRead(BTN_4));
  Joystick.setButton(4, !digitalRead(BTN_5));
  Joystick.setButton(5, !digitalRead(BTN_6));
  Joystick.setButton(6, !digitalRead(BTN_7));
  Joystick.sendState();
  //Serial.println("Steering " + String(map(analogRead(STEERING), 0, 671, 0, 1023)) + ", RPM " + String(digitalRead(RPM)) + ", Speed " + String(curSpeed) + ", pos " + String(pos));
  //Serial.println("Steering " + String(map(analogRead(STEERING), 40, 685, 0, 1023)) + ", Speed " + String(curSpeed) + ", pos " + String(pos));
  //Serial.println("Speed " + String(curSpeed) + ", pos " + String(pos));
  delay(50);
}
