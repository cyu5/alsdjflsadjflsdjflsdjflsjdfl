#include "navi2.h" 


void setup() {
    //delay(2500);                //wait for me to turn on the switch
    Serial.begin(9600);         //for debugging
    setupColor();                 //colorSensor
    setupTimer();               //interrupt motion
    setupLine();
    setupServo();
    while (1)
    {
        drivi(3*20);while(TIMSK3)lineFollow();
        rotati(180);pickup();while(TIMSK3)noFollow();
        drivi(3*20);while(TIMSK3)lineFollow();
        rotati(180);pickup();while(TIMSK3)noFollow();
    }
    
//    testDeliver(YELLOW, BLUE, 0);
//    rotati(180);while(TIMSK3);
//    testDeliver(BLUE, GREEN, 0);
//    rotati(180);while(TIMSK3);
//    testDeliver(GREEN, BLUE, 0);
//    while(1);


    
//    while(1)
//    {
//        drivi(100); while(TIMSK3);
//        delay(500);
//        drivi(-100); while(TIMSK3);
//        delay(500);
//    }
//    testAlignMove(YELLOW);
//    while(1);
    startGame(3);
}


void loop() {

    while (millis() < (gameTime - 10000) && coinCnt)
    {
        getCoin();
        pickup();
        deliver();
    }
    returnHome();
    while(1);
}

void startGame(char roundNum){

    gameRound = roundNum;
    switch (roundNum)
    {
        case 1:
            coinCnt = 12;
            gameTime = 300000;
            for (char i=0; i<NUM_COLORS-1; i++)
            {
                 coinArray[i][1] = true;   
                 coinArray[i][3] = true; 
            }
        break;
        case 2:
            coinCnt = 18;
            gameTime = 360000;
            for (char i=0; i<NUM_COLORS-1; i++)
            {
                coinArray[i][1] = true;  
                coinArray[i][2] = true;  
                coinArray[i][3] = true;
            }
        break;
        case 3:
            coinCnt = 24;
            gameTime = 480000;
            for (char i=0; i<NUM_COLORS-1; i++)
            {
                coinArray[i][0] = true; 
                coinArray[i][1] = true; 
                coinArray[i][2] = true;  
                coinArray[i][3] = true;
            }
        break;
        default:
            coinCnt = 14;
            gameTime = 240000;
            for (char i=0; i<NUM_COLORS-1; i++)
            {
                coinArray[i][0] = true; 
                coinArray[i][1] = true; 
                coinArray[i][2] = true;  
                coinArray[i][3] = true;
            }
        break;
    }
    
    drivi(43.5);  while (TIMSK3);  
    rotati(-45);  while (TIMSK3);
    currColor = YELLOW; currSpot = 5;
}

void testAlignMove(char nowColor){
    currColor = nowColor;
    alignMove();
    pickUp();
}

void testDeliver(char nowColor, char finalColor, char nowSpot){
    currColor = nowColor;
    destColor = finalColor;
    currSpot = nowSpot;
    deliverMove();
}

void testReturn(){
    drivi(43.5);  while(TIMSK3); 
    drivi(-42);  while(TIMSK3); 
    rotati(-90); while(TIMSK3);
    drivi(18);
    float returnDist;
    float junMark;
    while (TIMSK3)
    {
        if (PINA)
        {
            junMark = odometer();
            while (odometer() - junMark < JUN_LEN);
            while (TIMSK3)
            {
                if (PINA)
                {
                    junMark = odometer();
                }
            }
            returnDist = odometer() - junMark;
            drivi(-returnDist-4.25);
            return;   
        }
    }
    
    drivi(-30);
    while (TIMSK3)
    {
        if (PINA)
        {
            junMark = odometer();
        }
    }
    returnDist = odometer() - junMark;
    drivi(-returnDist-4.25);
}

void returnHome(){
    if (currSpot != 5)
    {
        centerinMove();
    }
    destColor=GRAY;
    rotati(colorVector[currColor][destColor]); while(TIMSK3);
    drivi(42);while(TIMSK3);
    rotati(90);while(TIMSK3);
    drivi(18);
    float returnDist;
    float junMark;
    while (TIMSK3)
    {
        if (PINA)
        {
            junMark = odometer();
            while (odometer() - junMark < JUN_LEN);
            while (TIMSK3)
            {
                if (PINA)
                {
                    junMark = odometer();
                }
            }
            returnDist = odometer() - junMark;
            drivi(-returnDist-4.25);
            return;   
        }
    }
    
    drivi(-30);
    while (TIMSK3)
    {
        if (PINA)
        {
            junMark = odometer();
        }
    }
    returnDist = odometer() - junMark;
    drivi(-returnDist-4.25);
    
//    rotati(90); while(TIMSK3);
//    constDrivi(18, 600);
//    
//    float returnDist;
//    float junMark;
//    while (TIMSK3)
//    {
//        if (PINA)
//        {
//            junMark = odometer();
//            while (odometer() - junMark < JUN_LEN);
//            while (TIMSK3)
//            {
//                if (PINA)
//                {
//                    constDrivi(-4.25, 600);
//                    return;
//                }
//            }
//            returnDist = odometer() - junMark;
//            constDrivi(-returnDist-4.25, 600);
//            return;   
//        }
//    }
//    constDrivi(-30, 600);
//    while (TIMSK3)
//    {
//        if (PINA)
//        {
//            constDrivi(-4.25, 600);
//            return;
//        }
//        
//    }
    
}

