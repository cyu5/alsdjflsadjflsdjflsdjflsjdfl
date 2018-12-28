#ifndef NAVIGATION_H
#define NAVIGATION_H

#include "lineSensor.h"
#include "interrupt_motion.h"
#include "colorSensor.h"


char pastColor = YELLOW, currColor = YELLOW, destColor = CENTER, spot = 1;

void lineFollow(){
        readLineSensor();
        float ratio;
        ratio = exp((4 * (float)position / ((NUM_SENSORS-1)*CALOUT)) - 2 );
        steeri(ratio);
}

char setMotion()
{
    if (currColor < MAGENTA || (currColor == CENTER && destColor < MAGENTA)) 
    {
        return 8.5 + 3*(spot == 5 || !spot); 
    }
    else
    {
        return 6 + 3*(spot == 5 || !spot);
    }
}

void navigation(){

    while (lineCnt < 2)
    {
        lineFollow();
        //printNav();
    }
    spot += (currColor == CENTER) ? -1 : 1;
    constDrivi(setMotion(), 300);
    if (spot)
    {
        if (spot == 5) 
        {
            while (TIMSK3);
            //orientate to dest
            rotati(180);
            currColor = CENTER;
            constDrivi(setMotion(), 300);
            do readLineSensor(); while (lineCnt != 2);
        }
        else 
        {
            while (lineCnt > 1)
            {
                lineFollow();
            }
        }
    }
    else
    {
        while (TIMSK3);
        //drop coin
        rotati(180);
        currColor = destColor;
        destColor = CENTER;

        //bookmark: get this distance
        constDrivi(setMotion(), 300);
    }
    
}

void meetup(){
    if (currColor < MAGENTA || (currColor == CENTER && destColor < MAGENTA)) 
    {
        return 8.5 + 3*(spot == 5 || !spot); 
    }
    else
    {
        return 6 + 3*(spot == 5 || !spot);
    }
}

void printNav(){
    Serial.print("curr-");showColor(currColor);Serial.print((int)spot);Serial.print('\t');
    Serial.print("dest-");showColor(destColor);Serial.print('\t');Serial.print('\t');
    printLineSensor();
    Serial.print('\n');

}

#endif
