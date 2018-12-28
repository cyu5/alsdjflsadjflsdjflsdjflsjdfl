#ifndef INTERRUPT_MOTION_H
#define INTERRUPT_MOTION_H


#define B1 B00100000          //left-front backward
#define B2 B00000000          //right-front backward
#define B3 B00000010          //left-back backward
#define B4 B00000000          //right-back backward
#define F1 B00000000          //left-front forward
#define F2 B10000000          //right-front forward
#define F3 B00000000          //left-back forward
#define F4 B00001000          //right-back forward
#define M1 B00010000          //left-front motor
#define M2 B01000000          //right-front motor
#define M3 B00000001          //left-back motor
#define M4 B00000100          //right-back motor
#define faceFront F1|F2|F3|F4
#define faceRear B1|B2|B3|B4
#define faceCounterWise B1|F2|B3|F4
#define faceClockWise F1|B2|F3|B4
#define faceLeft B1|F2|F3|B4 
#define faceRight F1|B2|B3|F4
#define allWheels M1|M2|M3|M4
#define leftWheels M1|M3
#define rightWheels M2|M4
#define frontWheels M1|M2
#define rearWheels M3|M4
#define rightDiagonal M2|M3
#define leftDiagonal M1|M4

#define STEPS_PER_INCH 215.6
#define STEPS_PER_DEGREE 23.5
#define INCH_PER_DEGREE (float)STEPS_PER_DEGREE/STEPS_PER_INCH
#define ANGLE_PER_RATIO 45.0

#define drivi(dist)           (dist > 0)  ?  accelMovi(faceFront, dist)  :  accelMovi(faceRear, -1*dist);
#define rotati(degree)        (degree > 0)  ?  accelMovi(faceClockWise, INCH_PER_DEGREE*degree)  :  accelMovi(faceCounterWise, -1*INCH_PER_DEGREE*degree);
#define strafi(dist)          (dist > 0)  ?  accelMovi(faceRight, dist)  :  accelMovi(faceLeft, -1*dist);

#define constDrivi(dist, period)      (dist > 0)  ?  movi(faceFront, dist, period)  :  movi(faceRear, -1*dist, period);
#define constRotati(degree, period)   (degree > 0)  ?  movi(faceClockWise, INCH_PER_DEGREE*degree, period)  :  movi(faceCounterWise, -1*INCH_PER_DEGREE*degree, period);
#define constStrafi(dist, period)     (dist > 0)  ?  movi(faceRight, dist, period)  :  movi(faceLeft, -1*dist, period);

#define pushConstDrivi(dist, period)      (dist > 0)  ?  movi(faceFront, dist, period, true)  :  movi(faceRear, -1*dist, period,  true);
#define pushConstRotati(degree, period)   (degree > 0)  ?  movi(faceClockWise, INCH_PER_DEGREE*degree, period, true)  :  movi(faceCounterWise, -1*INCH_PER_DEGREE*degree, period, true);
#define pushConstStrafi(dist, period)     (dist > 0)  ?  movi(faceRight, dist, period, true)  :  movi(faceLeft, -1*dist, period,  true);

#define E_CLOCK 16
#define PRESCALER 64
#define startTimer() ({\
        stepCnt = 0;    \
        OCR3A = TCNT3 + (E_CLOCK*10/PRESCALER);        /* A is for master/left wheels */\
        OCR3B = TCNT3 + (E_CLOCK*10/PRESCALER);         /* B is for slave/right wheels */\
        TIMSK3 |= 0x06;     /* enable OC interrupt A and B */\
        })
#define setupTimer() ({\
        DDRL = B11111111;\     
        TCCR3A = 0;       /* no OC action */ \
        TCNT3  = 0;       /* reset count */ \
        TCCR3B = 0x03;    /* normal mode, 64 prescaler */ \
        TIMSK3 = 0;       /* clear TCNT3 interrupts */ \
        })
#define MAX_PERIOD_CNT 65535
#define MAX_PERIOD  (PRESCALER * (long)MAX_PERIOD_CNT / E_CLOCK)      // max count number for the timer
#define MIN_PERIOD  50
#define MIN_PERIOD_CNT (E_CLOCK * (long)MIN_PERIOD / PRESCALER)
#define odometer() ((float)stepmeter/STEPS_PER_INCH)


//Interrupt motion globals
volatile unsigned long stepCnt = 0;
volatile unsigned int periodCnt = (E_CLOCK*10/PRESCALER);
volatile unsigned int masterCnt = (E_CLOCK*10/PRESCALER);
volatile unsigned int slaveCnt = (E_CLOCK*10/PRESCALER);
volatile unsigned char stackCnt = 0;
volatile unsigned long stepmeter = 0;
volatile int stpercent = 100;

//interrupt vectors
ISR(TIMER3_COMPA_vect)          // leftwheels master
{
        PORTL ^= leftWheels;
        if (stpercent >= 0)
        {
            if (stepCnt)
            {
                stepCnt--;
                stepmeter++;  
            }
            else if (stage<cruiseStage && midSteps)
            {
                stage++;
                stepCnt = stepStage[stage];
                periodCnt = periodStage[stage];
            }
            else if (stage==cruiseStage)
            {
                stepCnt = midSteps;
                midSteps = 0;
            }
            else 
            
            
            else if (totalSteps>decelSteps)
            {
                stage++;
                stepCnt = stepStage[stage];
                if (totalSteps - stepCnt < decelSteps)
                {
                    stepCnt = totalSteps - decelSteps;
                    totalSteps = decelSteps;
                }
                else 
                {
                    periodCnt = periodStage[stage];
                    totalSteps -= stepCnt;
                }
            }
            else
            {
                stepCnt = 0;
                TIMSK3 = 0;   // stop OC interrupt, stop all motion 
            }
            masterCnt = periodCnt;
        }
        else masterCnt = constrain((long)periodCnt*(-1*stpercent)/100, MIN_PERIOD_CNT, MAX_PERIOD_CNT);
        OCR3A += masterCnt;
        
}

ISR(TIMER3_COMPB_vect)          // rightwheels slave
{
        PORTL ^= rightWheels;
        if (stpercent < 0)
        {
            if (stepCnt)
            {
                stepCnt--;
                stepmeter++;  
            }
            else 
            {
                stepCnt = 0;
                TIMSK3 = 0;   // stop OC interrupt, stop all motion 
            }
            slaveCnt = periodCnt;
        }
        else  slaveCnt = constrain((long)periodCnt*stpercent/100, MIN_PERIOD_CNT, MAX_PERIOD_CNT);
        OCR3B += slaveCnt;              
}



//interrupt functions           
#define NUM_STAGES 50
volatile unsigned long totalSteps;
volatile unsigned long decelSteps;
volatile unsigned int periodStage[NUM_STAGES];
volatile unsigned int stepStage[NUM_STAGES];
volatile unsigned char stage;

void setupAccel(unsigned int startPeriod = 1000, unsigned int startSteps = 64){
    for (int n=1, n<=NUM_STAGES; n++)
    {
        stepStage[n] = startSteps*n;
        periodStage[n] = startPeriod/n;
    }
}

void accelMovi(byte dir, float dist, boolean overwrite = true, unsigned int n = 20, unsigned int startPeriod = 1000, unsigned char startSteps = 64){
    PORTL = dir;
    totalSteps = dist * STEPS_PER_INCH;
    decelSteps = n*(n+1)*startSteps;
}

