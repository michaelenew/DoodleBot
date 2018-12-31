// inverse kinematics, printer positions to motor rotations, motor rotations to ticks, global ticks to local ticks

#include "arduino.h"
#include <ArduinoSTL.h>

using namespace std;

// Declare variables contained in params.h (defined in INO file as global variables)
extern long xsteps, ysteps, zsteps, thbsteps, thpsteps;
extern float tip_offset, xrot2mm, yrot2mm, zrot2mm, thbrot2rot, thprot2rot;
extern vector<long> gtickpos; // Global step count defined in INO file



// INVERSE KINEMATICS takes in coordinates from the printed object's frame and returns
//                    coordinates in the printer's frame
void inv_kin(vector<float>& points) {
  
  float x, y, z, thy, thz;
  
  x = points[0]; // Store local x y and z values
  y = points[1];
  z = points[2];
  thy = points[3] * 2 * 3.1415; // convert local rotations to radians for use in cos/sin
  thz = points[4] * 2 * 3.1415; // convert local rotations to radians for use in cos/sin

  // Inverse kinematics relationships
  points[0] = - x * sin(thz) + y * cos(thz);
  points[1] = + x * cos(thz) + y * sin(thz) + tip_offset * cos(thy); // *cos(thy);
  points[2] = -z + tip_offset * (1 - sin(thy)); //-locpoints[i][2] + tip_offset * (1 - sin(thy));
  // points[3] stays the same
  points[4] *= -1;
}



// Takes a matrix of printer positions and finds the corresponding motor rotations
void ppos2mrot(vector<float>& ppos) {
  
  ppos[0] /= xrot2mm; // divide by rotations/mm for the x direction
	ppos[1] /= yrot2mm;
	ppos[2] /= zrot2mm;
	ppos[3] /= thprot2rot;
	ppos[4] /= thbrot2rot;
}



// Takes a matrix motor rotations and converts them to motor steps
vector<long> mrot2gtick(vector<float> mrot) {
  
	vector<long> gtick(5);
	gtick[0] = (int)(mrot[0] * (float)xsteps);
	gtick[1] = (int)(mrot[1] * (float)ysteps);
	gtick[2] = (int)(mrot[2] * (float)zsteps);
	gtick[3] = (int)(mrot[3] * (float)thpsteps);
	gtick[4] = (int)(mrot[4] * (float)thbsteps);
	
	return gtick;
}

// Takes motor step positions relative to the datum and converts them to motor commands 
// (e.g. takes move from 190 to 200 into move forward 10 steps)
// Called in sched.cpp > schedule()
vector<long> gtick2ltick(vector<long> gtick) {
  
	vector<long> ltick(5);
	for (int i = 0; i < 5; i++) {
	  ltick[i] = gtick[i] - gtickpos[i]; // Desired step position - current step position
	}
	
	return ltick;
}
