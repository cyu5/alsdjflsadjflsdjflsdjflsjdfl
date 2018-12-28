#include "navi2.h" 


void setup() {
    delay(2500);                //wait for me to turn on the switch
    Serial.begin(9600);         //for debugging
    setupColor();                 //colorSensor
    setupTimer();               //interrupt motion
    setupLine();
    setupServo();
    startGame(1);
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
    
//      drivi(-42);  while (TIMSK3);  
//      rotati(45);  while (TIMSK3);  
//      constDrivi(CEN_OUT_CR + BOT_LEN, 300);
//      while (TIMSK3) lineFollow();

    drivi(-42);  while (TIMSK3);  
    rotati(-45);  while (TIMSK3);
    currColor = YELLOW; currSpot = 5;
}

void returnHome(){
    if (currSpot != 5)
    {
        centerinMove();
    }
    destColor=GRAY;
    rotati(colorVector[currColor][destColor]); while(TIMSK3);
    drivi(42);while(TIMSK3);
    rotati(90); while(TIMSK3);
    constDrivi(18, 600);
    while (TIMSK3)
    {
        if (PINA)
        { 
            constDrivi(-4, 600);
            return;
        }
    }
    constDrivi(-30, 600);
    while (TIMSK3)
    {
        if (PINA) 
        {
            constDrivi(-4, 600);
            return;
        }
        
    }
}

