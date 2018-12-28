#ifndef NAVIGATION_H
#define NAVIGATION_H

#include "lineFollow.h"
#include "colorSensor.h"
#include "servo.h"



#define HYPO_LEN 9.5
#define SIDE_LEN 7.0
#define BOT_LEN 3.75
#define TUR_LEN 4.5

#define MAG_LEN 3.5
#define MAG_LEN2 3.5
#define JUN_LEN 2.5
#define ERR_LEN 1.0

#define CEN_OUT_CR 4.8
#define CEN_OUT_EDGE 2.3
#define CEN_IN_CR 10.5
#define CEN_IN_EDGE 8.5

char currColor = YELLOW, destColor = YELLOW, currSpot = 0, destSpot = 0, coinCnt, gameRound;
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


void simpleMove(float startDist, float endDist, float segDist, char viaPoints, byte followMode = steerMiddle, bool accel = true){
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

    if (accel) drivi(dist);
    else constDrivi(dist, 300);
    float junMark = odometer();
    char firstPoint = viaPoints-1;
    while (viaPoints--)
    {
        //#define JUNCTION (((lineReading&B00010100) == B00010100) || ((lineReading&B00101000) == B00101000) || ((lineReading&B11000011) == B11000011))
        #define JUNCTION (lineCnt >= 2)
        #define onMilestone (odometer() - junMark >= abs(distStack[viaPoints]) - JUN_LEN)
        #define offMilestone (odometer() - junMark >= abs(distStack[viaPoints]) + BOT_LEN)


        //constDrivi(dist, 300);
        while (!onMilestone)
        {
            if (viaPoints == firstPoint) lineFollow(followMode);
            else lineFollow();
            if (!TIMSK3)
            {
                constDrivi(1, 300);
                accel = false;
            }
            
        }
        if (!accel) changePeriod(1200);
        while (!JUNCTION)
        {
            if (viaPoints == firstPoint) lineFollow(followMode);
            else lineFollow();
            if (!TIMSK3)
            {
                constDrivi(1, 300);
                accel = false;
            }
        }
        junMark = odometer();       
        dist -= distStack[viaPoints];
        if (!accel) changePeriod(300);
    }

    readLineSensor();
    stpercent = 100; sensorCnt = 0; lineCnt=0;
    while(TIMSK3); delay(100);
//    if (accel) 
//        drivi((dist - (odometer() - junMark)));
//    else 
        constDrivi((dist - (odometer() - junMark)), 600);
    
    while (TIMSK3);

}



void alignMove(){
    delay(100);
    if (currColor<MAGENTA)
    {
//        constDrivi(-BOT_LEN, 1200); 
//        while (TIMSK3)
//        {
//            readLineSensor();
//            if (lineCnt >= 2) changeDist(MAG_LEN3);
//        }
        //drivi(-MAG_LEN); while (TIMSK3);
        constDrivi(-MAG_LEN, 800); while (TIMSK3);
    }
    else
    {
//        constDrivi(-BOT_LEN, 1200); 
//        while (TIMSK3)
//        {
//            readLineSensor();
//            if (lineCnt >= 2) changeDist(MAG_LEN4);
//        }
        //drivi(-MAG_LEN2); while (TIMSK3);
        constDrivi(-MAG_LEN2, 800); while (TIMSK3);
    }
    delay(100);
//    readLineSensor();
//    while (!lineReading)
//    {
//        bool foundLine = false;
//        constDrivi(0.1, 1600);
//        while(TIMSK3)
//        {
//            lineFollow();
//            if (lineReading)
//            {
//                foundLine = true;
//                break;
//            }
//        }
//        if (foundLine) break;
//        constDrivi(-0.1, 1600);
//        while(TIMSK3)
//        {
//            lineFollow();
//            if (lineReading)
//            {
//                foundLine = true;
//                break;
//            }
//        }
//        if (foundLine) break;
//    }
//    byte counter = 20;
//    while ((lineReading != B00011000 || lineReading != B00010000 || lineReading != B00001000) && counter--){
//        if (stpercent > 100 && lineCnt < 2)
//        {
//            constStrafi(0.1, 1600);while(TIMSK3);
//        }
//        else if (stpercent < 100 && lineCnt < 2)
//        {
//            constStrafi(-0.1, 1600);while(TIMSK3);
//        }
//        lineFollow();
//    }
}

void localMove(){
    if (currColor<MAGENTA)
    {
        simpleMove(MAG_LEN-ERR_LEN, ERR_LEN, HYPO_LEN, destSpot-currSpot+1);
        //simpleMove(MAG_LEN-ERR_LEN, JUN_LEN, HYPO_LEN, destSpot-currSpot+1);
    }
    else
    {
        simpleMove(MAG_LEN2-ERR_LEN, ERR_LEN, SIDE_LEN, destSpot-currSpot+1);
        //simpleMove(MAG_LEN2-ERR_LEN, ERR_LEN+0.5, SIDE_LEN, destSpot-currSpot+1);
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

void deliverMove1(){
    destSpot = -1;

    int vector = colorVector[currColor][destColor];
    float startDist;
    if (vector == 0)
    {
        if (currColor<MAGENTA)
        {
            startDist = (MAG_LEN-ERR_LEN) + (7-currSpot)*HYPO_LEN  - JUN_LEN;
            simpleMove(startDist, ERR_LEN, HYPO_LEN, 5);
        }
        else
        {
            startDist = (MAG_LEN2-ERR_LEN) + (7-currSpot)*SIDE_LEN  - JUN_LEN;
            simpleMove(startDist, JUN_LEN-0.5, SIDE_LEN, 5);
        }
    }
    else if (abs(vector) == 90)
    {
        
        simpleMove(JUN_LEN, BOT_LEN, HYPO_LEN, 1);
        if (vector<0)
        {
            rotati(-45);while(TIMSK3);
            simpleMove((5-currSpot)*12, JUN_LEN, HYPO_LEN, 1);
            rotati(-45);while(TIMSK3);
        }
        else 
        {
            rotati(45);while(TIMSK3);
            simpleMove((5-currSpot)*12, JUN_LEN, HYPO_LEN, 1);
            rotati(45);while(TIMSK3);
        }
        simpleMove(HYPO_LEN-BOT_LEN, ERR_LEN, HYPO_LEN, currSpot+1);
        
//        byte steerMode = (vector<0) ? steerLeft : steerRight;
//        startDist = (MAG_LEN-ERR_LEN) + (5-currSpot)*12 + HYPO_LEN + 2*TUR_LEN;
//        simpleMove(startDist, ERR_LEN, HYPO_LEN, currSpot+1, steerMode);
        
    }
    else if (abs(vector) == 135)
    {
        
        if (currColor<MAGENTA)
        {
            simpleMove(JUN_LEN, BOT_LEN, HYPO_LEN, 1);
            startDist = (5-currSpot)*6 - BOT_LEN;
            //startDist = MAG_LEN-ERR_LEN + (5-currSpot)*6 + TUR_LEN;
            if (vector>0)
            {
                rotati(-45);while(TIMSK3);  //
                
                simpleMove(startDist, BOT_LEN, HYPO_LEN, 1, steerRight);
                rotati(90);while(TIMSK3);
            }
            else 
            {
                rotati(45);while(TIMSK3); //
                
                simpleMove(startDist, BOT_LEN, HYPO_LEN, 1, steerLeft);
                rotati(-90);while(TIMSK3);
            }
            simpleMove(SIDE_LEN-BOT_LEN, JUN_LEN-0.5, SIDE_LEN, currSpot+1);
        }
        else
        {
            simpleMove(MAG_LEN2-ERR_LEN, BOT_LEN, SIDE_LEN, 1);
            startDist = (5-currSpot)*6 + HYPO_LEN + TUR_LEN;
            if (vector>0)
            {
                rotati(90);while(TIMSK3);
                simpleMove(startDist, ERR_LEN, HYPO_LEN, currSpot+1, steerRight);
            }
            else 
            {
                rotati(-90);while(TIMSK3);
                simpleMove(startDist, ERR_LEN, HYPO_LEN, currSpot+1, steerLeft);
            }
        }
    }
    else if (abs(vector) == 45)
    {
        byte curveMode = (vector<0) ? curveLeft: curveRight;
        if (currColor<MAGENTA)
        {
            startDist = (MAG_LEN-ERR_LEN) + (5-currSpot)*HYPO_LEN + 2*SIDE_LEN;
            simpleMove(startDist, JUN_LEN-0.5, SIDE_LEN, 5, curveMode);
        }
        else
        {
            startDist = (MAG_LEN2-ERR_LEN) + (5-currSpot)*SIDE_LEN + 2*HYPO_LEN;
            simpleMove(startDist, ERR_LEN, HYPO_LEN, 5, curveMode);
        }
    }
    else if (vector == 180)
    {
        if (currColor<MAGENTA)
        {
            simpleMove(MAG_LEN-ERR_LEN, ERR_LEN, HYPO_LEN, 1);
            rotati(180);  while(TIMSK3);
            simpleMove(BOT_LEN, ERR_LEN, HYPO_LEN, currSpot+1);
        }
        else
        {
            simpleMove(MAG_LEN2-ERR_LEN, JUN_LEN, SIDE_LEN, 1);
            rotati(180);  while(TIMSK3);
            simpleMove(JUN_LEN, JUN_LEN-0.5, SIDE_LEN, currSpot+1);
        }
    }
    
    currSpot = 0;
    currColor = destColor;
    
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
        simpleMove(CEN_OUT_EDGE+SIDE_LEN, JUN_LEN-0.5, SIDE_LEN, 5);
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
    if (currColor<MAGENTA)
    {
        simpleMove(CEN_OUT_CR+HYPO_LEN, JUN_LEN, HYPO_LEN, 4-(destSpot-1));
        rotati(180);  while(TIMSK3);    // turn facing dest
        //simpleMove(HYPO_LEN-BOT_LEN, JUN_LEN, HYPO_LEN, 1);
        simpleMove(HYPO_LEN-BOT_LEN, ERR_LEN, HYPO_LEN, 1);
    }
    else 
    {
        simpleMove(CEN_OUT_EDGE+SIDE_LEN, BOT_LEN, SIDE_LEN, 4-(destSpot-1));
        rotati(180);  while(TIMSK3);    // turn facing dest
        //simpleMove(SIDE_LEN-(SIDE_LEN-BOT_LEN), ERR_LEN+0.5, SIDE_LEN, 1);
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
//            bool halfGray = true;
//            if (colorSamples[i] != VOID && colorSamples[i] != GRAY)
//            {
//                colorCounts[colorSamples[i]]++;
//            }
//            else if (colorSamples[i] == GRAY)
//            {
//                if (halfGray) colorCounts[colorSamples[i]]++;
//                halfGray = !halfGray;
//            }
            
            if (gameRound < 3)
            {
                if (colorSamples[i] != VOID && colorSamples[i] != currColor && colorSamples[i] != GRAY)
                {
                    colorCounts[colorSamples[i]]++;
                } 
            }
            else if (gameRound == 3)
            {
                bool halfGray = true;
                if (colorSamples[i] != VOID && colorSamples[i] != currColor && colorSamples[i] != GRAY)
                {
                    colorCounts[colorSamples[i]]++;
                }  
                else if (colorSamples[i] == GRAY)
                {
                    if (halfGray) colorCounts[colorSamples[i]]++;
                    halfGray = !halfGray;
                }
                
            }
            else
            {
                if (colorSamples[i] != VOID)
                {
                    colorCounts[colorSamples[i]]++;
                }  
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
        if (mostCounts < 3)
        {
            leaveToken();
            if (takeSamples==2)
            {
              //constDrivi(-0.8 ,1000);
              drivi(-0.8);
            }
            else if (takeSamples==1) 
            {
              //constDrivi(1.6 ,1000);
              drivi(1.6);
            }
            else if (takeSamples==0)
            {
              //constDrivi(-0.8 ,1000);
              drivi(-0.8);
            }
            while(TIMSK3);
            pickUp();
        }
        else break;
    }

    //final reading result
    destColor = coinColor;
    if (destColor == VOID) 
    {
        destColor = GRAY;
    }
    coinArray[currColor][currSpot] = false;
    coinCnt--;

//    byte colorArray[] = {YELLOW, BLUE, GREEN, BLUE, RED, CYAN, MAGENTA, GREEN, YELLOW, CYAN, RED, MAGENTA};
//    destColor = colorArray[coinCnt];
}

void pickup1(){
    pickUp();
    coinArray[currColor][currSpot] = false;
    coinCnt--;
    byte colorArray[] = {YELLOW, BLUE, GREEN, BLUE, RED, CYAN, MAGENTA, GREEN, YELLOW, CYAN, RED, MAGENTA};
    destColor = colorArray[coinCnt];
}

void deliver(){
    if (destColor != GRAY)
    {
        deliverMove();
        //constDrivi(1, 500);while(TIMSK3);
        drivi(1);while(TIMSK3);
        leaveToken(); //drop coin
//        rotati(10);while(TIMSK3);
//        rotati(-10);while(TIMSK3);
//        rotati(10);while(TIMSK3);
//        rotati(-10);while(TIMSK3);
//        rotati(10);while(TIMSK3);
//        rotati(-10);while(TIMSK3);
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
//        rotati(10);while(TIMSK3);
//        rotati(-10);while(TIMSK3);
//        rotati(10);while(TIMSK3);
//        rotati(-10);while(TIMSK3);
//        rotati(10);while(TIMSK3);
//        rotati(-10);while(TIMSK3);
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
