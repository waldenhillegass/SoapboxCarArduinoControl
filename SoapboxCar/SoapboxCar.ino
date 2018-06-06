/*
 * Written by Walden Hillegass
 * June 5, 2018
 * 
 * This program takes potentiometer data and turns it into PWM signals which can be interpreted by a SPARK or TALON SR motorcontroller. 
 * NOT TESTED YET.
 */

#include <Servo.h>

//PIN going to signal pin of motor controller
#define PWM_PIN 6
//Pin for potentiometer
#define POT_PIN 0

//LED will be used to tell user they need to zero throttle
#define LED_PIN 13
#define REV_BUTTON 7

//Will test potentiometer in seperate tester program to get these values
#define POT_MIN 0
#define POT_MAX 1023


//I have a feeling reverse mode might be finicky so here is a debug variable
bool reversible = true;

bool inReverse = false;
float throttle;

//Hijacked the servo library to run motor controllers
Servo Controller;

void setup() {
  //Serial will be used for trouble shooting.
  Serial.begin(9600);
  
  pinMode(REV_BUTTON, INPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  Controller.attach(PWM_PIN);
  digitalWrite(PWM_PIN, LOW);

//Throttle should be zeroed before PWM starts. #safety 
  waitForZeroThrottle();
}


void loop() {
  int PwmHighTime;
  readThrottle();
 
  Serial.println(throttle);
  
  if(inReverse && reversible) {
    
    PwmHighTime = (int)map(throttle, 0, 1, 1500, 1000);
    
    if (digitalRead(REV_BUTTON) == LOW)
    {
      if (isZeroThrottle) {
        digitalWrite(LED_PIN, LOW);
        inReverse = false;
        //Must have throttle zeroed to switch direction. Doing otherwise will suck.
      } else {
        digitalWrite(LED_PIN, HIGH);
      }
      
    }
  } else {
    //is not in reverse
     PwmHighTime = (int) map(throttle, 0, 1, 1500, 2000);

     if (digitalRead(REV_BUTTON) == HIGH)
     {
      if (isZeroThrottle) {
        digitalWrite(LED_PIN, LOW);
        inReverse = false;
        //Must have throttle zeroed to switch direction. Doing otherwise will suck.
      } else {
        digitalWrite(LED_PIN, HIGH);
      }
    }

  }
 Controller.writeMicroseconds(PwmHighTime);
}

float readThrottle(){
  int potRead = analogRead(POT_PIN);
  throttle = (float) map(potRead, POT_MIN, POT_MAX, 0, 1);
  return throttle;
  
}

void waitForZeroThrottle(){
  while(!isZeroThrottle())
    {
      digitalWrite(LED_PIN, HIGH);
    //do nothing.
    }
  digitalWrite(LED_PIN, LOW);
}

bool isZeroThrottle(){

  return (throttle < .01);
}

