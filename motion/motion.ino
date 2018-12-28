#include "motion.h"

void setup() {
    delay(2000);
    Serial.begin(9600);
    DDRL = 0xFF;

    //constDrive(60,400);
    //constDrive(-10,400);
    rotate(360*3);
    //drivep2(50);
    //drivep2(-50);
    //strafe(40);
    //strafe(-40);   
}

void loop() {
    //testWheels();
    turn(12*10,360, 300);
    rotate(360*3)
    delay(2000);
}

//debugging code for stack accel interrupt
/*startEngine ();
    accelMovi(faceFront, 60, 800, 0, 4);
    for (int i = stackCnt; i>=0; i--){
        Serial.print("stepStack[");Serial.print(i);Serial.print("]: ");Serial.print(stepStack[i]);Serial.print("\t");
        Serial.print("masterStack[");Serial.print(i);Serial.print("]: ");Serial.print(masterStack[i]);Serial.print("\t");
        Serial.print("slaveStack[");Serial.print(i);Serial.print("]: ");Serial.print(slaveStack[i]);Serial.print("\t");
        Serial.println();
    }
    while (stackCnt>0){
        //readLineSensor(lineSensor);
        Serial.print("steps: ");Serial.print(steps);Serial.print("\t");
        Serial.print("masterCnt: ");Serial.print(masterCnt);Serial.print("\t");
        Serial.print("slaveCnt: ");Serial.print(slaveCnt);Serial.print("\t");
        Serial.println();
    }*/
