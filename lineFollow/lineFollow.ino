#include "lineFollow.h"


#define JUNCTION (((PINA&B00011100) == B00011100) || ((PINA&B00111000) == B00111000))

void setup() {
    //delay(2000);                //wait for me to turn on the switch
    Serial.begin(9600);         //for debugging
    setupTimer();
    setupLine();

    constDrivi(60, 300);
    while(1)
    {
        #define JUNCTION (((PINA&B00010100) == B00010100) || ((PINA&B00101000) == B00101000) || ((PINA&B11000011) == B11000011))
        lineFollow(PINA);
        printlineSensor();
        if (JUNCTION)
        {
            TIMSK3 = 0;
            delay(3000);
            constDrivi(60, 300);
        }
        else if (TIMSK3 == 0)
        {
            delay(3000);
            constDrivi(60, 300);
        }
        
    }
    while(1);
    while (1)
    {
        constDrivi(-12, 800);
        while (TIMSK3) 
        {
            stpercent = steerTable[PINC];
        }
        constDrivi(12, 800);
        while (TIMSK3) 
        {
            stpercent = steerTable[PINA];
        }
    }
    
}

void loop() {
  // put your main code here, to run repeatedly:

}



