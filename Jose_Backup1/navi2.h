#ifndef NAVIGATION_H
#define NAVIGATION_H

#include "lineFollow.h"
#include "colorSensor.h"
#include "servo.h"
#include "motion.h"


#define HYPO_LEN 10
#define SIDE_LEN 7.5
#define BOT_LEN 3.75

#define MAG_LEN 4.0
#define MAG_LEN2 3.5
#define JUN_LEN 2.5
#define ERR_LEN 0.5

#define CEN_OUT_CR 4.5
#define CEN_OUT_EDGE 2.5
#define CEN_IN_CR 4.5
#define CEN_IN_EDGE 2.5


#define CEN_IN_CR 10
#define CEN_IN_EDGE 8.5

char currColor = YELLOW, destColor = YELLOW, currSpot = 0, destSpot = 0, coinCnt;
unsigned long gameTime;
bool coinArray[NUM_COLORS-1][4];


int colorVector[NUM_COLORS][NUM_COLORS] = {
                                          {    0  ,  -90  ,    0  ,   90  ,  -45  ,  135  ,   45},
                                          {   90  ,    0  ,  -90  ,  180  ,   45  , -135  ,  135},
                                          {  180  ,   90  ,    0  ,  -90  ,  135  ,  -45  , -135},
                                          {  -90  ,  180  ,   90  ,    0  , -135  ,   45  ,  -45},
                                          {   45  ,  -45  , -135  ,  135  ,    0  ,  180  ,   90},
                                          { -135  ,  135  ,   45  ,  -45  ,  180  ,    0  ,  -90},
                                          {  135  ,   45  ,  -45  , -135  ,   90  ,  -90  ,    0}
                                          };


void simpleMove(float startDist, float endDist, float segDist, char viaPoints, bool moveBack = false){
    float dist = endDist;
    if (moveBack) dist *= -1;
    float distStack[viaPoints];

    if (viaPoints)
    {
        for (char i=0; i<viaPoints-1; i++)
        {
            distStack[i] = segDist;
            if (moveBack) distStack[i] *= -1;
            dist += distStack[i];
        }
        distStack[viaPoints-1] = startDist;
        if (moveBack) distStack[viaPoints-1] *= -1;
        dist += distStack[viaPoints-1];
    } 

    float junMark = odometer();
    //drivi(dist);
    constDrivi(dist, 300);
    while (viaPoints--)
    {
        #define JUNCTION (((lineReading&B00010100) == B00010100) || ((lineReading&B00101000) == B00101000) || ((lineReading&B11000011) == B11000011))
        #define onMilestone (odometer() - junMark >= abs(distStack[viaPoints]) - JUN_LEN)

        while (!onMilestone)
        {
            lineFollow();
            if (!TIMSK3)
            {
                if (moveBack) {constDrivi(-6, 300);}
                else  {constDrivi(6, 300);}
            }
        }
        while (!JUNCTION)
        {
            lineFollow();
            if (!TIMSK3)
            {
                if (moveBack) {constDrivi(-6, 300);}
                else  {constDrivi(6, 300);}
            }
        }
        junMark = odometer();
        dist -= distStack[viaPoints];  
    }
    while(TIMSK3)lineFollow();                                 
    constDrivi((dist - (odometer() - junMark)), 500);  while(TIMSK3)lineFollow();
}


void locateMove(){
    if (currColor<MAGENTA)
    {
        simpleMove(JUN_LEN, ERR_LEN, HYPO_LEN, destSpot-currSpot+1, true);
        delay(500);
        constDrivi(-MAG_LEN, 600); while (TIMSK3);
    }
    else
    {
        simpleMove(JUN_LEN, ERR_LEN, SIDE_LEN, destSpot-currSpot+1, true);
        delay(500);
        constDrivi(-MAG_LEN2, 600); while (TIMSK3);
    }
    
    currSpot = destSpot+1;
}

void centerinMove(){
    if (currColor<MAGENTA)
    {
        simpleMove(JUN_LEN, CEN_IN_CR, HYPO_LEN, 4-currSpot+1, true);
    }
    else
    {
        simpleMove(JUN_LEN, CEN_IN_EDGE, SIDE_LEN, 4-currSpot+1, true);
    }
}

void deliverMove(){
    destSpot = -1;
    if (currColor<MAGENTA)
    {
        simpleMove(HYPO_LEN-MAG_LEN, CEN_IN_CR, HYPO_LEN, 4-currSpot+1 , true);
    }
    else
    {
        simpleMove(SIDE_LEN-MAG_LEN2, CEN_IN_EDGE, SIDE_LEN, 4-currSpot+1 , true);
    }
    
    rotati(colorVector[currColor][destColor]);  while(TIMSK3);    // turn facing dest
    currColor = destColor;
    destSpot = -1;
    if (currColor<MAGENTA)
    {
        simpleMove(CEN_OUT_CR+HYPO_LEN, JUN_LEN, HYPO_LEN, 5);
    }
    else
    {
        simpleMove(CEN_OUT_EDGE+SIDE_LEN, JUN_LEN, SIDE_LEN, 5);
    }
    
    currSpot = -1;
    
}

void centeroutMove(){
    rotati(colorVector[currColor][destColor]);  while(TIMSK3);    // turn facing dest

    currColor = destColor;
    if (currColor<MAGENTA)
    {
        simpleMove(CEN_OUT_CR+HYPO_LEN, ERR_LEN, HYPO_LEN, 4-destSpot);
        delay(500);
        constDrivi(-MAG_LEN, 600); while (TIMSK3);
        
    }
    else 
    {
        simpleMove(CEN_OUT_EDGE+SIDE_LEN, ERR_LEN, SIDE_LEN, 4-destSpot);
        delay(500);
        constDrivi(-MAG_LEN2, 600); while (TIMSK3);
    }
    
    
}



void getCoin(){
    bool coinLocated = false;
    for (destSpot=currSpot; destSpot<4 && !coinLocated; destSpot++)
    {
        if (coinArray[destColor][destSpot])
        {
            locateMove();
            coinLocated = true;
        }
    }
    /*
    if (currSpot != 5)
    {
        for (destSpot=0; destSpot<4 && !coinLocated; destSpot++)
        {
            if (coinArray[destColor][destSpot])
            {
                locateMove();
                coinLocated = true;
            }
        }
    }
    */
    if (!coinLocated)
    {
        for (destSpot=3; destSpot>=0 && !coinLocated; destSpot--)
        {
            for (destColor=YELLOW; destColor<GRAY && !coinLocated; destColor++)
            {
                if (coinArray[destColor][destSpot])
                {
                    
                    if (currSpot != 5)
                    {
                        centerinMove();
                    }
                    centeroutMove();
                    currSpot = destSpot+1;
                    coinLocated = true;
                    currColor = destColor;
                    
                }
            }
        }
    }



    //coinCnt == 0?
}

void pickup(){
    char testArray[12] = {GREEN, CYAN, BLUE, GREEN, RED, MAGENTA, YELLOW, CYAN, MAGENTA, RED, YELLOW, BLUE};
    char testArrayt[12] = {GRAY, BLUE, YELLOW, GRAY, GREEN, GRAY, CYAN, GRAY, RED, MAGENTA, GRAY, GRAY};

    byte takeSamples = 3;                       //take samples 3 times at most
    byte colorCounts[NUM_COLORS] = {0};         //count for each color in all samples from 3 measurements
    byte mostly = GRAY;                         //the color that is the mode of all 3 measurements
    byte mostCounts = 0;                        //the count for mostly(mode of all colors)
    pickUp();
    //try pickup and read 3 times
    while (takeSamples--)
    {
        byte colorSamples[60];
        byte sampleNum = 0;

        //scan and collect multiple color samples
        for (char wabble = -25; wabble <= 25; wabble++)
        {
            sliderTurn(SLIDER_IN + wabble);
            readColor();
            colorSamples[sampleNum++] = coinColor;
        }

        //count the occurance of each color
        for (byte i=0; i<sampleNum; i++)
        {
            if (colorSamples[i] != VOID)
            {
                colorCounts[colorSamples[i]]++;
            }
        }
        
        //determine coin color as the most frequently read, except VOID color
        for (byte i=0; i<NUM_COLORS; i++)
        {
            if (mostCounts < colorCounts[i])
            {
                mostCounts = colorCounts[i];
                mostly = i;
            }
        }

        //if under 20 counts then move a little and start over, else commit to mostly color finish the function
        coinColor = mostly;
        if (mostCounts < 20)
        {
            leaveToken();
            if (takeSamples==2)
            {
              constDrive(-0.5 ,1000);
            }
            else if (takeSamples==1) 
            {
              constDrive(1 ,1000);
            }
            else if (takeSamples==0)
            {
              constDrive(-0.5 ,1000);
            }
            pickUp();
        }
        else break;
    }
    
    destColor = coinColor;
    if (destColor == VOID) destColor = GRAY;
    
    //destColor = testArray[12-coinCnt];
    coinArray[currColor][currSpot] = false;
    coinCnt--;
}

void deliver(){
    if (destColor != GRAY)
    {
        deliverMove();
        currColor = destColor;
        leaveToken(); //drop coin
        rotati(180); while (TIMSK3);
    }
    else 
    {
        destColor = currColor;
        if (currColor<MAGENTA)
        {
            simpleMove(HYPO_LEN-MAG_LEN, MAG_LEN, HYPO_LEN, 4-currSpot+1, true);
        }
        else 
        {
            simpleMove(SIDE_LEN-MAG_LEN2, MAG_LEN, SIDE_LEN, 4-currSpot+1, true);
        }
        
        currSpot = 5;
        leaveToken(); //drop coin
        if (currColor < MAGENTA)
        {
            constDrivi(CEN_IN_CR-MAG_LEN,400);
            //drivi(HYPO_LEN-ERR_LEN);
            while(TIMSK3);
        }
        else 
        {
            constDrivi(CEN_IN_EDGE-MAG_LEN2,400);
            //drivi(SIDE_LEN-ERR_LEN);
            while(TIMSK3);
        }
        
    }
}


#endif
