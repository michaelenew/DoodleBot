#include "pinset.h"

extern int pot_pin;
extern float a, a_adj;
extern int sumOld, sumNew;



float ConditionAnalog(int cfactor, int pin){
//conditions analog signal
  float readings[cfactor];
  int readIndex = 0;
  float total = 0;
  for (int thisReading = 0; thisReading < cfactor; thisReading++){
    readings[thisReading] = 0;
  }
  for(int i = 0; i < cfactor; i++){
    //total = total - readings[i];
    readings[i] = analogRead(pin);
    total = total + readings[i];
  }

    float average = total/(float)cfactor;
    return average;
}



void state() {

  int z_top = digitalRead(Z_MAX_PIN);
  int z_bot = digitalRead(Z_MIN_PIN);


  a = ConditionAnalog(50, pot_pin);
  a_adj = (a)*360/1023;

  
  int  x_right = digitalRead(X_MIN_PIN);
  int  x_left = digitalRead(X_MAX_PIN);


  int  y_back = digitalRead(Y_MIN_PIN);
  int  y_front = digitalRead(Y_MAX_PIN);
  sumNew = x_right + x_left + y_back + y_front + z_top + z_bot + a_adj;

  if (sumNew != sumOld){
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println("Left X Endstop:");
    Serial.println(x_left);
    Serial.println("Right X Endstop:");
    Serial.println(x_right);
    Serial.println("Front Y Endstop:");
    Serial.println(y_front);
    Serial.println("Back Y Endstop:");
    Serial.println(y_back);
    Serial.println("Top Z Endstop:");
    Serial.println(z_top);
    Serial.println("Bottom Z Endstop:");
    Serial.println(z_bot);
    Serial.println("Pen Angle:");
    Serial.println(a_adj);
    delay(100);
    sumOld = sumNew;
  }
}

