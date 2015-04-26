#include "DualVNH5019MotorShield.h"

// Automated Bubble Machine
// Alex Shure 2015

//  Pin mapping
//  _INA1 = 2;
//  _INB1 = 4;
//  _EN1DIAG1 = 6;
//  _PWM1 = 9; //fixed to timer
//  _CS1 = A0;

//  limit switch = 3

//  _INA2 = 7;
//  _INB2 = 8;
//  _EN2DIAG2 = 12;
//  _CS2 = A1;

DualVNH5019MotorShield md;

int speedHome = 100;
int speedDown = -250;
int speedUp = 250;
int ramp = 30; // ramp time at acceleration and deceleration
unsigned long travelDown = 3000;
unsigned long travelUp = travelDown;
unsigned long timeDown = 1000; //time to spend at the bottom
unsigned long timeUp = 1000; //time to spend at the top

int curSpeed = 0;
int limit = 0; // global for limit switch, 1 if triggered
int maxCurrent = 6000; // maximum current in mA
int motorSpeed = 0;

void pCurrent () {
  Serial.print("Current: ");
  Serial.println(md.getM1CurrentMilliamps());
}

void travel (long time) {
  Serial.println("Traveling");
  for (long i = 0; i <= time; i++) //travel for the given time
  {
    checkStop(); // check if we hit the end switch during the time we travel.
    if (limit == 0) { //if switch not triggered, travel further in 1ms increments, otherwise do nothing
      delay(1);
    }
    if (limit == 1) { 
      Serial.println("! Endstop while travelling.");
      return;
    }
  }
  
}

void checkStop()
{
  if (md.getM1Fault()) //check if the motor driver is sending an overcurrent, overvoltage, undervoltage, thermal or short error.
  {
    Serial.print("Motor fault occured, waiting 10s");
    for (int i = 0; i <= 10; i++) {
      Serial.print(".");
      delay (1000);
      md.setM1Speed(0);
      curSpeed = i;
    }
    Serial.println(".");
  }

  if (limit == 1)  //if limit switch has triggered before, check if it is released now
  {
    if (digitalRead(3) == 0) // if limit switch released
    {
      limit = 0;
    }
  }

  if (limit == 0) { //if limit switch was not triggered before, check if it is triggered now
    if (digitalRead(3) == HIGH) // if limit switch triggered, ie pulled to GND
    {
      Serial.println("Limit switch triggered.");
      limit = 1;
      md.setM1Speed(0);
      curSpeed = 0;
      digitalWrite(13, 1);
      delay(30);
      return;
    }
  }

  if (md.getM1CurrentMilliamps() > maxCurrent) {
    //md.setM1Speed(50);
    Serial.print("Exceeding maxCurrent, ");
    printCurrent();
  }
}

void printCurrent() {
  Serial.print("Motor current: ");
  Serial.println(md.getM1CurrentMilliamps());
}

void setVel(int s) {
  md.setM1Speed(s);
  curSpeed = s;
}
void setup()
{
  pinMode(3, INPUT);
  digitalWrite(3, HIGH);
  Serial.begin(115200);
  Serial.println("Controller for Automated Bubble Machine. CC0 2015");
  md.init(); //initialize motor driver instance
  printCurrent();
  // start to ride down a little bit when the machine is powered up, just to make sure that we are not past the top end switch.

  if (limit == 1) Serial.println("Limit switch active during startup.");
  if (limit == 0) {
    Serial.println("Limit switch inactive, accelerating upwards.");
    for (int i = 0; i <= speedHome; i++) //accelerating
    {
      if (limit == 0) { //if switch not triggered, accelerate further, otherwise do nothing
        setVel(i);
        checkStop();
        delay(10);
      }
    }

    Serial.println("Searching for top limit.");

    while (limit == 0) { //if switch not triggered, pull rope in further
      checkStop();
      delay(1);
    }
    Serial.println("Top reached.");
  }
}

void loop()
{
  for (int i = 0; i >= speedDown; i--) //accelerating downwards
  {
    setVel(i);
    checkStop();
    delay(5);
  }

  travel(travelDown); //travelling down for the predefined amount of time

  Serial.println("Decelerating to bottom stop.");

  for (int i = curSpeed; i <= 0; i++) //decelerating, coming to a halt at the bottom
  {
    setVel(i);
    checkStop();
    if (i % 50 == 0) printCurrent();
    delay(ramp);
  }

  setVel(0); // just for good measure
  Serial.println("Wait at bottom.");
  delay(timeDown); //waiting at the bottom
  
  Serial.println("Accelerating up.");
  
  for (int i = 0; i <= speedUp; i++) //curSpeed should be 0, accelerate up
  {
    setVel(i);
    checkStop();
    if (limit==1) break; // if limit switch was hit, break out of the for loop
    if (i % 50 == 0) printCurrent();
    delay(ramp);
  }
  
  Serial.println("Travelling up.");
  travel(travelUp); //travelling up for the predefined amount of time
  delay(timeUp); //waiting at the bottom
  Serial.println("Decelerating up.");
  for (int i = curSpeed; i >= 0; i--) //decelerating
  {
    setVel(i);
    checkStop();
    if (limit==1) break; // if limit switch was hit, break out of the for loop
    if (i % 50 == 0) printCurrent();
    delay(ramp);
  }
  
  setVel(0); // just for good measure
  Serial.println("Wait at top.");
  delay(timeUp); //waiting at the bottom
  

}
