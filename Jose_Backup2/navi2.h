#ifndef NAVIGATION_H
#define NAVIGATION_H

#include "lineFollow.h"
#include "colorSensor.h"
#include "servo.h"


#define HYPO_LEN 10
#define SIDE_LEN 7.0
#define BOT_LEN 3.75

#define MAG_LEN 4.0
#define MAG_LEN2 3.5
#define JUN_LEN 2.5
#define ERR_LEN 1.0

#define CEN_CR 4.5
#define CEN_EDGE 2.5

char currColor, destColor, currSpot, destSpot, coinCnt;
unsigned long gameTime;
bool coinArray[NUM_COLORS-1][4];
int colorVector[NUM_COLORS][NUM_COLORS] = {
                                          {    0  ,  -90  ,  180  ,   90  ,  -45  ,  135  ,   45},
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
    //constDrivi(dist, 300);
    while (viaPoints--)
    {
        
        byte junSensor;
        byte followSensor;
        #define readLineSensor() ({junSensor = PINA; followSensor = (moveBack) ? PINC : PINA;})
        #define lineFollow()   ({readLineSensor(); stpercent = steerTable[followSensor];})
        #define JUNCTION (((junSensor&B00010100) == B00010100) || ((junSensor&B00101000) == B00101000) || ((junSensor&B11000011) == B11000011))
        #define onMilestone (odometer() - junMark >= abs(distStack[viaPoints]) - JUN_LEN)

        constDrivi(dist, 300);
        while (!onMilestone)
        {
            lineFollow();
            if (!TIMSK3)
            {
                if (moveBack) {constDrivi(-6, 300);}
                else  {constDrivi(6, 300);}
            }
        }
        changePeriod(1000);
        while (!JUNCTION)
        {
            lineFollow();
            if (!TIMSK3)
            {
                //if (moveBack) {constDrivi(-6, 300);}
                //else  {constDrivi(6, 300);}
                if (moveBack) {constDrivi(-6, 1000);}
                else  {constDrivi(6, 1000);}
            }
        }
        TIMSK3 = 0;
        delay(1000);
        junMark = odometer();
        dist -= distStack[viaPoints];  
    }
    stpercent = 100;
    constDrivi(dist, 500);
    while(TIMSK3);
}



void alignMove(){
    if (currColor<MAGENTA)
    {
        constDrivi(-MAG_LEN, 600); while (TIMSK3);
    }
    else
    {
        constDrivi(-MAG_LEN2, 600); while (TIMSK3);
    }
}

void localMove(){
    if (currColor<MAGENTA)
    {
        simpleMove(HYPO_LEN-(MAG_LEN-ERR_LEN), ERR_LEN, HYPO_LEN, destSpot-currSpot+1, true);
    }
    else
    {
        simpleMove(SIDE_LEN-(MAG_LEN2-ERR_LEN), ERR_LEN, SIDE_LEN, destSpot-currSpot+1, true);
    }
    alignMove();
    currSpot = destSpot+1;
}

void centerinMove(){
    if (currColor<MAGENTA)
    {
        simpleMove(HYPO_LEN-(MAG_LEN-ERR_LEN), CEN_CR, HYPO_LEN, 4-currSpot+1, true);
    }
    else
    {
        simpleMove(SIDE_LEN-(MAG_LEN2-ERR_LEN), CEN_EDGE, SIDE_LEN, 4-currSpot+1, true);
    }
    
}

void deliverMove(){
    destSpot = -1;
    centerinMove();
    rotati(colorVector[currColor][destColor]);  while(TIMSK3);  currColor = destColor;  // turn facing dest
    destSpot = -1;
    if (currColor<MAGENTA)
    {
        simpleMove(CEN_CR+HYPO_LEN, ERR_LEN, HYPO_LEN, 5);
    }
    else
    {
        simpleMove(CEN_EDGE+SIDE_LEN, ERR_LEN, SIDE_LEN, 5);
    }
    currSpot = -1;
    currColor = destColor;
    
}

void regionMove(){

    if (currSpot != 5)
    {
        centerinMove();
    }
    rotati(colorVector[currColor][destColor]);  while(TIMSK3);currColor = destColor;    // turn facing dest
    if (currColor<MAGENTA)
    {
        simpleMove(CEN_CR+HYPO_LEN, JUN_LEN, HYPO_LEN, 4-destSpot);
    }
    else 
    {
        simpleMove(CEN_EDGE+SIDE_LEN, JUN_LEN, SIDE_LEN, 4-destSpot);
    }
    simpleMove(JUN_LEN, ERR_LEN, SIDE_LEN, 1, true);
    alignMove();
    currSpot = destSpot+1;
    currColor = destColor;
}


void getCoin(){
    bool coinLocated = false;
    destColor = currColor;
    for (destSpot=currSpot; destSpot<4 && !coinLocated; destSpot++)
    {
        if (coinArray[destColor][destSpot])
        {
            localMove();
            coinLocated = true;
        }
    }
    if (!coinLocated)
    {
        for (destSpot=3; destSpot>=0 && !coinLocated; destSpot--)
        {
            for (destColor=YELLOW; destColor<GRAY && !coinLocated; destColor++)
            {
                if (coinArray[destColor][destSpot])
                {
                    regionMove();
                    coinLocated = true;   
                    
                }
            }
        }
    }
}

void pickup(){
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
            bool halfGray = true;
            if (colorSamples[i] != VOID && colorSamples[i] != GRAY)
            {
                colorCounts[colorSamples[i]]++;
            }
            else if (colorSamples[i] == GRAY)
            {
                if (halfGray) colorCounts[colorSamples[i]]++;
                halfGray = !halfGray;
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
        if (mostCounts < 5)
        {
            leaveToken();
            if (takeSamples==2)
            {
              constDrivi(-0.5 ,1000);
            }
            else if (takeSamples==1) 
            {
              constDrivi(1 ,1000);
            }
            else if (takeSamples==0)
            {
              constDrivi(-0.5 ,1000);
            }
            while(TIMSK3);
            pickUp();
        }
        else break;
    }
    
    destColor = coinColor;
    if (destColor == VOID) destColor = GRAY;
    coinArray[currColor][currSpot] = false;
    coinCnt--;
}

void deliver(){
    if (destColor != GRAY)
    {
        deliverMove();
        leaveToken();
        if (currColor<MAGENTA)
        {
            constDrivi(-MAG_LEN-ERR_LEN, 500);
        }
        else 
        {
            constDrivi(-MAG_LEN2-ERR_LEN, 500);
        }
        currSpot = 0;
    }
    else 
    {
        destColor = currColor;
        if (currColor<MAGENTA)
        {
            simpleMove(HYPO_LEN-MAG_LEN, CEN_CR, HYPO_LEN, 4-currSpot+1, true);
        }
        else 
        {
            simpleMove(SIDE_LEN-MAG_LEN2, CEN_EDGE, SIDE_LEN, 4-currSpot+1, true);
        }
        currSpot = 5;
        drivi(-BOT_LEN);while(TIMSK3);
        leaveToken(); 
        drivi(BOT_LEN);while(TIMSK3);
    }
}

    

#endif
