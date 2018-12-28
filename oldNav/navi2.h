#ifndef NAVIGATION_H
#define NAVIGATION_H

#include "lineFollow.h"
#include "colorSensor.h"
#include "servo.h"

#define JUNCTION (((PINA&B00010100) == B00010100) || ((PINA&B00101000) == B00101000) || ((PINA&B11000011) == B11000011))
#define HYPO_LEN 9
#define SIDE_LEN 6.5
#define BOT_LEN 3.75

#define MAG_LEN 2.5
#define MAG_LEN2 3.0
#define JUN_LEN 2.5
#define ERR_LEN 0.5

#define CEN_OUT_CR 5
#define CEN_OUT_EDGE 3
#define CEN_IN_CR 11
#define CEN_IN_EDGE 9

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

void simpleMove(float startDist, float endDist, float segDist, char viaPoints){
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
    
    for (float record = odometer(); viaPoints; record = odometer(), viaPoints--)
    {
        #define beforeMilestone (odometer() - record < distStack[viaPoints-1] - ERR_LEN)
        #define afterMilestone (odometer() - record > distStack[viaPoints-1] + JUN_LEN)

        /*
        if (viaPoints%2)
        {
            constDrivi(dist, 300);
        }
        else 
        {
            constDrivi(dist, 500);
        }
        */
        TIMSK3 = 0;
        delay(250);
        constDrivi(dist, 300);
        //drivi(dist);
        
        //look for viaPoint
        while (beforeMilestone)
        {
            lineFollow();
            if (!TIMSK3) 
            {
                constDrivi(12, 300);
            }
        }
        while (!JUNCTION)//  && !afterMilestone)  
        {
            lineFollow();
            if (!TIMSK3) 
            {
                constDrivi(12, 300);
            }
        }

        //at viaPoint
        dist -= distStack[viaPoints-1];    
    }
    constDrivi(dist, 500);
    //drivi(dist);
    while (TIMSK3)
    {
        readLineSensor();
    }

}


void locateMove(){
    if (currColor<MAGENTA)
    {
        simpleMove(BOT_LEN, ERR_LEN, HYPO_LEN, destSpot-currSpot+1);
        delay(500);
        constDrivi(-MAG_LEN2, 600); while (TIMSK3);
        //drivi(-MAG_LEN); while (TIMSK3);
    }
    else
    {
        simpleMove(SIDE_LEN-BOT_LEN, ERR_LEN, SIDE_LEN, destSpot-currSpot+1);
        delay(500);
        constDrivi(-MAG_LEN, 600); while (TIMSK3);
        //drivi(-MAG_LEN); while (TIMSK3);
    }
    
    currSpot = destSpot;
}

void centerinMove(){
    if (currColor<MAGENTA)
    {
        simpleMove(MAG_LEN, CEN_IN_CR, HYPO_LEN, 4-currSpot+1);
    }
    else
    {
        simpleMove(MAG_LEN, CEN_IN_EDGE, SIDE_LEN, 4-currSpot+1);
    }
}

void deliverMove(){
    destSpot = -1;
    if (currColor<MAGENTA)
    {
        simpleMove(MAG_LEN, CEN_IN_CR, HYPO_LEN, 4-currSpot+1);
    }
    else
    {
        simpleMove(MAG_LEN, CEN_IN_EDGE, SIDE_LEN, 4-currSpot+1);
    }
    //float segDist = (currColor<MAGENTA) ? HYPO_LEN : SIDE_LEN;
    //simpleMove(MAG_LEN, segDist*1.3, segDist, 4-currSpot+1);
    
    rotati(colorVector[currColor][destColor]);  while(TIMSK3);    // turn facing dest
    currColor = destColor;
    destSpot = -1;
    if (currColor<MAGENTA)
    {
        simpleMove(CEN_OUT_CR+HYPO_LEN, JUN_LEN, HYPO_LEN, 5);
    }
    else
    {
        simpleMove(CEN_OUT_EDGE+SIDE_LEN, BOT_LEN, SIDE_LEN, 5);
    }
    
    //segDist = (destColor<MAGENTA) ? HYPO_LEN : SIDE_LEN;
    //float starDist = (destColor<MAGENTA) ? JUN_LEN : JUN_LEN;
    //float endDist = (destColor<MAGENTA) ? CEN_LEN : BOT_LEN+JUN_LEN;
    //simpleMove(starDist, endDist, segDist, 6);
    //simpleMove(starDist+segDist, endDist, segDist, 5);
    currSpot = 0;
    
}

void centeroutMove(){
    rotati(colorVector[currColor][destColor]);  while(TIMSK3);    // turn facing dest

    currColor = destColor;
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
    
    //float segDist = (destColor<MAGENTA) ? HYPO_LEN : SIDE_LEN;
    //float starDist = (destColor<MAGENTA) ? JUN_LEN : JUN_LEN;
    //float endDist = (destColor<MAGENTA) ? CEN_LEN : BOT_LEN+JUN_LEN;
    //simpleMove(starDist, endDist, segDist, 4-(destSpot-1)+1);
    //simpleMove(starDist+segDist, endDist, segDist, 4-(destSpot-1));
    //rotati(180);  while(TIMSK3);    // turn facing dest
    //simpleMove(segDist-BOT_LEN, ERR_LEN, segDist, 1);
    
    delay(500);
    constDrivi(-MAG_LEN, 600); while (TIMSK3);
    //drivi(-MAG_LEN); while (TIMSK3);
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
                    currSpot = destSpot;
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
    char wabble = (-40);
    char tries = 2;
    coinColor = VOID;
    while (coinColor == VOID && tries)
    {
        constDrivi(-0.5, 600); while (TIMSK3);
        wabble = (-40);
        pickUp();   //coin pickup
        while (wabble < 40)
        {
            readColor();
            if (coinColor != VOID)
            {
                destColor = coinColor;
                break;
            }
            else 
            {
                wabble += 8;
                sliderTurn(ENDSLIDER+wabble, 500);
            }
        }
        if (coinColor == VOID)
        {
            leaveToken();
            tries--;
        }
    }

    if (!tries) destColor = GRAY;
    
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
        float segDist = (currColor<MAGENTA) ? HYPO_LEN : SIDE_LEN;
        simpleMove(MAG_LEN, MAG_LEN, segDist, 4-currSpot+1);
        currSpot = 5;
        leaveToken(); //drop coin
        if (currColor < MAGENTA)
        {
            constDrivi(HYPO_LEN-ERR_LEN,400);
            //drivi(HYPO_LEN-ERR_LEN);
            while(TIMSK3);
        }
        else 
        {
            constDrivi(SIDE_LEN-ERR_LEN,400);
            //drivi(SIDE_LEN-ERR_LEN);
            while(TIMSK3);
        }
        
    }
}


//recycle
/*

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

void locateMove2(float startDist, float endDist){
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
