/*
  Motion.h - Library for moving robot with stepper motors
  Created by Chengjia Yu, October 5, 2017
  For UNO Robotics Club
*/

#include "Arduino.h"
#include "Motion.h"
#define steps_per_inch 215.6
#define steps_per_degree 26.5

Motion::Motion(){
  DDRL = B11111111;
}

void Motion::testWheels(){
  PORTL = B00000000;
  for(long i=0;i<5000;i++){
    PORTL ^= M1;
    delayMicroseconds(800);
  }
  for(long i=0;i<5000;i++){
    PORTL ^= M2;
    delayMicroseconds(800);
  }
  for(long i=0;i<5000;i++){
    PORTL ^= M3;
    delayMicroseconds(800);
  }
  for(long i=0;i<5000;i++){
    PORTL ^= M4;
    delayMicroseconds(800);
  }
}

void Motion::mov(byte dir, float dist, long del){
  PORTL = dir;
  float stepf = dist * steps_per_inch;
  long steps = stepf;
  for(long i=0;i<steps;i++){
    PORTL ^= allWheels;
    delayMicroseconds(del);
  }
}

void Motion::turn(byte dir, float degree, long del){
  PORTL = dir;
  float stepf = degree * steps_per_degree;
  long steps = stepf;
  for(long i=0;i<steps;i++){
    PORTL ^= allWheels;
    delayMicroseconds(del);
  }
}

void Motion::strafe(byte dir, float dist, long del){
  PORTL = dir;
  float stepf = dist * steps_per_inch;
  long steps = stepf;
  for(long i=0;i<steps;i++){
    PORTL ^= allWheels;
    delayMicroseconds(del);
  }
}

void Motion::movPairs(byte dir, float dist, long del, byte wheels){
  PORTL = dir;
  float stepf = dist * steps_per_inch;
  long steps = stepf;
  for(long i=0;i<steps;i++){
    PORTL ^= wheels;
    delayMicroseconds(del);
  }
}

void Motion::steer(byte dir, float dist, long del, float ratio, byte master, byte slave){
  PORTL = dir;
  float stepf = dist * steps_per_inch;
  long steps = stepf;
  int slavecount = 0, mastercount = 0, stepcount = 0;
  float temp = del * ratio;
  long slavedelay = temp;
  while(stepcount < steps){
    if(mastercount > del){
      PORTL ^= master;
      mastercount = 0;
      stepcount++;
    }
    if(slavecount > slavedelay){
      PORTL ^= slave;
      slavecount = 0;
    }
    mastercount++;
    slavecount++;
  }
}


    
       
    
    