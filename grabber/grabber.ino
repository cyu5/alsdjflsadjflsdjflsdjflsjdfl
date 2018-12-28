#include "colorSensor.h"
#include "servo.h"
#include "motion.h"

void setup() {
    delay(2500);                //wait for me to turn on the switch
    Serial.begin(9600);         //for debugging
    setupColor();                 //colorSensor
    setupServo();
//    while (1)
//    {  
//        pickUp();
//        for (byte i=1; i; i++)
//        {
//        readColor();
//        showColor();
//        }
//        leaveToken();
//    }

    while (1)
    {
        pickupMove();
        delay(1000);
    }

    
}

void loop() {
    Serial.print("sample in 3 seconds");
    delay(3000);
    Serial.print("start sampling");
    sampleColor();
    delay(20000);
}

void pickupMove1(){
    byte colorCounts[NUM_COLORS] = {0};         //count for each color in all samples from 3 measurements
    byte mostly = GRAY;                         //the color that is the mode of all 3 measurements
    byte mostCounts = 0;                        //the count for mostly(mode of all colors)
    byte colorSamples[60];
    byte sampleNum = 0;
    pickUp();
    
    //scan and collect multiple color samples
    for (char wabble = -25; wabble <= 25; wabble++)
    {
        sliderTurn(SLIDER_IN + wabble);
        readColor();
        colorSamples[sampleNum++] = coinColor;
        showColor();
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
        Serial.print(colorCounts[i]); showColor(i);
        if (mostCounts < colorCounts[i])
        {
            mostCounts = colorCounts[i];
            mostly = i;
        }
    }

    if (mostly > 5) coinColor = mostly;
    Serial.print("final color:"); showColor(coinColor);
    leaveToken();
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
            showColor();
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
              constDrive(-0.5 ,1000);
            }
            else if (takeSamples==1) 
            {
              constDrive(1 ,1000);
            }
            else if (takeSamples==0)
            {
              constDrive(-0.5 ,1000);
            }
            pickUp();
        }
        else break;
    }

    //final reading result
    Serial.print("final color:"); showColor();
    leaveToken();
}


void testTime(){
    unsigned long timeout = micros();
    readColor();
    timeout = micros() - timeout;
    Serial.print("readtime: ");Serial.println(timeout);
}

