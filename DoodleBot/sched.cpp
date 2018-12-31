#include "Arduino.h"
#include "pinset.h"
#include <ArduinoSTL.h>
#include "conversions.h"
#include <Servo.h>

using namespace std;

extern vector<long> gtickpos;

// Declare the Servo pin 
int servoPin = 63; 
// Create a servo object 
Servo Servo1; 

void schedule(vector<long>& ticks, unsigned long& msecs);

void ButtonPresser(){
 //Press Button on Printing Pen
   Servo1.attach(servoPin); 
   Servo1.write(75);
   delay(1000);
   Servo1.write(50);
   delay(1000);
   Servo1.write(75);
   delay(1000);
   Servo1.detach();
}

// SCHEDULE moves all motors simultaneously a desired number of steps.
// Takes in the desired motor steps to be completed by all 5 motors and the time in which the
// move should be completed and makes the motors move simultaneously toward the desired position, and arrive
// simultaneously at the desired position.
// INPUTS: ticks[5] is the ticks to move forward for each motor (negative = backwards movement)
//         msecs is the time taken to make the move in milliseconds
void schedule(vector<long>& ticks, unsigned long& msecs) {
	// tickinc is the number of milliseconds between ticks for each motor

  int timeinc = 2;
	float tickinc[5], tickcond[5];
  for(int i = 0; i < 5; i++) {
	  if (abs(ticks[i]) < 0.001) { // special case: motor shouldn't move
      tickcond[i] = msecs + 2 * timeinc; // time at which motor would move is > time that the program will run
      tickinc[i] = 0;
    }
    else{
      tickcond[i] = (float)(msecs) / (float)(abs(ticks[i])); // used as a check for whether the motor should step at a given time step
      tickinc[i] = (float)(msecs) / (float)(abs(ticks[i])); // motor (i) will take 1 step every tickinc[i] milliseconds over msecs milliseconds, for a total of ticks[i] steps
    }
	}
  
  // Set the motor directions; negative ticks value means rotate in the negative direction
  int dir[5] = {1,1,1,1,1}; // Used to increment/decrement global step counter when a motor steps
	if (ticks[0] < 0) {
		digitalWrite(X_DIR_PIN, LOW); // if x value is negative, set x direction to move backward
    dir[0] = -1; // if x value is negative, change x increment to -1 (decrement global step counter when x moves)
	}
  else{
    digitalWrite(X_DIR_PIN, HIGH); // if x value is positive, set x direction to move forward
  }
	if (ticks[1] < 0) {
		digitalWrite(Y_DIR_PIN, LOW);
    dir[1] = -1;
	}
  else{
    digitalWrite(Y_DIR_PIN, HIGH);
  }
	if (ticks[2] < 0) {
		digitalWrite(Z_DIR_PIN, LOW);
    dir[2] = -1;
	}
  else{
    digitalWrite(Z_DIR_PIN, HIGH);
  }
	if (ticks[3] < 0) {
		digitalWrite(THP_DIR_PIN, LOW);
    dir[3] = -1;
	}
  else{
    digitalWrite(THP_DIR_PIN, HIGH);
  }
	if (ticks[4] < 0) {
		digitalWrite(THB_DIR_PIN, LOW);
    dir[4] = -1;
	}
  else{
    digitalWrite(THB_DIR_PIN, HIGH);
  }

  // This loop runs once every (timeinc) milliseconds and tells motors to move at correct timesteps
	unsigned long t = 0;
	while (t <= msecs) {
		t += timeinc; // 2 milliseconds have elapsed since we last ran this loop
		// Send pulse to motors
		if (t >= tickcond[0] && !(!digitalRead(X_MIN_PIN) && (dir[0] == -1)) && !(!digitalRead(X_MAX_PIN) && (dir[0] == 1))) { // if x is not running into itself and the current time elapsed is >= the time that x needed to be moved at, move x
			digitalWrite(X_STEP_PIN, HIGH); // Make x motor move
      digitalWrite(X_STEP_PIN, LOW); // Make x motor move
      tickcond[0] += tickinc[0]; // if x moved, tell x to wait until its next time step
      gtickpos[0] += dir[0]; // if x moved, increment or decrement the global step counter
		}
		if (t >= tickcond[1] && !(!digitalRead(Y_MIN_PIN) && (dir[1] == -1)) && !(!digitalRead(Y_MAX_PIN) && (dir[1] == 1))) {
			digitalWrite(Y_STEP_PIN, HIGH);
      digitalWrite(Y_STEP_PIN, LOW);
      tickcond[1] += tickinc[1];
      gtickpos[1] += dir[1];
		}
		if (t >= tickcond[2] && !(!digitalRead(Z_MAX_PIN) && (dir[2] == -1)) && !(!digitalRead(Z_MIN_PIN) && (dir[2] == 1))) {
			digitalWrite(Z_STEP_PIN, HIGH);
      digitalWrite(Z_STEP_PIN, LOW);
      tickcond[2] += tickinc[2];
      gtickpos[2] += dir[2];
		}
		if (t >= tickcond[3]) {
			digitalWrite(THP_STEP_PIN, HIGH);
      digitalWrite(THP_STEP_PIN, LOW);
      tickcond[3] += tickinc[3];
      gtickpos[3] += dir[3];
		}
		if (t >= tickcond[4]) {
			digitalWrite(THB_STEP_PIN, HIGH);
      digitalWrite(THB_STEP_PIN, LOW);
      tickcond[4] += tickinc[4];
      gtickpos[4] += dir[4];
		}
		
		delay(timeinc);
	}
}
