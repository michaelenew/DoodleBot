// Read printer positions from text file then move the printer accordingly

#include <ArduinoSTL.h>
#include "pinset.h"
#include "params.h"
#include "conversions.h"
#include "sched.h"
#include "initialize.h"
#include "state.h"


using namespace std;

// Step position of motors relative to datum
vector<long> gtickpos = {0,0,0,thpsteps / 4,0};
// Position vector to be received from the host computer
vector<float> pos(5);
vector<float> prevpos = {0,0,0,0.25,0}; // starting position of printer
unsigned long movetime; // time 
int penState = 0, prevPenState = 0;
float rot_offset = 0; // rotational offset of base
bool penflip = 0;
char op; // s to run state, p to run pingpong, o to run origin

// This determines the communication speed of the Arduino Mega 2560 Controller in bits per second
#define BAUDRATE 250000
//if 250000 does not work try: #define BAUDRATE 115200

int pot_pin = 3;
float a = 0, a_adj = 0;
int sumOld = 0, sumNew = 0;



void setup() {
// Prepare for serial input
  Serial.begin(BAUDRATE);
 
// Set up all pins
  pinSetup(); // Set up pins, in setup.cpp

// Communicate ready to receive input from host computer
  Serial.print('r');

  while (Serial.available() == 0) {
  }
  
  if(Serial.available() > 0) {
    char cont = Serial.read();
// ________________ Sensor testing __________________
    if (cont == 's') {
      Serial.println("Testing Sensors...");
      delay(2000);
      while (1) {
        state();
      }
    }
// _______________ End sensor testing _______________

    else if (cont == 'd') {
    }
    else { // If we are setting origin or printing, read in printer parameters
      Serial.print('r');
      while (Serial.available() == 0) {} // wait for the parameters to be written
      tip_offset = Serial.readStringUntil(',').toFloat();
      angularconv = Serial.readStringUntil(',').toFloat();
      arcthreshold = Serial.readStringUntil(',').toFloat();
      xsteps = Serial.readStringUntil(',').toInt();
      ysteps = Serial.readStringUntil(',').toInt();
      zsteps = Serial.readStringUntil(',').toInt();
      thpsteps = Serial.readStringUntil(',').toInt();
      thbsteps = Serial.readStringUntil(',').toInt();
      xrot2mm = Serial.readStringUntil(',').toFloat();
      yrot2mm = Serial.readStringUntil(',').toFloat();
      zrot2mm = Serial.readStringUntil(',').toFloat();
      thprot2rot = Serial.readStringUntil(',').toFloat();
      thbrot2rot = Serial.readStringUntil(',').toFloat();
      xmm2orig = Serial.readStringUntil(',').toFloat();
      ymm2orig = Serial.readStringUntil(',').toFloat();
      zmm2orig = Serial.readStringUntil(',').toFloat();
      thp_orig_potval = Serial.readStringUntil(',').toFloat();
    }
  }

  while (Serial.available() > 0) {
    Serial.read();
  }
  Serial.flush();

  Serial.print('r');

  while (Serial.available() == 0) {}
  
  op = Serial.read();

  if (op != 'o' && op != 'p') {
    Serial.println("ERROR: Invalid operation character received");
    while (1) {} // Halt
  }


// ___________________ If it is desired to set the origin ___________________
  if (op == 'o') {
    // RUN CODE TO DETERMINE ORIGIN, THEN PACKAGE VALUES AND SEND BACK TO COMPUTER
    vector<float> origvals = setorigin();
    for (int i = 0; i < origvals.size(); i++) {
      Serial.println(origvals[i]);
    }
    
    while (1) {} // Now halt
  }


// ___________________ If it is desired to execute a print ___________________
  if (op == 'p') {
    // Move to origin
    origin();
  }
}


// ___________________ loop WILL ONLY BE REACHED IF op == 'p' (if we are printing) ___________________
void loop () {
// Tell computer we are ready to receive input
  Serial.print('r');
  
// Do nothing until you receive serial input:
  while(Serial.available()==0) {}
  
// Getting the points from the text file:
  for(int i = 0; i < 5; i++) { // loop through number of points to be read
    if(Serial.available() > 0) {
      pos[i] = Serial.readStringUntil(',').toFloat();
    }
  }
  movetime = long(Serial.readStringUntil(',').toInt());
  penState = Serial.readStringUntil(',').toInt();
  
  // Clear serial port
  while(Serial.available() > 0) { // Clear rows and columns
    Serial.read();
  }
  Serial.flush();

  //Press Button when state change is detected
  if(penState != prevPenState){
      ButtonPresser();
      penState = prevPenState;
      //Serial.println("Button Pushed");
  }



  pos[3] /= (float) angularconv; // convert from rad/deg to rotations
  pos[4] /= (float) angularconv;

  if (penflip) {
    pos[3] = 0.5 - pos[3];
  }
  pos[4] += rot_offset;

  vector<float> check = pos;
  inv_kin(check);
  
  if (check[1] < 0) {
    pos[3] = 0.5 - pos[3];
    penflip = not(penflip);
    if (check[0] < 0) {
      pos[4] -= 0.5;
      rot_offset -= 0.5;
      
    }
    else {
      pos[4] += 0.5;
      rot_offset += 0.5;
    }
  }

  int numiter;
  if (abs(pos[3] - prevpos[3]) > abs(pos[4] - prevpos[4])) {
    numiter = abs(pos[3] - prevpos[3]) * 360.0 / arcthreshold + 1;
  }
  else {
    numiter = abs(pos[4] - prevpos[4]) * 360.0 / arcthreshold + 1;
  }

  for (int i = 0; i < numiter; i++) {
    
    vector<float> iterpos(5);
    for (int j = 0; j < 5; j++) {
      iterpos[j] = prevpos[j] + (pos[j] - prevpos[j]) * (float)(i + 1) / (float)numiter;
    }

    inv_kin(iterpos);
    ppos2mrot(iterpos); // find corresponding motor rotations
    vector<long> itermove = gtick2ltick(mrot2gtick(iterpos)); // Find steps we wish to move

    long maxval = 0;
    for (int i = 0; i < 5; i++) {
      if (abs(itermove[i]) > maxval) {
        maxval = abs(itermove[i]);
      }
    }

    unsigned long itertime = movetime / numiter + 1;
    
    if (maxval > itertime * 3) {
      itertime = maxval * 3;
    }

    schedule(itermove, itertime); // move to desired position, gtick2ltick in conversions.cpp
  }
  
  prevpos = pos;

}
