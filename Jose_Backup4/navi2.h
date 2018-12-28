#ifndef NAVIGATION_H
#define NAVIGATION_H

#include "lineFollow.h"
#include "colorSensor.h"
#include "servo.h"

#define HYPO_LEN 8.5
#define SIDE_LEN 6.0
#define BOT_LEN 3.75

#define MAG_LEN 3.0
#define MAG_LEN2 3.0
#define JUN_LEN 2.5
#define ERR_LEN 1.0

#define CEN_OUT_CR 5.0
#define CEN_OUT_EDGE 2.5
#define CEN_IN_CR 10.5
#define CEN_IN_EDGE 8.5

char currColor = YELLOW, destColor = YELLOW, currSpot = 0, destSpot = 0, coinCnt;
unsigned long gameTime;
bool coinArray[NUM_COLORS-1][4];
int colorVector[NUM_COLORS][NUM_COLORS] = {
                                          {180, 90, 0, -90, 135, -45, -135},
                                          {-90, 180, 90, 0, -135, 45, -45},
                                          {0, -90, 180, 90, -45, 135, 45},
                                          {90, 0, -90, 180, 45, -135, 135},
                                          {-135, 135, 45, -45, 180, 0, -90},
                                          {45, -45, -135, 135, 0, 180, 90},
                                          {-45, -135, 135, 45, -90, 90, 180}
                                          };

void simpleMove2(float startDist, float endDist, float segDist, char viaPoints, bool accel = false){
    float dist;
    float distStack[10];
     
    if (!viaPoints) dist = endDist;
    else if (viaPoints == 1)
    {
        dist = startDist + endDist;
        distStack[viaPoints-1] = startDist;
    }
    else
    {
        dist = startDist + endDist + (viaPoints-1)*segDist;
        for (char i=0; i<viaPoints-1; i++)
        {
            distStack[i] = segDist;
        }
        distStack[viaPoints-1] = startDist;
    } 

    //constDrivi(dist, 300);
    if (accel) drivi(dist);
    else constDrivi(dist, 300);
    float junMark = odometer();
    if (viaPoints)
    {
        #define JUNCTION (((lineReading&B00010100) == B00010100) || ((lineReading&B00101000) == B00101000) || ((lineReading&B11000011) == B11000011))
        //#define JUNCTION (sensorCnt > 3)
        //#define JUNCTION (lineCnt > 2)
        #define lastMilestone (odometer() - junMark >= abs(dist-endDist) - JUN_LEN)
        #define noMilestone (odometer() - junMark >= abs(dist))
        while(!lastMilestone)
        {
            lineFollow();
            if (!TIMSK3)
            {
                constDrivi(1, 300);
                accel = false;
                //drivi(0.5);
            }
        }
        changePeriod(1200);
        while (!JUNCTION)
        {
            lineFollow();
            if (!TIMSK3)
            {
                constDrivi(1, 300);
                accel = false;
                //drivi(0.5);
            }
            else if (noMilestone)
            {
                readLineSensor();
                return;
            }
        }
        if (accel) 
        {
            while(TIMSK3); 
            //constDrivi((dist - (odometer() - junMark)), 500);
            drivi((dist - (odometer() - junMark)));
        }
        else constDrivi(dist, 500);
        while (TIMSK3);
    }
}

void simpleMove1(float startDist, float endDist, float segDist, char viaPoints, bool accel = false){
    float dist;
    float distStack[10];
     
    if (!viaPoints) dist = endDist;
    else if (viaPoints == 1)
    {
        dist = startDist + endDist;
        distStack[viaPoints-1] = startDist;
    }
    else
    {
        dist = startDist + endDist + (viaPoints-1)*segDist;
        for (char i=0; i<viaPoints-1; i++)
        {
            distStack[i] = segDist;
        }
        distStack[viaPoints-1] = startDist;
    } 

    //constDrivi(dist, 300);
    if (accel) drivi(dist);
    else constDrivi(dist, 300);
    float junMark = odometer();
    while (viaPoints--)
    {
        #define JUNCTION (((lineReading&B00010100) == B00010100) || ((lineReading&B00101000) == B00101000) || ((lineReading&B11000011) == B11000011))
        //#define JUNCTION (lineCnt > 2)
        #define onMilestone (odometer() - junMark >= abs(distStack[viaPoints]) - JUN_LEN)
        #define offMilestone (odometer() - junMark >= abs(distStack[viaPoints]) + BOT_LEN)

        constDrivi(dist, 300);
        while (!onMilestone)
        {
            lineFollow();
            if (!TIMSK3)
            {
                constDrivi(1, 300);
                accel = false;
                //drivi(0.5);
            }
        }
        changePeriod(1200);
        while (!JUNCTION)
        {
            lineFollow();
            if (!TIMSK3)
            {
                constDrivi(1, 300);
                accel = false;
                //drivi(0.5);
            }
            else if (offMilestone)
            {
                TIMSK3 = 0;
                delay(1000);
                junMark = odometer()-BOT_LEN;
                break;
            }
        }
        if (!offMilestone) junMark = odometer();
        dist -= distStack[viaPoints];
    }

    readLineSensor();
    //constDrivi(dist, 500);

    if (accel) 
    {
        while(TIMSK3); 
        //constDrivi((dist - (odometer() - junMark)), 500);
        drivi((dist - (odometer() - junMark)));
    }
    else constDrivi(dist, 500);
    
    while (TIMSK3);

}

void simpleMove(float startDist, float endDist, float segDist, char viaPoints, bool accel = false){
    float dist;
    float distStack[10];
     
    if (!viaPoints) dist = endDist;
    else if (viaPoints == 1)
    {
        dist = startDist + endDist;
        distStack[viaPoints-1] = startDist;
    }
    else
    {
        dist = startDist + endDist + (viaPoints-1)*segDist;
        for (char i=0; i<viaPoints-1; i++)
        {
            distStack[i] = segDist;
        }
        distStack[viaPoints-1] = startDist;
    } 

    //constDrivi(dist, 300);
    if (accel) drivi(dist);
    else constDrivi(dist, 300);
    float junMark = odometer();
    bool overshoot = false;
    while (viaPoints--)
    {
        #define JUNCTION (((lineReading&B00010100) == B00010100) || ((lineReading&B00101000) == B00101000) || ((lineReading&B11000011) == B11000011))
        //#define JUNCTION (lineCnt > 2)
        #define onMilestone (odometer() - junMark >= abs(distStack[viaPoints]) - JUN_LEN)
        #define offMilestone (odometer() - junMark >= abs(distStack[viaPoints]) + BOT_LEN)

        constDrivi(dist, 300);
        while (!onMilestone)
        {
            lineFollow();
            if (!TIMSK3)
            {
                constDrivi(1, 300);
                accel = false;
                //drivi(0.5);
            }
        }
        changePeriod(1200);
        while (!JUNCTION)
        {
            lineFollow();
            if (!TIMSK3)
            {
                if(overshoot) 
                {
                    readLineSensor();
                    constDrivi(2, 300);while(TIMSK3);
                    return;
                }
                else 
                {
                    constDrivi(4, 300);
                    accel = false;
                    //drivi(0.5);
                    overshoot = true;
                 }
            }
            
        }
        junMark = odometer();
        if (!offMilestone)
        {
            dist -= distStack[viaPoints];
        }
        else
        {
            dist -= distStack[viaPoints];
            if (viaPoints)
            {
                dist -= distStack[--viaPoints];
            }
            else 
            {
                dist -= distStack[viaPoints];
            }
            
        }
    }

    readLineSensor();
    //constDrivi(dist, 500);

    if (accel) 
    {
        while(TIMSK3); 
        //constDrivi((dist - (odometer() - junMark)), 500);
        drivi((dist - (odometer() - junMark)));
    }
    else constDrivi(dist, 500);
    
    while (TIMSK3);

}


void alignMove(){
    if (currColor<MAGENTA)
    {
        //constDrivi(-MAG_LEN, 600); while (TIMSK3);
        drivi(-MAG_LEN); while (TIMSK3);
    }
    else
    {
        //constDrivi(-MAG_LEN2, 600); while (TIMSK3);
        drivi(-MAG_LEN2); while (TIMSK3);
    }
}

void localMove(){
    if (currColor<MAGENTA)
    {
        //simpleMove(MAG_LEN, ERR_LEN, HYPO_LEN, destSpot-currSpot+1);
        simpleMove(MAG_LEN-ERR_LEN, ERR_LEN, HYPO_LEN, destSpot-currSpot+1);
    }
    else
    {
        //simpleMove(MAG_LEN2, ERR_LEN, SIDE_LEN, destSpot-currSpot+1);
        simpleMove(MAG_LEN2-ERR_LEN, ERR_LEN, SIDE_LEN, destSpot-currSpot+1);
    }
    alignMove();
    currSpot = destSpot;
}

void centerinMove(){
    if (currColor<MAGENTA)
    {
        //simpleMove(MAG_LEN, CEN_IN_CR, HYPO_LEN, 4-currSpot+1);
        simpleMove(MAG_LEN-ERR_LEN, CEN_IN_CR, HYPO_LEN, 4-currSpot+1);
    }
    else
    {
        //simpleMove(MAG_LEN2, CEN_IN_EDGE, SIDE_LEN, 4-currSpot+1);
        simpleMove(MAG_LEN2-ERR_LEN, CEN_IN_EDGE, SIDE_LEN, 4-currSpot+1);
    }
}

void deliverMove(){
    destSpot = -1;
    centerinMove();
    rotati(colorVector[currColor][destColor]);  while(TIMSK3);  currColor = destColor;  // turn facing dest
    destSpot = -1;
    if (currColor<MAGENTA)
    {
        simpleMove(CEN_OUT_CR+HYPO_LEN, ERR_LEN, HYPO_LEN, 5);
    }
    else
    {
        simpleMove(CEN_OUT_EDGE+SIDE_LEN, MAG_LEN2, SIDE_LEN, 5);
    }
    currSpot = 0;
    currColor = destColor;
    
}

void regionMove(){

    if (currSpot != 5)
    {
        centerinMove();
    }
    rotati(colorVector[currColor][destColor]);  while(TIMSK3); currColor = destColor;    // turn facing dest
    /*
    if (currColor<MAGENTA)
    {
        simpleMove(CEN_OUT_CR+HYPO_LEN, JUN_LEN, HYPO_LEN, 4-(destSpot-1));
        rotati(180);  while(TIMSK3);    // turn facing dest
        //simpleMove(HYPO_LEN-BOT_LEN, ERR_LEN, HYPO_LEN, 1);
    }
    else 
    {
        simpleMove(CEN_OUT_EDGE+SIDE_LEN, BOT_LEN, SIDE_LEN, 4-(destSpot-1));
        rotati(180);  while(TIMSK3);    // turn facing dest
        //simpleMove(SIDE_LEN-(SIDE_LEN-BOT_LEN), ERR_LEN, SIDE_LEN, 1);
    }

    //alignMove();
    */
    if (currColor<MAGENTA)
    {
        simpleMove(CEN_OUT_CR+HYPO_LEN, JUN_LEN, HYPO_LEN, 4-(destSpot-1));
        rotati(180);  while(TIMSK3);    // turn facing dest
        simpleMove(HYPO_LEN-BOT_LEN, ERR_LEN, HYPO_LEN, 1);
    }
    else 
    {
        simpleMove(CEN_OUT_EDGE+SIDE_LEN, BOT_LEN, SIDE_LEN, 4-(destSpot-1));
        rotati(180);  while(TIMSK3);    // turn facing dest
        simpleMove(SIDE_LEN-(SIDE_LEN-BOT_LEN), ERR_LEN, SIDE_LEN, 1);
    }

    alignMove();
    currSpot = destSpot;
    currColor = destColor;
}


void getCoin(){
    bool coinLocated = false;
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
        if (mostCounts < 8)
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

    //final reading result
    destColor = coinColor;
    if (destColor == VOID) destColor = GRAY;
    coinArray[currColor][currSpot] = false;
    coinCnt--;
}

void deliver(){
    if (destColor != GRAY)
    {
        deliverMove();
        //constDrivi(1, 500);while(TIMSK3);
        drivi(1);while(TIMSK3);
        leaveToken(); 
        //constDrivi(-1, 500);while(TIMSK3);
        drivi(-1);while(TIMSK3);
        rotati(180); while (TIMSK3);
        if (currColor<MAGENTA)
        {
            
        }
        else 
        {
            
        }
    }
    else 
    {
        destColor = currColor;
        if (currColor<MAGENTA)
        {
            simpleMove(MAG_LEN, CEN_IN_CR-BOT_LEN-JUN_LEN, HYPO_LEN, 4-currSpot+1);
        }
        else 
        {
            simpleMove(MAG_LEN2, CEN_IN_EDGE-BOT_LEN-JUN_LEN, SIDE_LEN, 4-currSpot+1);
        }
        currSpot = 5;
        leaveToken(); //drop coin
        if (currColor < MAGENTA)
        {
            //constDrivi(BOT_LEN+JUN_LEN, 400);
            drivi(BOT_LEN+JUN_LEN);
            while(TIMSK3);
        }
        else 
        {
            //constDrivi(BOT_LEN+JUN_LEN, 400);
            drivi(BOT_LEN+JUN_LEN);
            while(TIMSK3);
        }
        
    }
}


#endif
