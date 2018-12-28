#include "interrupt_motion.h"

void setup() {
    delay(2000);                //wait for me to turn on the switch
    Serial.begin(9600);         //for debugging
    setupTimer();
    for (float record=0; record<60; record = odometer()){
        //TIMSK3 = 0;
        drivi(60-record);
        //TIMSK3 = 0;
        //showStack();
        //TIMSK3 |= 0x06;
        while(odometer() - record < 10);
    }
}

void loop() {
  
}




//recycle bin
/*

//start moving for auto-calibrate
    movi(faceFront, 3, 800);
    movi(faceRear, 3, 800);
    movi(faceFront, 3, 800);
    movi(faceRear, 3, 800);
    while(stackCnt > 0) {lineSensor.calibrate();}
    accelMovi(faceRight, 48);
    
    delay(1000);           //wait for robot to move out the home box
    //while(on_line[3] == 0)  {readLineSensor(); printCalibratedValues();}   //looking for arrival of corner square 
    clearTimer();
    //accelMovi(faceFront, 72);
    //while(position >= 2800)   {readLineSensor();}

    accelMovi(faceFront, 60);
    while (stackCnt>0||steps>0);
    delay(1000);






*/
