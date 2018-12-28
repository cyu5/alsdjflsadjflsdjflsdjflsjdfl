/*
  Motion.h - Library for moving robot with stepper motors
  Created by Chengjia Yu, October 5, 2017
  For UNO Robotics Club
*/

#ifndef Motion_h
#define Motion_h

#include "Arduino.h"

class Motion {
  public:
    Motion();
    
    byte D1=B10000000;          //left-front backward
    byte D2=B00100000;          //right-front backward
    byte D3=B00000000;          //left-back backward
    byte D4=B00000010;          //right-back backward
    byte M1=B01000000;
    byte M2=B00010000;
    byte M3=B00000100;
    byte M4=B00000001;
    byte allWheels = B01010101;
    byte leftWheels = B01000100;
    byte rightWheels = B00010001;
    byte frontWheels = B01010000;
    byte rearWheels = B00000101;
    byte rightDiagonal = B00010100;
    byte leftDiagonal = B01000001;
    byte faceFront = B00001000;
    byte faceRear = B10100010;
    byte faceClockWise = B10000000;
    byte faceCounterWise = B00101010;
    byte faceLeft = B10001010;
    byte faceRight = B00100000;
    
    void testWheels ();
    void mov (byte dir, float dist, long del);
    void turn (byte dir, float degree, long del);  
    void strafe (byte dir, float dist, long del);
    void movPairs (byte dir, float dist, long del, byte wheels);
    void steer (byte dir, float dist, long del, float ratio, byte master, byte slave);    
};
    
    
#endif    
    
    