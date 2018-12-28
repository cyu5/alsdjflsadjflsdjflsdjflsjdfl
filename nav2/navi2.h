#ifndef NAVIGATION_H
#define NAVIGATION_H

#include "lineFollow.h"
#include "colorSensor.h"
#include "servo.h"

#define HYPO_LEN 9.5
#define SIDE_LEN 7
#define BOT_LEN 3.75

#define MAG_LEN 3
#define MAG_LEN2 3.5
#define JUN_LEN 2.5
#define ERR_LEN 0.5

#define CEN_OUT_CR 5
#define CEN_OUT_EDGE 3
#define CEN_IN_CR 10
#define CEN_IN_EDGE 8.5

char currColor, destColor, currSpot, destSpot, coinCnt;
unsigned long gameTime;
bool coinArray[NUM_COLORS-1][4];
int colorVector2[NUM_COLORS][NUM_COLORS] = {
                                          {  180  ,   90  ,    0  ,  -90  ,  135  ,  -45  , -135},
                                          {  -90  ,  180  ,   90  ,    0  , -135  ,   45  ,  -45},
                                          {    0  ,  -90  ,  180  ,   90  ,  -45  ,  135  ,   45},
                                          {   90  ,    0  ,  -90  ,  180  ,   45  , -135  ,  135},
                                          { -135  ,  135  ,   45  ,  -45  ,  180  ,    0  ,  -90},
                                          {   45  ,  -45  , -135  ,  135  ,    0  ,  180  ,   90},
                                          {  -45  , -135  ,  135  ,   45  ,  -90  ,   90  ,  180}
                                          };

int colorVector[NUM_COLORS][NUM_COLORS] = {
                                          {    0  ,  -90  ,    0  ,   90  ,  -45  ,  135  ,   45},
                                          {   90  ,    0  ,  -90  ,  180  ,   45  , -135  ,  135},
                                          {  180  ,   90  ,    0  ,  -90  ,  135  ,  -45  , -135},
                                          {  -90  ,  180  ,   90  ,    0  , -135  ,   45  ,  -45},
                                          {   45  ,  -45  , -135  ,  135  ,    0  ,  180  ,   90},
                                          { -135  ,  135  ,   45  ,  -45  ,  180  ,    0  ,  -90},
                                          {  135  ,   45  ,  -45  , -135  ,   90  ,  -90  ,    0}
                                          };



void navigation(char currColor, char destColor, char currSpot, char destSpot){
    if (currColor != destColor) //region move
    {
        //centerinMove();
        rotati(colorVector[currColor][destColor]); while(TIMSK3);    // turn facing dest
    }
    else if (currSpot > destSpot)
    {
        //centerout and forward
    }
    else 
    {
        //centerin and backward
    }   
}
/*
void newMove()(float startDist, float endDist){
    char viaPoints = currSpot - destSpot;
    char incSpot = 1;
    if (viaPoints < 0)
    {
        incSpot *= -1;   
        viaPoints *= -1; 
    }

    float segDist = (currSpot<MAGENTA) ? HYPO_LEN : SIDE_LEN;
    float distStack[viaPoints];
    float dist = endDist * -incSpot;
    if (viaPoints)
    {
        for (char i=0; i<viaPoints-1; i++)
        {
            distStack[i] = segDist * -incSpot;
            dist += distStack[i];
        }
        distStack[viaPoints-1] = startDist * -incSpot;
        dist += distStack[viaPoints-1];
    } 
    
    float junMark = odometer();
    while (currSpot+=incSpot <= destSpot)
    {
        #define JUNCTION (((lineReading&B00010100) == B00010100) || ((lineReading&B00101000) == B00101000) || ((lineReading&B11000011) == B11000011))
        #define LINE  (!(lineReading&B11100111) && lineReading)
        #define onMilestone (odometer() - junMark >= abs(distStack[viaPoints]) - JUN_LEN)
        #define offMilestone (odometer() - junMark >= abs(distStack[viaPoints]) + JUN_LEN)

        constDrivi(distStack[viaPoints] - (odometer() - junMark), 300);
        
        //look for viaPoint
        while (!JUNCTION || !onMilestone)
        {
            lineFollow();
            if (!TIMSK3)
            {
                delay(500);
                constDrivi(-incSpot, 500);
            }
        }
        junMark = odometer();
        changePeriod(800);
        while(TIMSK3);
    }
    TIMSK3 = 0;
    delay(500);
    readLineSensor();
    constDrive((endDist - (odometer() - junMark)) * -incSpot, 500);
}
*/

void simpleMove(float startDist, float endDist, float segDist, char viaPoints, bool moveBack = false){
    float distStack[viaPoints];

    Serial.println("start simpleMove");
    Serial.print(startDist);Serial.print('\t');
    Serial.print(endDist);Serial.print('\t');
    Serial.print(segDist);Serial.print('\t');
    Serial.println((byte)viaPoints);Serial.print('\n');
    if (viaPoints)
    {
        for (char i=0; i<viaPoints-1; i++)
        {
            distStack[i] = segDist;
            Serial.print(i);Serial.print("-distStack: ");Serial.print(distStack[i]);Serial.print('\t');
            if (moveBack) distStack[i] *= -1;
        }
        distStack[viaPoints-1] = startDist;
        Serial.print(viaPoints-1);Serial.print("-distStack: ");Serial.print(distStack[viaPoints-1]);Serial.print('\n');
        if (moveBack) distStack[viaPoints-1] *= -1;
    } 

    
    
    float junMark = odometer();
    while (viaPoints--)
    {
        #define JUNCTION (((lineReading&B00010100) == B00010100) || ((lineReading&B00101000) == B00101000) || ((lineReading&B11000011) == B11000011))
        #define LINE  (!(lineReading&B11100111) && lineReading)
        #define onMilestone (odometer() - junMark >= abs(distStack[viaPoints]) - JUN_LEN)
        #define offMilestone (odometer() - junMark >= abs(distStack[viaPoints]) + JUN_LEN)

        constDrivi(distStack[viaPoints] - (odometer() - junMark), 300);;
        Serial.print("drive dist:");Serial.println(distStack[viaPoints]);
        
        //look for viaPoint
        bool undershoot = false;
        Serial.print("odometer: ");Serial.print(odometer());Serial.print("junMark: ");Serial.println(junMark);
        Serial.print("beforeMilestone: ");Serial.print(odometer() - junMark); Serial.print(">="); Serial.println(abs(distStack[viaPoints]) - JUN_LEN);
        while (!onMilestone)
        {
            lineFollow();
            if (!TIMSK3)
            {
                undershoot = true;
                if (moveBack) {constDrivi(-6, 1800);}
                else  {constDrivi(6, 1800);}
            }
        }
        changePeriod(1800);
        Serial.print("odometer: ");Serial.print(odometer());Serial.print("junMark: ");Serial.println(junMark);
        Serial.println("wait for junction");
        while (!JUNCTION)
        {
            lineFollow();
            if (!TIMSK3)
            {
                undershoot = true;
                if (moveBack) {constDrivi(-6, 1800);}
                else  {constDrivi(6, 1800);}
            }
        }
        TIMSK3 = 0;
        delay(1000);
        junMark = odometer();
        changePeriod(1800);
        TIMSK3 = 0x06;
        
        Serial.print("odometer: ");Serial.print(odometer());Serial.print("junMark: ");Serial.println(junMark);
        Serial.println("afterMilestone");
        
        if (!undershoot) 
        {
            while(TIMSK3);
        }

    }
    readLineSensor();
    constDrivi(endDist - (odometer() - junMark), 500); while(TIMSK3);
    Serial.println("end simpleMove");
}

void fastMove(float startDist, float endDist, float segDist, char viaPoints, bool moveBack = false){
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
    drivi(dist);
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
    }
    constDrivi(endDist - (odometer() - junMark), 500);  while(TIMSK3);
}

void alignMove(){
    /*int pos = steerTable[PINA];
    while (pos != 100)
    {
        if (pos > 0)
        {
            constStrafe(0.1 , 1000);
        }
        else 
        {
            constStrafe(-0.1 , 1000);    
        }
        pos = steerTable[PINA];
    }
    TIMSK3 = 0;
    */
    if (currColor<MAGENTA)
    {
        constDrivi(-MAG_LEN2, 600); while (TIMSK3);
    }
    else
    {
        constDrivi(-MAG_LEN, 600); while (TIMSK3);
    }
}

void localMove(){
    if (currColor<MAGENTA)
    {
        simpleMove(BOT_LEN, ERR_LEN, HYPO_LEN, destSpot-currSpot+1);
    }
    else
    {
        simpleMove(SIDE_LEN-BOT_LEN, ERR_LEN, SIDE_LEN, destSpot-currSpot+1);
    }
    alignMove();
    currSpot = destSpot;
}

void centerinMove(){
    if (currSpot == 5)  return;
    if (currColor<MAGENTA)
    {
        if (destColor == currColor && destSpot < 4)
        {
            simpleMove(BOT_LEN, ERR_LEN, HYPO_LEN, destSpot-currSpot+1);
        }
        else 
        {
            simpleMove(MAG_LEN, CEN_IN_CR, HYPO_LEN, 4-currSpot+1);
        }
    }
    else
    {
        if (destColor == currColor && destSpot < 4)
        {
            simpleMove(SIDE_LEN-BOT_LEN, ERR_LEN, SIDE_LEN, destSpot-currSpot+1);
        }
        else 
        {
            simpleMove(MAG_LEN, CEN_IN_EDGE, SIDE_LEN, 4-currSpot+1);
        }
    }

    currSpot = 5;
}

void centeroutMove(){
    if (currColor<MAGENTA)
    {
        simpleMove(CEN_OUT_CR+HYPO_LEN, JUN_LEN, HYPO_LEN, 5);
    }
    else
    {
        simpleMove(CEN_OUT_EDGE+SIDE_LEN, BOT_LEN, SIDE_LEN, 5);
    }
    currSpot = -1;
}

void deliverMove(){
    centerinMove(); 
    rotati(colorVector[currColor][destColor]); while(TIMSK3);currColor = destColor;    // turn facing dest
    centeroutMove();

    constDrivi(0.2, 800);while(TIMSK3);
    leaveToken(); 
    constDrivi(-0.2, 800);while(TIMSK3);
    rotati(180);while(TIMSK3);currSpot=0;
}

void regionMove(){

    centerinMove();
    rotati(colorVector[currColor][destColor]);  while(TIMSK3);currColor = destColor;    // turn facing dest
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
}


bool getCoin(){
    destColor = currColor;
    for (destSpot=currSpot; destSpot<4; destSpot++)
    {
        if (coinArray[destColor][destSpot])
        {
            return true;
        }
    }

    for (destSpot=3; destSpot>=0; destSpot--)
    {
        for (destColor=YELLOW; destColor<GRAY; destColor++)
        {
            if (coinArray[destColor][destSpot])
            {
                return false;
            }
        }
    }
    
}

void pickupMove(){
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
              constDrivi(-0.5 ,1000);while(TIMSK3);
            }
            else if (takeSamples==1) 
            {
              constDrivi(1 ,1000);while(TIMSK3);
            }
            else if (takeSamples==0)
            {
              constDrivi(-0.5 ,1000);while(TIMSK3);
            }
            pickUp();
        }
        else break;
    }

}




//recycle
/*

void pickup(){
    char testArray[12] = {GREEN, CYAN, BLUE, GREEN, RED, MAGENTA, YELLOW, CYAN, MAGENTA, RED, YELLOW, BLUE};
    char testArrayt[12] = {GRAY, BLUE, YELLOW, GRAY, GREEN, GRAY, CYAN, GRAY, RED, MAGENTA, GRAY, GRAY};
    
    
    //destColor = testArray[12-coinCnt];
    coinArray[currColor][currSpot] = false;
    coinCnt--;
}

void deliver(){
    if (destColor != GRAY)
    {
        deliverMove();
        //currColor = destColor;
        leaveToken(); //drop coin
        rotati(180); while (TIMSK3);
    }
    else 
    {
        
//        destColor = currColor;
//        float segDist = (currColor<MAGENTA) ? HYPO_LEN : SIDE_LEN;
//        simpleMove(MAG_LEN, MAG_LEN, segDist, 4-currSpot+1);
//        currSpot = 5;
        
        centerinMove();
        leaveToken(); //drop coin
        
//        if (currColor < MAGENTA)
//        {
//            constDrivi(HYPO_LEN-ERR_LEN,400);
//            //drivi(HYPO_LEN-ERR_LEN);
//            while(TIMSK3);
//        }
//        else 
//        {
//            constDrivi(SIDE_LEN-ERR_LEN,400);
//            //drivi(SIDE_LEN-ERR_LEN);
//            while(TIMSK3);
//        }
        
        
    }
}


void inwardMove(){
    if (currColor<MAGENTA)
    {
        if (currSpot == 5)
        {
            simpleMove(MAG_LEN, CEN_IN_CR, HYPO_LEN, 4-currSpot+1);
            delay(500);
            constDrivi(-MAG_LEN, 600); while (TIMSK3);
            //drivi(-MAG_LEN); while (TIMSK3);
        }
        else simpleMove(BOT_LEN, ERR_LEN, HYPO_LEN, destSpot-currSpot+1);
    }
    else 
    {
        if (currSpot == 5)  
        {
            simpleMove(MAG_LEN, CEN_IN_EDGE, SIDE_LEN, 4-currSpot+1);
            delay(500);
            constDrivi(-MAG_LEN, 600); while (TIMSK3);
            //drivi(-MAG_LEN); while (TIMSK3);
        }
        else simpleMove(SIDE_LEN-BOT_LEN, ERR_LEN, SIDE_LEN, destSpot-currSpot+1);   
    }
}

void searchoutMove(){
    float segDist = (currColor<MAGENTA) ? HYPO_LEN : SIDE_LEN;
    simpleMove(MAG_LEN, segDist*1.3, segDist, 4-currSpot+1);
    rotati(colorVector[currColor][destColor]);  while(TIMSK3);    // turn facing dest
    
    segDist = (destColor<MAGENTA) ? HYPO_LEN : SIDE_LEN;
    float starDist = (destColor<MAGENTA) ? JUN_LEN : JUN_LEN;
    float endDist = (destColor<MAGENTA) ? CEN_LEN : BOT_LEN+JUN_LEN;
    //simpleMove(starDist, endDist, segDist, 4-(destSpot-1)+1);

    simpleMove(starDist+segDist, endDist, segDist, 4-(destSpot-1));
    rotati(180);  while(TIMSK3);    // turn facing dest
    
    simpleMove(segDist-BOT_LEN, ERR_LEN, segDist, 1);
    delay(500);
    constDrivi(-MAG_LEN, 600); while (TIMSK3);
}

void lineFollow2(){
    int steerTable[NUM_SENSORS] = {-739, -417, -236, -133, 133, 236, 417, 739};
    readLineSensor();
    //stpercent = steerTable[position];
}

void localMove2(float startDist, float endDist){
    //bookmark: separate out startDist(followLine) and endDist(readLine), 
    #define LOCATE 1
    #define DELIVER 2
        
    char type = LOCATE;
    //float startDist;
    //float endDist;
    float currSeg = (currColor<MAGENTA) ? HYPO_LEN : SIDE_LEN;
    float destSeg = (destColor<MAGENTA) ? HYPO_LEN : SIDE_LEN;
    float dist;
    float record;
    if (type == LOCATE)
    {
        startDist = BOT_LEN;
        endDist = ERR_LEN;
        dist = startDist + currSeg*(destSpot-currSpot) + endDist;
    }
    else if (type == DELIVER)
    {
        startDist = MAG_LEN;
        endDist = currSeg + CEN_LEN;
        destSpot = 4;
        dist = startDist + (currSeg*(destSpot-currSpot)) + endDist;
    }
    

    
    constDrivi(dist, 400);
    ////drivi(dist);
    readLineSensor();
    
    while (currSpot <= destSpot)
    {
        //look for next viaPoint
        while (!JUNCTION)  lineFollow();

        //at viaPoint
        if (dist == currSeg)  dist -= startDist;
        else dist -= currSeg;
        
        TIMSK3 = 0;
        delay(1000);
        ////drivi(dist);
        
        if (dist >= JUN_LEN)
        {
            //get pass viaPoint
            record = odometer();
            while (odometer() - record < JUN_LEN) readLineSensor();
            currSpot++;
            constDrivi(dist-JUN_LEN, 400);
        }
        else constDrivi(dist, 400);
    }
    while (TIMSK3); readLineSensor();
    
    if (type == LOCATE)
    {
        delay(500);
        constDrivi(-MAG_LEN, 600); while (TIMSK3);
        return;
    }
    else if (type == DELIVER)
    {        
        rotati(colorVector[currColor][destColor]);  while(TIMSK3);    // turn facing dest
        
//bookmark: flip dest and curr, destSeg depend on destColor, consider new names for them, curr and dest 
//are higher level variables, maybe inward and outward is for this lower level. Front and back is sandwiched 
//between these two levels and is dependance on degree of turn. North/South and east/west booleans are lower
//is directly calculated from turns and used to derive front and back..        

        startDist = destSeg - CEN_LEN;
        endDist = BOT_LEN;
        destSpot = -1;
        currSpot = 3;
        dist = startDist + destSeg*(4-destSpot) + endDist;
    }
    
    // move out square
    constDrivi(dist, 400);
    ////drivi(dist);
    
    record = odometer();
    while (odometer() - record < destSeg) readLineSensor();
    dist -= destSeg - CEN_LEN;
    
    while (destSpot <= currSpot)
    {
        //look for next viaPoint
        while (!JUNCTION)  lineFollow();

        //at viaPoint
        dist -= destSeg;
        TIMSK3 = 0;
        delay(1000);
        constDrivi(dist, 400);
        ////drivi(dist);

        if (dist >= JUN_LEN)
        {
            //get pass viaPoint
            record = odometer();
            while (odometer() - record < JUN_LEN) readLineSensor();
            currSpot--;
            constDrivi(dist-JUN_LEN, 400);
        }
        else constDrivi(dist, 400);
    }
    
    while (TIMSK3); readLineSensor();
    
}

void deliverMove(float startDist, float endDist){
    float currSeg = (currColor<MAGENTA) ? HYPO_LEN : SIDE_LEN;
    float destSeg = (destColor<MAGENTA) ? HYPO_LEN : SIDE_LEN;
    float currDist = startDist + (currSeg*(5-currSpot)) + CEN_LEN;
    float destDist = endDist + destSeg*(5-destSpot) - CEN_LEN;
    float dist = currDist;
    float record;


    constDrivi(dist, 400);
    ////drivi(dist);
    readLineSensor();
    
    while (currSpot < 5)
    {
            //look for next viaPoint
            while (!JUNCTION)  lineFollow();
    
            //at viaPoint
            if (dist == currDist)  dist -= startDist;
            else dist -= currSeg;
            
            TIMSK3 = 0;
            delay(1000);
            constDrivi(dist, 400);
            ////drivi(dist);
    
            //get pass viaPoint
            record = odometer();
            while (odometer() - record < JUN_LEN) readLineSensor();
            currSpot++;
    }
    
    while (TIMSK3); readLineSensor();   // into square and/or stopping
    rotati(colorVector[currColor][destColor]);  while(TIMSK3);    // turn facing dest
    currColor = destColor;
    
    // move out square
    dist = destDist;
    constDrivi(dist, 400);
    ////drivi(dist);
    
    record = odometer();
    while (odometer() - record < destSeg) readLineSensor();
    dist -= destSeg - CEN_LEN;
    currSpot = 3;

    while (currSpot > -2)   //off by -1, facing inward
    {
        //look for next viaPoint
        while (!JUNCTION)  lineFollow();

        //at viaPoint
        dist -= destSeg;
        TIMSK3 = 0;
        delay(1000);
        constDrivi(dist, 400);
        ////drivi(dist);
        if (dist <= 0) break;


        //get pass viaPoint
        record = odometer();
        while (odometer() - record < JUN_LEN) readLineSensor();
        currSpot--;
    }
    
    while (TIMSK3); readLineSensor();
}

void masterMove(float startDist, float endDist){
    float currSeg = (currColor<MAGENTA) ? HYPO_LEN : SIDE_LEN;
    float destSeg = (destColor<MAGENTA) ? HYPO_LEN : SIDE_LEN;
    float currDist = startDist + currSeg*(6-currSpot) + CEN_LEN;
    float destDist = endDist + destSeg*(5-destSpot) - CEN_LEN;
    float dist;
    float record;
    char type;
    #define ACROSS 2
    #define LOCAL 1
    #define NORMAL 0
    #define BACK 4
    #define HOME 3
    
    if (colorVector[currColor][destColor] == 0) 
    {
        if (destColor == GRAY)
        {
            type = HOME;
            dist = currDist;
        }
        else 
        {
            type = ACROSS;
            dist = currDist+destDist;
        }
    }
    else if (colorVector[currColor][destColor] == 180)
    {
        if (currSpot <= destSpot)
        {
            type = LOCAL;
            currDist = startDist+endDist+(currSeg*(destSpot-currSpot));
            dist = currDist;
        }
        else  
        {
            type = BACK;
            currDist = 2*(currSeg-BOT_LEN)+startDist;
            dist = currDist;
        }
    }
    else
    {
        type = NORMAL;
        dist = currDist;
    }
    
    
    constDrivi(dist, 400);
    ////drivi(dist);
    
    if (type < BACK)    //heading center
    {
        while (currSpot < 5)
        {
            //look for next viaPoint
            while (!JUNCTION)  lineFollow();
    
            //at viaPoint
            currSpot++;
            if (dist == currDist)  dist -= startDist;
            else dist -= currSeg;
            
            TIMSK3 = 0;
            delay(1000);
            constDrivi(dist, 400);
            ////drivi(dist);
            if (dist <= 0) return;
    
            //get pass viaPoint
            record = odometer();
            while (odometer() - record < JUN_LEN) readLineSensor();
        }
        if (type == NORMAL)
        {
            while (TIMSK3); readLineSensor();   // into square and/or stopping
            rotati(colorVector[currColor][destColor]);  while(TIMSK3);    // turn facing dest
            currColor = destColor;
            
            // move out square
            dist = destDist;
            constDrivi(dist, 400);
            ////drivi(dist);
            
            record = odometer();
            while (odometer() - record < destSeg - CEN_LEN) readLineSensor();
            dist -= destSeg - CEN_LEN;
            
        }
        else if (type == HOME)
        {
            while (TIMSK3); readLineSensor();   // into square and/or stopping
            rotati(colorVector[currColor][destColor]);  while(TIMSK3);    // turn facing home
            drivi(42) while(TIMSK3); return;        //return home
        }
        else 
        {
            record = odometer();
            while (odometer() - record < 2*currSeg) readLineSensor();
            dist -= 2*currSeg;
            
        }
        currSpot = 3;
    }
    else
    {
        rotati(180);  while(TIMSK3);    // turn facing dest
        currColor = destColor;
    }
    
    while (currSpot > destSpot)   //off by -1, facing inward
    {
        //look for next viaPoint
        while (!JUNCTION)  lineFollow();

        //at viaPoint
        currSpot--;
        dist -= destSeg;
        TIMSK3 = 0;
        delay(1000);
        constDrivi(dist, 400);
        ////drivi(dist);
        if (dist <= 0) return;


        //get pass viaPoint
        record = odometer();
        while (odometer() - record < JUN_LEN) readLineSensor();
    }
    
    while (TIMSK3); readLineSensor();
    
    
}


void makeTrip(char type, char viaPoints){
    
    float viaDist = (currColor<MAGENTA) ? HYPO_LEN : SIDE_LEN;
    float dist = viaDist * viaPoints;

    if (type == MEETUP) dist -= BOT_LEN;
    else if (type == OUTCEN) dist += CEN_CR_LEN;
    else if (type == INCEN) dist += BOT_LEN + viaDist; 
    
    constDrivi(dist, 400);
    ////drivi(dist);
    
    while (viaPoints)
    {


        //get pass viaPoint
        while (lineCnt > 1) readLineSensor();
        float record = odometer();
        while (odometer() - record < JUN_LEN);

        //look for next viaPoint
        while (lineCnt < 2)  lineFollow();

        //at viaPoint
        viaPoints--;
        dist -= viaDist;
        if (type == OUTCEN) dist -= CEN_CR_LEN;
        
        TIMSK3 = 0;
        delay(1000);
        constDrivi(dist, 400);
        ////drivi(dist);

    }
    while (TIMSK3) readLineSensor();
}


void intoSquare(float viaDist, char viaPoints){
    float dist = viaDist * viaPoints + BOT_LEN;
    constDrivi(dist, 300);
    ////drivi(dist);
    while (viaPoints)
    {
        while (lineCnt > 1) readLineSensor();
        float recordStep = odometer();
        while (odometer() - recordStep < BOT_LEN);

        //pass previous viaPoint
        while (lineCnt < 2)  lineFollow();

        //at viaPoint
        dist = viaDist * --viaPoints + BOT_LEN + viaDist;
        constDrivi(dist, 300);
        ////drivi(dist);
    }
    while (TIMSK3);
}

void outSquare(float dist, char viaPoints){
    float dist = viaDist * viaPoints + viaDist;
    constDrivi(dist, 300);
    ////drivi(dist);
    while (viaPoints)
    {
        while (lineCnt > 1) readLineSensor();
        float recordStep = odometer();
        while (odometer() - recordStep < BOT_LEN);

        //pass previous viaPoint
        while (lineCnt < 2)  lineFollow();

        //at viaPoint
        dist = viaDist * --viaPoints + BOT_LEN;
        constDrivi(dist, 300);
        ////drivi(dist);
    }
    while (TIMSK3);
}


*/
 

#endif
