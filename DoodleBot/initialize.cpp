#include "pinset.h"
#include "conversions.h"
#include "sched.h"
#include <ArduinoSTL.h>
#include "state.h"

using namespace std;

extern float xmm2orig, ymm2orig, zmm2orig, tip_offset, thp_orig_potval;
extern long xsteps, ysteps, zsteps, thpsteps;
extern vector<long> gtickpos;
extern float xrot2mm, yrot2mm, zrot2mm, thprot2rot;

extern int pot_pin;
extern float a, a_adj;


void pinSetup() {
  pinMode(X_STEP_PIN  , OUTPUT);
  pinMode(X_DIR_PIN    , OUTPUT);
  pinMode(X_ENABLE_PIN    , OUTPUT);
  
  pinMode(Y_STEP_PIN  , OUTPUT);
  pinMode(Y_DIR_PIN    , OUTPUT);
  pinMode(Y_ENABLE_PIN    , OUTPUT);
  
  pinMode(Z_STEP_PIN  , OUTPUT);
  pinMode(Z_DIR_PIN    , OUTPUT);
  pinMode(Z_ENABLE_PIN    , OUTPUT);
  
  pinMode(THB_STEP_PIN  , OUTPUT);
  pinMode(THB_DIR_PIN    , OUTPUT);
  pinMode(THB_ENABLE_PIN    , OUTPUT);
  
  pinMode(THP_STEP_PIN  , OUTPUT);
  pinMode(THP_DIR_PIN    , OUTPUT);
  pinMode(THP_ENABLE_PIN    , OUTPUT);
  
  digitalWrite(X_ENABLE_PIN    , LOW);
  digitalWrite(Y_ENABLE_PIN    , LOW);
  digitalWrite(Z_ENABLE_PIN    , LOW);
  digitalWrite(THB_ENABLE_PIN    , LOW);
  digitalWrite(THP_ENABLE_PIN    , LOW);
  
  pinMode(13, OUTPUT);
}


void origin() {

  // Move z to the endstop
  digitalWrite(Z_DIR_PIN, LOW);
  while(digitalRead(Z_MAX_PIN)){
    digitalWrite(Z_STEP_PIN, HIGH);
    digitalWrite(Z_STEP_PIN, LOW);
    delay(3);
  }

  //Set pen to 0 degree position
  a = ConditionAnalog(50, pot_pin);
  a_adj = (a)*360/1023;
  //always turn counterclockwise
  digitalWrite(THP_DIR_PIN, LOW);
    
  
  while(a_adj > thp_orig_potval + 10 || a_adj < thp_orig_potval + 5){

    digitalWrite(THP_STEP_PIN, HIGH);
    digitalWrite(THP_STEP_PIN, LOW);
    delay(2);
	  a = ConditionAnalog(50, pot_pin);
    a_adj = (a)*360/1023;
  }
  while(a_adj > thp_orig_potval + 0.5 || a_adj < thp_orig_potval - 0.5){

    digitalWrite(THP_STEP_PIN, HIGH);
    //delay(1);
    digitalWrite(THP_STEP_PIN, LOW);
    delay(50);
	  a = ConditionAnalog(50, pot_pin);
    a_adj = (a)*360/1023;
  }

  
  // Move x to the endstop
  digitalWrite(X_DIR_PIN, LOW);
  while(digitalRead(X_MIN_PIN)){
    digitalWrite(X_STEP_PIN, HIGH);
    digitalWrite(X_STEP_PIN, LOW);
    delay(4);
  }

  // Move y to the endstop
  digitalWrite(Y_DIR_PIN, LOW);
  while(digitalRead(Y_MIN_PIN)){
    digitalWrite(Y_STEP_PIN, HIGH);
    digitalWrite(Y_STEP_PIN, LOW);
    delay(4);
  }

  // Move to the origin
  int n = 3;
  for (int i = 0; i < n; i++) {

    vector<float> orig = {xmm2orig / (float)n, ymm2orig / (float)n, zmm2orig / (float)n, 0.25 / (float)n, 0};
    ppos2mrot(orig); // convert from printer position to motor rotations
    vector<long> tickorig = mrot2gtick(orig); // convert from motor rotations to step count
    
    unsigned long maxval = 0;
    for (int i = 0; i < 5; i++) {
      if (abs(tickorig[i]) > maxval) {
        maxval = abs(tickorig[i]);
      }
    }
    
    unsigned long t = maxval * 2;
    schedule(tickorig, t);// move that thing, from sched.cpp
    }

  /*vector<float> origtemp = {tip_offset, 0, tip_offset, 0, 0,};
  inv_kin(origtemp);
  ppos2mrot(origtemp);
  gtickpos = mrot2gtick(origtemp);*/
  gtickpos = {0,0,0,thpsteps / 4,0};
}




vector<float> setorigin() {

  vector<float> orig = {0,0,0};
  unsigned long int steps = 0;
  
  // Move z to the endstop and track how many steps it takes
  digitalWrite(Z_DIR_PIN, LOW);
  while(digitalRead(Z_MAX_PIN)) {
    digitalWrite(Z_STEP_PIN, HIGH);
    digitalWrite(Z_STEP_PIN, LOW);
    delay(3);
    steps++;
  }

  orig[2] = (float)steps / (float)zsteps * zrot2mm;
  
  // Move x to the endstop
  steps = 0;
  digitalWrite(X_DIR_PIN, LOW);
  while(digitalRead(X_MIN_PIN)) {
    digitalWrite(X_STEP_PIN, HIGH);
    digitalWrite(X_STEP_PIN, LOW);
    delay(4);
    steps++;
  }

  orig[0] = (float)steps / (float)xsteps * xrot2mm;

  // Move y to the endstop
  steps = 0;
  digitalWrite(Y_DIR_PIN, LOW);
  while(digitalRead(Y_MIN_PIN)) {
    digitalWrite(Y_STEP_PIN, HIGH);
    digitalWrite(Y_STEP_PIN, LOW);
    delay(4);
    steps++;
  }

  orig[1] = (float)steps / (float)ysteps * yrot2mm;

  return orig;
}

