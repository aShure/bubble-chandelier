void accelerateDown() {
  Serial.println("Accelerating Downwards.");
  for (int i = 0; i >= speedDown; i--) //accelerating downwards
  {
    setVel(i);
    checkStop();
    delay(5);
  }
}

void decelerateDown() {
  Serial.println("Decelerating to bottom stop.");
  for (int i = curSpeed; i <= 0; i++) //decelerating, coming to a halt at the bottom
  {
    setVel(i);
    checkStop();
    if (i % 50 == 0) printCurrent();
    delay(ramp); // slower ramp when coming to a bottom halt
  }
}

void accelerateUp() {
  Serial.println("Accelerating up.");
  for (int i = 0; i <= speedUp; i++) //curSpeed should be 0, accelerate up
  {
    setVel(i);
    checkStop();
    if (limit == 1) break; // if limit switch was hit, break out of the for loop and return
    if (i % 50 == 0) printCurrent();
    delay(ramp);
  }
}

void decelerateUp() {
  Serial.println("Decelerating up.");
  for (int i = curSpeed; i >= 0; i--) //decelerating
  {
    setVel(i);
    checkStop();
    if (limit == 1) break; // if limit switch was hit, break out of the for loop
    if (i % 50 == 0) printCurrent();
    delay(ramp); //slower ramp when coming to a top halt.
  }
}
