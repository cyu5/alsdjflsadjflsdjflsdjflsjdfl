#include "navi2.h" 


void setup() {
    delay(2500);                //wait for me to turn on the switch
    Serial.begin(9600);         //for debugging
    setupColor();                 //colorSensor
    setupTimer();               //interrupt motion
    setupLine();
    setupServo();
    startGame(3);
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
        
        currColor = destColor;  currSpot = destSpot;
        coinArray[currColor][currSpot] = false;
        coinCnt--;
        pickupMove();
        
        if (coinColor != GRAY)  
        {
            destColor = coinColor;
            deliverMove();

            destSpot = -1;
            currSpot = 0;
            constDrive(0.2, 800);
            leaveToken(); 
            constDrive(-0.2, 800);
            
            rotati(180);while(TIMSK3);
        }
        else 
        {
            destSpot = 5;
            centerinMove();

            constDrive(0.2, 800);
            leaveToken(); 
            constDrive(-0.2, 800);
        }

        currColor = destColor; currSpot = destSpot;
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
    
    drive(-42);
    rotate(45); 
    constDrivi(CEN_OUT_CR + BOT_LEN+2, 300);           
    //drivi(dist);
    while (TIMSK3) lineFollow();
}

void returnHome(){
    if (currSpot != 5)
    {
        centerinMove();
    }
    destColor = GRAY;
    rotati(colorVector[currColor][destColor]); while(TIMSK3);
    drivi(42);
    
    bool beforeTheEdge = true;
    rotati(90);  
    while (TIMSK3)
    {
        if(PINA) beforeTheEdge = false;
    }
    
    if (beforeTheEdge) 
    {
        constDrivi(6, 600); 
    }
    else
    {
        constDrivi(-6, 600);
    }
    
    while (!PINA);
    constDrivi(-2, 600); while (TIMSK3);
}

