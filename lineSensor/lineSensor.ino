#include "lineSensor.h"


void setup()
{
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    setupLine();
    testReadTime();
}


void loop(){
    
    readLineSensor();
    printLineSensor();
    #define JUNCTION (((lineReading&B00010100) == B00010100) || ((lineReading&B00101000) == B00101000) || ((lineReading&B11000011) == B11000011))
    #define JUNCTION (lineCnt >= 2)
       
    if (JUNCTION) delay(1000);
}

void testReadTime(){
    unsigned long readTime = micros();
    //readLineSensor();
    //bool testbool = (PINA&B00111100) == B00011100 || (PINA&B00111100) == B00111000;
    //int stpercent = steerTable[PINA];
    readTime = micros() - readTime;
    Serial.print("myTime:");Serial.print(readTime);       //about 1180 + TIMEOUT to read sensors(using library as in v2)
                                                          //about 560 + TIMEOUT in v1
    Serial.print('\n');
}


