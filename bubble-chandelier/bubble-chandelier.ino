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

int speedHome = 50;
int speedDown = 200;
int speedUp = 100; 
int ramp = 5; // ramp time at acceleration and deceleration
long travelDown = 3000;
long travelUp = travelDown;

int curSpeed = 0;
int limit = 0; // global for limit switch, 1 if triggered
int maxCurrent = 6000; // maximum current in mA
int motorSpeed = 0;

void pCurrent () {
  Serial.print("Current: ");
  Serial.println(md.getM1CurrentMilliamps());
}

void travel (long time){
  Serial.print("Traveling");
    for (long i = 0; i <= time; i++) //travel for the given time
    {
      checkStop(); // check if we hit the end switch during the time we travel.
      if (limit == 0) { //if switch not triggered, travel further in 1ms increments, otherwise do nothing
        delay(1);
      }
    }
  Serial.println(".");
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
      curSpeed=i;
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
      curSpeed=0;
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

void setup()
{
  pinMode(3,INPUT);
  digitalWrite(3,HIGH);
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
        md.setM1Speed(i);
        curSpeed=i;
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
  for (int i = 0; i <= speedDown; i++) //accelerating downwards
  {
    md.setM1Speed(i);
    checkStop();
    delay(5);
    curSpeed=i;
  }
  
  travel(travelDown); //travelling down for the predefined amount of time
  
  Serial.println("Decelerating to bottom stop."); 
  
  for (int i = curSpeed; i >= 0; i--) //decelerating, coming to a halt at the bottom
  {
    md.setM1Speed(i);
    checkStop();
    if (i % 50 == 0) printCurrent();
    delay(ramp);
  }

  for (int i = -400; i <= 0; i++)
  {
    md.setM1Speed(i);
    checkStop();
    if (i % 200 == 100)
    {
      Serial.print("M1 current: ");
      Serial.println(md.getM1CurrentMilliamps());
    }
    delay(2);
  }


}
