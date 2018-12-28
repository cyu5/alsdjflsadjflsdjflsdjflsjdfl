#include "navi2.h" 


void setup() {
    delay(2500);                //wait for me to turn on the switch
    Serial.begin(9600);         //for debugging
    setupColor();                 //colorSensor
    setupTimer();               //interrupt motion
    setupLine();
    setupServo();

    /*
    drive(-42);
    drive(42);
    rotate(90);
    constDrivi(12, 600);
    while (TIMSK3)
    {
        if (PINA) constDrivi(-3.5, 600);
    }
    while(1);
    */
    startGame(1);
}


void loop() {
    bool localCoin;
    while (millis() < (gameTime - 10000) && coinCnt)
    {
        localCoin = getCoin();
        if (localCoin)
        {
            localMove();
        }
        else 
        {
            regionMove();
        }
        
        coinArray[currColor][currSpot] = false;
        coinCnt--;
        pickupMove();
        
        if (coinColor != GRAY)  
        {
            destColor = coinColor;
            deliverMove();
        }
        else 
        {
            centerinMove();
            destColor = currColor;
            constDrivi(0.2, 800);while(TIMSK3);
            leaveToken(); 
            constDrivi(-0.2, 800);while(TIMSK3);
        }
        
    }
    returnHome();
    while(1);
}

void startGame(char roundNum){

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
            coinCnt = 24;   //14 actual
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

    currColor = YELLOW;
    currSpot = 5;
    
    drivi(-42); while(TIMSK3);
    rotati(-45); while(TIMSK3);
}

void returnHome(){
    if (currSpot != 5)
    {
        centerinMove();
    }
    destColor = GRAY;
    rotati(colorVector[currColor][destColor]); while(TIMSK3);
    drivi(42);while(TIMSK3);
    
    bool beforeTheEdge = true;
    rotati(90);  while(TIMSK3);
    while (TIMSK3)
    {
        if(PINA) beforeTheEdge = false;
    }
    
    if (beforeTheEdge) 
    {
        constDrivi(12, 600); 
    }
    else
    {
        constDrivi(-12, 600);
    }
    
    while (!PINA);
    constDrivi(-4.5, 600); while(TIMSK3);
}

