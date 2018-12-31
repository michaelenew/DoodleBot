float tip_offset = 85; // Change to be the distance in mm between pen axis of rotation and pen tip

float angularconv = 360; // if inputting angular pos in (deg, rad, rotations) use (360, 2pi, 1)

float arcthreshold = 1; // threshold in degrees of how far the pen/base can rotate in one motion

long xsteps = 800; // x motor steps per rotation
long ysteps = 400;
long zsteps = 200;
long thpsteps = 3200;
long thbsteps = 3200;

float xrot2mm = 39.98; // mm/rotation on x axis, measured from printer directly
float yrot2mm = 41.7;
float zrot2mm = 0.8;
float thprot2rot = 1; // pen rotation/motor rotation
float thbrot2rot = 1;

float xmm2orig = 94.5;
float ymm2orig = 15.53;
float zmm2orig = 63.2;

float thp_orig_potval = 185;
