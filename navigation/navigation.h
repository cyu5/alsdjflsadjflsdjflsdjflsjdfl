#ifndef NAVIGATION_H
#define NAVIGATION_H

#include "lineSensor.h"
#include "interrupt_motion.h"
#include "colorSensor.h"
#include "navDefines.h"

int pastColor = YELLOW, currColor = YELLOW, destColor = GRAY, spot = 1;
char landmark = FORK;
char maneuver = MIDDLE;
char trackPos;

void lineFollow(){
        readLineSensor();
        float ratio;
        if (trackPos != position)
        {
            ratio = exp((4 * (float)trackPos / ((NUM_SENSORS-1)*CALOUT)) - 2 );
        }
        else ratio = exp((4 * (float)position / ((NUM_SENSORS-1)*CALOUT)) - 2 );
        steeri(ratio);
        
}

void shortCut(){
    if (landmark == FORK)
    {
        if (maneuver == LEFT) trackPos = 0;
        else if (maneuver == RIGHT) trackPos = CALOUT*(NUM_SENSORS-1);
        else if (maneuver == BACK)
        {
            constDrivi(3,300);
            while (TIMSK3);
            rotati(180);
            trackPos = position;
        }
        else trackPos = position;
    }
    else
    {
        if (maneuver == RIGHT)
        {
            constDrivi(3,300);
            while (TIMSK3);
            rotati(90);
            trackPos = position;
        }
        else if (maneuver == LEFT)
        {
            constDrivi(3,300);
            while (TIMSK3);
            rotati(-90);
            trackPos = position;
        }
        else if (maneuver == BACK)
        {
            constDrivi(3,300);
            while (TIMSK3);
            rotati(180);
            trackPos = position;
        }
        else trackPos = position;
    }

}

void navCenter(){
    if (currColor < MAGENTA)
    {
         if (maneuver == LEFT)
         {
            if (destColor < MAGENTA)  trackPos = line[0];
            else 
            {
                constDrivi(6,300);
                while (TIMSK3);
                rotati(-135);
                trackPos = line[1];
            }
         }
         else if (maneuver == RIGHT)
         {
            if (destColor < MAGENTA)  trackPos = line[3];
            else 
            {
                constDrivi(6,300);
                while (TIMSK3);
                rotati(135);
                trackPos = line[1];
            }
         }
         else if (maneuver == LCURVE) 
         {
            while (lineCnt != CROSS) readLineSensor();
            trackPos = line[1];
         }
         else if (maneuver == RCURVE)
         {
            while (lineCnt != CROSS) readLineSensor();
            trackPos = line[2];
         }
    }
    else 
    {
        if (maneuver == LEFT)
        {
            constDrivi(3,300);
            while (TIMSK3);
            rotati(-90);
            trackPos = line[1];
        }
        else if (maneuver == RIGHT)
        {
            constDrivi(3,300);
            while (TIMSK3);
            rotati(90);
            trackPos = line[1];
        }
        else if (maneuver == LCURVE)  trackPos = line[1];
        else if (maneuver == RCURVE)  trackPos = line[2];
        else if (maneuver == MIDDLE)
        {
            while (lineCnt != 1) readLineSensor();
        }
        else 
        {
            constDrivi(3,300);
            while (TIMSK3);
            rotati(-180);
            trackPos = line[1];
        }
    }


    
}

void updatePath (){

    // update landmark
    if (currColor == GRAY)
    {
        if (destColor < MAGENTA)  
        {
            if (!spot) landmark = SPLIT;
            else if (spot == 5) landmark = FORK;   //at the center problem
            else landmark = FORK;
        }
        else landmark = CROSS;
    }
    else if (currColor < MAGENTA)
    {
        if (!spot) landmark = SPLIT;
        else if (spot == 5) landmark = FORK;
        else landmark = FORK;
    }
    else landmark = CROSS;

}

void printNav();

void navigation(){

    updatePath();
    while (lineCnt != landmark)
    {
        lineFollow();
        printNav();
    }
    
    constDrivi(travelDist, 300);
    maneuver = trackTable[pastColor][destColor];
    if (spot != 5 && spot)
    {
        shortCut(); 
        while (lineCnt >= SPLIT)
        {
            lineFollow();
            printNav();
        }

        
        if ((currColor == YELLOW) && (destColor == GRAY) && (spot == 3)) destColor = BLUE;
        if ((currColor == BLUE) && (destColor == GRAY) && (spot == 3)) destColor = YELLOW;
        pastColor = currColor;
        currColor = nextCurrColor[currColor][destColor];
        spot += incSpot[currColor][destColor];
    }
    else if (!spot)    
    {
        constDrivi(3,300);
        while(TIMSK3);
        //drop coin
        rotati(180);
        pastColor = destColor;
        currColor = destColor;
        destColor = GRAY;
        spot = 1;
    } 
    else 
    {
        navCenter();
        while (lineCnt != 1)
        { 
            lineFollow();
        }
        pastColor = currColor;
        currColor = GRAY;
        spot = 4;
    }

}

void printNav(){
    Serial.print("curr-");showColor(currColor);Serial.print((int)spot);Serial.print('\t');
    Serial.print("dest-");showColor(destColor);Serial.print('\t');
    Serial.print("lookfor-");
    if (landmark == SPLIT) Serial.print("split");
    else if (landmark == FORK) Serial.print("fork");
    else if (landmark == CROSS) Serial.print("cross");
    Serial.print('\t');
    Serial.print("maneuver-");
    if (maneuver == LEFT) Serial.print("left");
    else if (maneuver == RIGHT) Serial.print("right");
    else if (maneuver == MIDDLE) Serial.print("middle");
    else if (maneuver == BACK) Serial.print("back");
    else if (maneuver == LCURVE) Serial.print("lcurve");
    else if (maneuver == RCURVE) Serial.print("rcurve");
    Serial.print('\t');
    Serial.print("trackPos:"); Serial.print((int)trackPos);
    Serial.print('\t');
    printLineSensor();
    Serial.print('\n');

}


//recycling
/*

void checkMark(){

    //determine the landmark to look for
    if (destColor == GRAY)
    {
        if (currColor < MAGENTA)  
        {
            if (!spot) landmark = SPLIT;
            else if (spot == 5) landmark = CROSS;   //at the center problem
            else landmark = FORK;
        }
        else landmark = CROSS;
    }
    else if (currColor < MAGENTA)
    {
        if (!spot) landmark = SPLIT;
        else if (spot == 5) landmark = FORK;
        else landmark = FORK;
    }
    else landmark = CROSS;

    readLineSensor();

    //check if the landmark is met
    if (lineCnt == landmark)    //might be trouble with SPLIT due to wide on_mark range
    {
        matchMark = true;
    }
    else if (lineCnt == 1 && trackPos <30 && trackPos > 40)  //better with memory
    {
        matchMark = false;
    }
}







void autoCal(){
    constDrivi(3, 800);
    pushConstDrivi(-3, 800);
    pushConstDrivi(3, 800);
    pushConstDrivi(-3, 800);
    while(stackCnt||stepCnt > 31) 
    {
        lineSensor.calibrate();
    }
    //filter more noise for calmin
    for (int i = 0; i < NUM_SENSORS; i++)
    {
      lineSensor.calibratedMinimumOn[i] *= (float)6/5;
    }
}



void getPath2 (int currColor, int destColor, int spot){
    const float CSTS = 8.5;
    float distSeg1 = (spot+1) * CSTS;
    float distSeg2 = (6-spot) * 12;
    float distSeg3 = 2*(6-spot) * CSTS;

    float distSeg4 = (spot+1) * 6;
    float distSeg5 = 2*(6-spot) * 6;
    char colorDiff = destColor - currColor;
    bool cornerCurr = (currColor < MAGENTA);
    bool cornerDest = (destColor < MAGENTA);
    if (currColor < MAGENTA && destColor < MAGENTA){
        if (!colorDiff){
            drivi(distSeg1);
            spot--;
            //track(lineCnt/2);
        }
        else if ((currColor + destColor) % 2){
            drivi(distSeg1 + distSeg2);
            if (colorDiff > 0){
                tracking = (colorDiff == 1) ? 2 : 0;
            }
            else {
                tracking = (colorDiff == -1) ? 0 : 2; 
            }
        }
        else{
            drivi(distSeg1 + distSeg3)
        }
    }
    else if(currColor < MAGENTA){
        
    }
    else if(destColor < MAGENTA){
        
    }
    else{
        if (colorDiff){
            drivi(distSeg4+distSeg5);
            //track(lineCnt/2);
        }
        else{
            drivi(distSeg4);
            //track(lineCnt/2);
        }
    }
    
}

*/

#endif
