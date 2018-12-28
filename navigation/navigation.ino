#include "navigation.h"

void setup() {
    delay(2500);                //wait for me to turn on the switch
    Serial.begin(9600);         //for debugging
    setupColor();                 //colorSensor
    setupTimer();               //interrupt motion
    setupLine();
    firstMove();
}


void loop() {
    navigation();
    
    
    
}

void firstMove(){
    drivi(-42);                            // move from home to yellow
    while (TIMSK3);
    rotati(45); 
    while (TIMSK3);                       
    constDrivi(travelDist,300);                     // move from yellow to gray
    while (lineCnt != SPLIT) readLineSensor();
    while (lineCnt == SPLIT) readLineSensor();
}

