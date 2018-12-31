// Pin declarations For RAMPS 1.4

#include <Arduino.h> 
#include <string.h> 

// step input controls incremental motor steps
#define X_STEP_PIN         54
// direction input changes direction of motor
#define X_DIR_PIN          55
// enable input Logic Input. Enables the FET functionality within the motor driver. 
// If set to HIGH, the FETs will be disabled, and the IC will not drive the motor. 
// If set to LOW, all FETs will be enabled, allowing motor control.
#define X_ENABLE_PIN       38
// max/min input control microstepping modes
//  full step (0,0), half step (1,0), 1/4 step (0,1), 
// and 1/8 step (1,1 : default). where 1 represents HIGH and 0 represents LOW
#define X_MIN_PIN           3
#define X_MAX_PIN           2

#define Y_STEP_PIN         60
#define Y_DIR_PIN          61
#define Y_ENABLE_PIN       56
#define Y_MIN_PIN          14
#define Y_MAX_PIN          15

#define Z_STEP_PIN         46
#define Z_DIR_PIN          48
#define Z_ENABLE_PIN       62
#define Z_MIN_PIN          18
#define Z_MAX_PIN          19

//Based on ramps 1.4 schematic, Th motor will replace E0
#define THP_STEP_PIN         26
#define THP_DIR_PIN          28
#define THP_ENABLE_PIN       24

//Based on ramps 1.4 schematic, Phi motor will replace E1
#define THB_STEP_PIN         36
#define THB_DIR_PIN          34
#define THB_ENABLE_PIN       30

#define SDPOWER            -1
#define SDSS               53
#define LED_PIN            13

#define PS_ON_PIN          12
#define KILL_PIN           -1
