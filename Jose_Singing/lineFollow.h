#ifndef LINE_FOLLOW_H
#define LINE_FOLLOW_H


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

#define drivi(dist)           ({(dist > 0)  ?  accelMovi(faceFront, dist)  :  accelMovi(faceRear, -(dist));})
#define rotati(degree)        ({(degree > 0)  ?  accelMovi(faceClockWise, INCH_PER_DEGREE*degree)  :  accelMovi(faceCounterWise, -1*INCH_PER_DEGREE*degree);})
#define strafi(dist)          ({(dist > 0)  ?  accelMovi(faceRight, dist)  :  accelMovi(faceLeft, -(dist));})

#define constDrivi(dist, period)      ({(dist > 0)  ?  movi(faceFront, dist, period)  :  movi(faceRear, -(dist), period);})
#define constRotati(degree, period)   ({(degree > 0)  ?  movi(faceClockWise, INCH_PER_DEGREE*degree, period)  :  movi(faceCounterWise, -1*INCH_PER_DEGREE*degree, period);})
#define constStrafi(dist, period)     ({(dist > 0)  ?  movi(faceRight, dist, period)  :  movi(faceLeft, -(dist), period);})

#define pushConstDrivi(dist, period)      ({(dist > 0)  ?  movi(faceFront, dist, period, true)  :  movi(faceRear, -(dist), period,  true);})
#define pushConstRotati(degree, period)   ({(degree > 0)  ?  movi(faceClockWise, INCH_PER_DEGREE*degree, period, true)  :  movi(faceCounterWise, -1*INCH_PER_DEGREE*degree, period, true);})
#define pushConstStrafi(dist, period)     ({(dist > 0)  ?  movi(faceRight, dist, period, true)  :  movi(faceLeft, -(dist), period,  true);})


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
#define STACK_PTR ((stackFront+1) % STACK_SIZE)
#define STACK_SIZE 100
#define popStack()  ({\
        stepCnt = stepStack[STACK_PTR] - 1;\
        periodCnt = periodStack[STACK_PTR];\
        stackFront = STACK_PTR;\
        })
#define MAX_PERIOD_CNT 65535
#define MAX_PERIOD  (PRESCALER * (long)MAX_PERIOD_CNT / E_CLOCK)      // max count number for the timer
#define MIN_PERIOD  150
#define MIN_PERIOD_CNT (E_CLOCK * (long)MIN_PERIOD / PRESCALER)
#define odometer() ((float)stepmeter/STEPS_PER_INCH)


//Interrupt motion globals
volatile unsigned long stepCnt = 0;
volatile unsigned int periodCnt = (E_CLOCK*10/PRESCALER);
volatile unsigned int masterCnt = (E_CLOCK*10/PRESCALER);
volatile unsigned int slaveCnt = (E_CLOCK*10/PRESCALER);
volatile unsigned char stackBack = 0;
volatile unsigned int stackFront = STACK_SIZE-1;
volatile unsigned int stepStack[STACK_SIZE]; 
volatile unsigned int periodStack[STACK_SIZE];
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
            else if (STACK_PTR != stackBack)
            {
                popStack();
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
            else if (STACK_PTR != stackBack)
            {
                popStack();
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
void pushStack(unsigned int steps, unsigned long period){
    period = constrain(period, MIN_PERIOD, MAX_PERIOD);
    periodStack[stackBack] = E_CLOCK * period / PRESCALER;
    stepStack[stackBack] = steps ? steps : 1;              
    stackBack = (stackBack+1) % STACK_SIZE;
}

void changeDist(float dist){
    stackFront = (stackBack) ? stackBack-1 : STACK_SIZE-1;
    stepCnt = dist*STEPS_PER_INCH;
}

void changePeriod(unsigned long period){
    period = constrain(period, MIN_PERIOD, MAX_PERIOD);
    periodCnt = E_CLOCK * period / PRESCALER;
}

void movi(byte dir, float dist, unsigned int period, boolean overwrite = true){
    PORTL = dir;
    if (overwrite) stackFront = (stackBack) ? stackBack-1 : STACK_SIZE-1;
    pushStack(dist*STEPS_PER_INCH, period);   

    //kick off motion if it's not already running
    if (!TIMSK3){     
        startTimer();
    }
    // whether we want to overwrite the current motion program
    // or just push the program on the stack 
    else if (overwrite){
        popStack();
    }
    
}




void accelMovi1(byte dir, float dist, boolean overwrite = true, unsigned int n = 20, unsigned int startPeriod = 1000, unsigned char startSteps = 64){
    bool debug = false;
    
    PORTL = dir;
    unsigned long steps = dist * STEPS_PER_INCH;   // accel + mid + decel 
    unsigned long midSteps;                        // top speed cruising distance
    unsigned long totalSteps;                      // accel + decel
    unsigned int accelStage = 1;                  // accel increment
    unsigned int decelStage = 1;                  // decel decrement
    unsigned int decelSteps;               
    float ratio = 2;                              // ratio = decelSteps/accelSteps

    if (debug)
    {
        //Serial.print("----------pushing accelMovi-----------"); //Serial.print("\n");
        //Serial.print("dist: "); //Serial.print(dist);//Serial.print("\t");
        //Serial.print("steps: "); //Serial.print(dist*STEPS_PER_INCH);//Serial.print("\t");
        //Serial.print("input stage: "); //Serial.print(n);//Serial.print("\t");
    }
    
    if (TIMSK3){
        unsigned int periodCnt = (masterCnt<slaveCnt) ? masterCnt : slaveCnt;
        unsigned int periodNow = PRESCALER * periodCnt / E_CLOCK;
        decelStage = startPeriod / periodNow;
        unsigned long decelSteps = decelStage*(decelStage+1)*ratio*startSteps/2;
        n = (n>decelStage) ? n : decelStage;
        accelStage = decelStage;
        if (overwrite)
        {
            // if not enough distance for deceleration, make a hard brake
            if (decelSteps > steps + startSteps)
            {
                ratio = 2*decelSteps/(decelStage*(decelStage+1)*startSteps);
                if (debug)
                {
                //Serial.print("hardbreak-");
                //Serial.print("ratio: "); Serial.print(ratio);Serial.print("\t");
                //Serial.print("ratio*startSteps: "); Serial.print(ratio*startSteps);Serial.print("\t");
                //Serial.print("decelDist: "); //Serial.print(decelSteps/STEPS_PER_INCH);//Serial.print("\t");
                //Serial.print("decelStage: "); //Serial.print(decelStage);//Serial.print("\t");
                //Serial.print("\n");
                }
                stackFront = stackBack;stepCnt = 100;
                //stack pop
                //for (int i = 1; i <= decelStage; i++)
                //    pushStack(ratio*i*startSteps, startPeriod/i);
                
                //queue pop
                for (int i = decelStage; i >= 1; i--)
                    pushStack(ratio*i*startSteps, startPeriod/i);

                    
                popStack();
                return;
            }
            // otherwise adjust distance for halfway accelmov calculation
            // to account the shorter distance for acceleration 
            // since the robot already have some speed
            steps += decelSteps/ratio;
            decelStage = 1;
        }
        else{
            steps += decelSteps*(1+ratio)/ratio;
        }
    }

    totalSteps = n*(n-1)*(1+ratio)*startSteps/2;   
    // if not enough distance for n(acceleration stepCnt), recalculate a lower n 
    if (totalSteps > steps)
    {
        n = (sqrt(1+(4*2*steps/((1+ratio)*startSteps))) + 1) / 2;
        totalSteps = n*(n-1)*(1+ratio)*startSteps/2;  
    }
    midSteps = steps - totalSteps;
    

    if(debug){
    //Serial.print("midDist: "); //Serial.print(midSteps/STEPS_PER_INCH);//Serial.print("\t");
    //Serial.print("totalDist: "); //Serial.print(totalSteps/STEPS_PER_INCH);//Serial.print("\t");
    //Serial.print("actual stage: "); //Serial.print(n);//Serial.print("\t");
    //Serial.print("accelStage: "); //Serial.print(accelStage);//Serial.print("\t");
    //Serial.print("decelStage: "); //Serial.print(decelStage);//Serial.print("\t");
    //Serial.print("\n");
    }
    if (overwrite) stackFront = (stackBack) ? stackBack-1 : STACK_SIZE-1; stepCnt = 100;
    // push accel program in queue motion order 

    for (unsigned int stage = accelStage; stage <= n-1; stage++){
        pushStack(stage*startSteps, startPeriod/stage);
    }
    pushStack(midSteps, startPeriod/n);
    for (unsigned int stage = n-1; stage >= decelStage; stage--){
        pushStack(ratio*stage*startSteps, startPeriod/stage);
    }
    
    
    /*
    for (unsigned int stage = decelStage; stage < n; stage++){
        pushStack(ratio*stage*startSteps, startPeriod/stage);
    }
    pushStack(midSteps, startPeriod/n);
    for (unsigned int stage = n-1; stage >= accelStage; stage--){
        pushStack(stage*startSteps, startPeriod/stage);
    }
    */
    if (!TIMSK3){
        //popStack();
        startTimer();
    }
    else if (overwrite){
        popStack();
    }
    if (debug){
    //Serial.print("----------pushed accelMovi-----------"); //Serial.print("\n");
    //Serial.print("\n");
    }
}

void accelMovi(byte dir, float dist, unsigned char power = 2, unsigned int /*n=5,*/ n=10*4, unsigned long /*startPeriod = 300*25,*/ startPeriod=(long)4000*16, unsigned char startSteps=1){    
    bool debug = true;
    
    PORTL = dir;
    unsigned long steps = dist * STEPS_PER_INCH;   // accel + mid + decel 
    unsigned long midSteps;                        // top speed cruising distance
    unsigned long totalSteps;                      // accel + decel
    unsigned int accelStage = 1;                  // accel increment
    unsigned int decelStage = 1;                  // decel decrement
    unsigned int decelSteps;               
    float ratio = 2;                              // ratio = decelSteps/accelSteps

    if (debug){
    Serial.print("----------pushing accelMovi-----------"); Serial.print("\n");
    Serial.print("dist: "); Serial.print(dist);Serial.print("\t");
    Serial.print("steps: "); Serial.print(steps);Serial.print("\t");
    Serial.print("input stage: "); Serial.print(n);Serial.print("\t");
    }

    unsigned int n_actual = n;
    totalSteps = 0;
    for (unsigned int i = 1; i <= n; i++)
    {
        totalSteps += (1+ratio) * startSteps * (pow(n, power) - pow(n-i, power));
        
        // if not enough distance for acceleration
        // then cut the original curve short
        if(totalSteps > steps)
        {
            n_actual = i;
            totalSteps -= (1+ratio) * startSteps * (pow(n, power) - pow(n-i, power));
            break;
        }
    }
    
    midSteps = steps - totalSteps;
    if (debug){
    Serial.print("midDist: "); Serial.print(midSteps/STEPS_PER_INCH);Serial.print("\t");
    Serial.print("totalDist: "); Serial.print(totalSteps/STEPS_PER_INCH);Serial.print("\t");
    Serial.print("actual stage: "); Serial.print(n_actual);Serial.print("\t");
    Serial.print("accelStage: "); Serial.print(accelStage);Serial.print("\t");
    Serial.print("decelStage: "); Serial.print(decelStage);Serial.print("\t");
    Serial.print("\n");
    }
    stackFront = (stackBack) ? stackBack-1 : STACK_SIZE-1; stepCnt = 100;
    // push accel program in reverse motion order 
//    for (unsigned int i = decelStage;  i < n_actual;  i++)
//    {   
//        pushStack(startSteps*(pow(n, power) - pow(n-i, power)), startPeriod/(pow(n, power) - pow(n-i, power)));
//    }
//    pushStack(midSteps, startPeriod/(pow(n, power) - pow(n-n_actual, power)));
//    for (unsigned int i = n_actual-1; i >= accelStage; i--)
//    {
//        pushStack(ratio*startSteps*(pow(n, power) - pow(n-i, power)), startPeriod/(pow(n, power) - pow(n-i, power)));
//    }

    //push in queue order

    for (unsigned int i = accelStage; i <= n_actual-1; i++)
    {
        pushStack(startSteps*(pow(n, power) - pow(n-i, power)), startPeriod/(pow(n, power) - pow(n-i, power)));
    }
    pushStack(midSteps, startPeriod/(pow(n, power) - pow(n-n_actual, power)));
    for (unsigned int i = n_actual-1;  i >= decelStage;  i--)
    {   
        pushStack(ratio*startSteps*(pow(n, power) - pow(n-i, power)), startPeriod/(pow(n, power) - pow(n-i, power)));
    }
    
    

      
    if (!TIMSK3){
        //popStack();
        startTimer();
    }
    else{
        popStack();
    }
    if (debug){
    Serial.print("----------pushed accelMovi-----------"); Serial.print("\n");
    Serial.print("\n");
    }
}


void showStack(unsigned char top = STACK_PTR){
    unsigned long steps = stepCnt;
    Serial.print("----------showing stacks-----------");Serial.print("\n");
    Serial.print("dist: ");Serial.print(steps/STEPS_PER_INCH);Serial.print("\t");
    Serial.print("stepCnt: ");Serial.print(steps);Serial.print("\t");
    Serial.print("masterCnt: ");Serial.print((long)PRESCALER * masterCnt / E_CLOCK);Serial.print("\t");
    Serial.print("slaveCnt: ");Serial.print((long)PRESCALER * slaveCnt / E_CLOCK);Serial.print("\t");
    Serial.print("stpercent: ");Serial.print(stpercent);Serial.print("\t");
    Serial.print("stackFront: ");Serial.print(stackFront);Serial.print("\t");
    Serial.print("stackBack: ");Serial.print(stackBack);Serial.print("\t");
    Serial.print("stackPtr: ");Serial.print(STACK_PTR);Serial.print("\t");
    Serial.print("\n");
    for (unsigned char i = top; i != stackBack; i=(i+1)%STACK_SIZE)
    {
        Serial.print("dist[");Serial.print(i);Serial.print("]: ");Serial.print(stepStack[i]/STEPS_PER_INCH);Serial.print("\t");
        Serial.print("stepStack[");Serial.print(i);Serial.print("]: ");Serial.print(stepStack[i]);Serial.print("\t");
        Serial.print("periodStack[");Serial.print(i);Serial.print("]: ");Serial.print((long)PRESCALER * periodStack[i] / E_CLOCK);Serial.print("\t");
        Serial.print("\n");
        steps += stepStack[i];
    }
    Serial.print("dist: ");Serial.print((float)steps/STEPS_PER_INCH);Serial.print("\t");
    Serial.print("steps: ");Serial.print(steps);Serial.print("\n");
    Serial.print("----------end of stacks-----------");Serial.print("\n");
    Serial.print("\n");
}

//#include "Mario.h"

#define NUM_SENSORS 8
#define TIMEOUT 500
int steerTable[256];
byte sensorSum[256];
byte lineNum[256];
void setupLine(){
    steerTable[0] = 100;
    for(byte sensorValue=1; sensorValue!=0; sensorValue++)
    {
        byte i = 0;                          
        byte avg = 0;
        byte sum = 0;
        byte value = sensorValue;
        byte lineCnt=0;
        bool on_line = false;
        while(value)
        {
            if (value & 1) 
            {
                avg += i;
                sum++;
                if (!on_line) lineCnt++;
                on_line = !on_line;
            }
            else on_line = false;
            i++;
            value = value >> 1;
        }
        sensorSum[sensorValue] = sum;
        lineNum[sensorValue] = lineCnt;
        float ratio = exp(3.6 * ((float)avg/sum / (NUM_SENSORS-1)) - 1.8 );
        if (ratio >= 1) steerTable[sensorValue] = 100*ratio;
        else steerTable[sensorValue] = -100/ratio;  
    }   
}

byte lineReading = PINA;
byte sensorCnt = 0;
byte lineCnt = 0;

#define readLineSensor() ({lineReading = PINA;})
#define noFollow() ({stpercent = 100; sensorCnt = 0; lineCnt=0;})
//#define lineFollow()  ({readLineSensor(); if (lineReading) stpercent = steerTable[lineReading]; lineCnt = lineNum[lineReading];})

#define steerLeft 0 
#define steerMiddle 1
#define steerRight 2
#define curveLeft 3
#define curveRight 4
void lineFollow(byte steerMode = steerMiddle){
    readLineSensor();
    lineCnt = lineNum[lineReading];
    if (lineCnt && lineCnt < 2)
    {
        stpercent = steerTable[lineReading]; 
    }
    else if (lineCnt >= 2 && lineCnt < 4)
    {
        if (steerMode == steerLeft)          stpercent = -700;
        else if (steerMode == steerMiddle)   stpercent = 100;
        else if (steerMode == steerRight)    stpercent = 700;
    }
    else if (lineCnt == 4)
    {
        if (steerMode == curveLeft)          stpercent = -180;
        else if (steerMode == steerMiddle)   stpercent = 100;
        else if (steerMode == curveRight)    stpercent = 180;
    }
    
    
}


#endif
