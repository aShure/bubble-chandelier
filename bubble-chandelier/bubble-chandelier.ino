#include "DualVNH5019MotorShield.h"

// Automated Bubble Machine
// Alex Shure 2015

//  Pin mapping
//  _INA1 = 2;
//  _INB1 = 4;
//  _EN1DIAG1 = 6;
//  _PWM1 = 9; //fixed to timer
//  _CS1 = A0;


//  _INA2 = 7;
//  _INB2 = 8;
//  _EN2DIAG2 = 12;
//  _CS2 = A1;

DualVNH5019MotorShield md;
boolean limit = 0; // global for limit switch, 1 if triggered
int maxCurrent = 5000;
int motorSpeed = 0;
int speedDown = 0;
int speedUp = 0;

void pCurrent () {
  Serial.print("Current: ");
  Serial.println(md.getM1CurrentMilliamps());
}

void travel (long time){
  Serial.println("Traveling.");
    for (int i = 0; i <= time; i++) //accelerating
    {
      checkStop();
      if (limit == 0) { //if switch not triggered, travel further in 1ms increments, otherwise do nothing
        delay(1);
      }
    }
}

void checkStop()
{
  if (md.getM1Fault()) //check if the motor driver is sending an overcurrent, overvoltage, undervoltage, thermal or short error.
  {
    Serial.print("Motor fault occured, waiting 15s");
    for (int i = 0; i <= 15; i++) {
      Serial.print(".");
      delay (1000);
    }
    Serial.println(".");
  }

  if (limit == 0) { //if limit switch was not triggered before, check if it is triggered now
    if (digitalRead(3) == 1) // if limit switch triggered
    {
      Serial.println("Limit switch triggered.");
      limit = 1;
      md.setM1Speed(0);
      digitalWrite(13, 1);
    }
  }

  if (limit == 1)  //if limit switch was triggered before, check if it is released now
  {
    if (digitalRead(3) == 0) // if limit switch released
    {
      limit = 0;
    }
  }
  
  if (md.getM1CurrentMilliamps() > maxCurrent) {
    //md.setM1Speed(50);
    Serial.println("Exceeding maxCurrent: ");
    Serial.println(md.getM1CurrentMilliamps());
  }
  
}

void printCurrent() {
      Serial.print("Motor current: ");
      Serial.println(md.getM1CurrentMilliamps());
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Controller for Automated Bubble Machine. CC0 2015");
  md.init(); //initialize motor driver instance
  printCurrent();
  // start to ride down a little bit when the machine is powered up, just to make sure that we are not past the top end switch.

  if (limit == 1) Serial.println("Limit switch active during startup.");
  if (limit == 0) {
    Serial.println("Limit switch inactive. Seaching for upper limit.");
    for (int i = 0; i <= 100; i++) //accelerating
    {
      if (limit == 0) { //if switch not triggered, accelerate further, otherwise do nothing
        md.setM1Speed(i);
        checkStop();
        delay(2);
      }
    }

    while (limit == 0) { //if switch not triggered, pull rope in further
      checkStop();
      delay(1);
    }
    Serial.println("Top end reached, initiation complete.");
  }
}

void loop()
{
  for (int i = 0; i <= speedDown; i++)
  {
    md.setM1Speed(i);
    checkStop();
    delay(2);
  }
  
  travel(2000);
  
  for (int i = speedDown; i >= -400; i--)
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

  for (int i = 0; i <= 400; i++)
  {
    md.setM2Speed(i);
    checkStop();
    if (i % 200 == 100)
    {
      Serial.print("M2 current: ");
      Serial.println(md.getM2CurrentMilliamps());
    }
    delay(2);
  }

  for (int i = 400; i >= -400; i--)
  {
    md.setM2Speed(i);
    checkStop();
    if (i % 200 == 100)
    {
      Serial.print("M2 current: ");
      Serial.println(md.getM2CurrentMilliamps());
    }
    delay(2);
  }

  for (int i = -400; i <= 0; i++)
  {
    md.setM2Speed(i);
    checkStop();
    if (i % 200 == 100)
    {
      Serial.print("M2 current: ");
      Serial.println(md.getM2CurrentMilliamps());
    }
    delay(2);
  }
}
