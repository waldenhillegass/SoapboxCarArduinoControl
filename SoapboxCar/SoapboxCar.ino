/*
   Written by Walden Hillegass
   June 5, 2018

   This program takes potentiometer data and turns it into PWM signals which can be interpreted by a SPARK or TALON SR motorcontroller.
   NOT TESTED YET.
*/

#include <Servo.h>

//PIN going to signal pin of motor controller
#define PWM_PIN 6
//Pin for potentiometer
#define POT_PIN 0

//LED will be used to tell user they need to zero targetThrottle
#define LED_PIN 13
#define REV_BUTTON 7

//Will test potentiometer in seperate tester program to get these values
double POT_MIN = 0;
double POT_MAX = 1023;

float maxThrottleChangePerSec = .02;

//I have a feeling reverse mode might be finicky so here is a debug variable
bool reversible = false;

bool inReverse = false;
double targetThrottle = 0;
double throttle = 0;

double throttleLastCycle = 0;
long timeLastCycle;

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
  timeLastCycle = millis();
  //targetThrottle should be zeroed before PWM starts. #safety
  //waitForZeroThrottle();
}


void loop() {
  int PwmHighTime;
  
  readTargetThrottle();

  //All of this is for ramping the throttle;
  long timeSinceLastCycle = millis(); - timeLastCycle;
  double maxThrottleChange = (timeSinceLastCycle / 10000) * maxThrottleChangePerSec;
  if (targetThrottle > (throttleLastCycle + maxThrottleChange))
  {
    throttle = throttleLastCycle + maxThrottleChange;
  } 
  else if (targetThrottle < (throttleLastCycle - maxThrottleChange)) 
  {
    throttle = throttleLastCycle - maxThrottleChange;
  }
  else
  {
    throttle = targetThrottle;
  }

  timeLastCycle = millis();
  throttleLastCycle = throttle;

  
  Serial.println(throttle);
  
  if (inReverse && reversible) {

    PwmHighTime = (int)map(throttle, 0, 1, 1500, 1000);

    if (digitalRead(REV_BUTTON) == LOW)
    {
      if (isZeroThrottle) {
        digitalWrite(LED_PIN, LOW);
        inReverse = false;
        //Must have targetThrottle zeroed to switch direction. Doing otherwise will suck.
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
        //Must have targetThrottle zeroed to switch direction. Doing otherwise will suck.
      } else {
        digitalWrite(LED_PIN, HIGH);
      }
    }

  }
  
  Controller.writeMicroseconds(PwmHighTime);
  delay(20);
}

double readTargetThrottle() {
  double potRead = analogRead(POT_PIN);
  targetThrottle = doublemap(potRead, POT_MIN, POT_MAX, 0.00, 1.00);
  if (targetThrottle < .04) {
    targetThrottle = 0;
  }
  if (targetThrottle > 1) {
    targetThrottle = 1;
  }
  
  return targetThrottle;
}

void waitForZeroThrottle() {
  while (readTargetThrottle() != 0)
  {
    Serial.println(readTargetThrottle());
    digitalWrite(LED_PIN, HIGH);
    //do nothing.
  }
  digitalWrite(LED_PIN, LOW);
}

bool isZeroThrottle() {

  return (throttle < .01);
}


double doublemap(double x, double in_min, double in_max, double out_min, double out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}



