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
#define STACK_PTR (stackCnt-1)
#define STACK_SIZE 100
#define popStack()  ({\
        stepCnt += stepStack[STACK_PTR];\
        masterCnt = masterStack[STACK_PTR];\
        slaveCnt = slaveStack[STACK_PTR];\
        PORTL = (PORTL&allWheels)|dirStack[STACK_PTR]; /*clear dir bits, preserve motion bits, then change dir*/\
        stackCnt--;\
        })
#define MAX_PERIOD  (PRESCALER * (long)65535 / E_CLOCK)      // max count number for the timer
#define MIN_PERIOD  50



//Interrupt motion globals
volatile unsigned long stepCnt = 0;
volatile unsigned int masterCnt = (E_CLOCK*10/PRESCALER);
volatile unsigned int slaveCnt = (E_CLOCK*10/PRESCALER);
volatile unsigned char stackCnt = 0;
volatile unsigned long stepStack[STACK_SIZE]; 
volatile unsigned int masterStack[STACK_SIZE];
volatile unsigned int slaveStack[STACK_SIZE];
volatile unsigned char dirStack[STACK_SIZE];
volatile bool stepCtrl;
volatile float stratio = 1;
volatile int stangle = 0;

//interrupt vectors
ISR(TIMER3_COMPA_vect)          // leftwheels master
{
        PORTL ^= leftWheels;
        if (stepCtrl){
            if (stepCnt--){}
            else if (stackCnt){
                popStack();
            }
            else {
                stepCnt = 0;
                TIMSK3 = 0;   // stop OC interrupt, stop all motion 
            }
        }
        OCR3A += masterCnt;
        
}

ISR(TIMER3_COMPB_vect)          // rightwheels slave
{
        PORTL ^= rightWheels;
        if (!stepCtrl){
            if (stepCnt--){}
            else if (stackCnt){
                popStack();
            }
            else {
                stepCnt = 0;
                TIMSK3 = 0;   // stop OC interrupt, stop all motion 
            }
        }
        OCR3B += slaveCnt;
              
}



//interrupt functions           
void pushStack(byte dir, unsigned int steps, unsigned long masterPeriod, unsigned long slavePeriod){
    masterPeriod = masterPeriod<MAX_PERIOD ? masterPeriod : MAX_PERIOD;
    masterPeriod = masterPeriod>MIN_PERIOD ? masterPeriod : MIN_PERIOD;
    masterStack[stackCnt] = E_CLOCK * masterPeriod / PRESCALER;
    slavePeriod = slavePeriod<MAX_PERIOD ? slavePeriod : MAX_PERIOD;
    slavePeriod = slavePeriod>MIN_PERIOD ? slavePeriod : MIN_PERIOD;
    slaveStack[stackCnt] = E_CLOCK * slavePeriod / PRESCALER;
    dirStack[stackCnt] = dir;
    stepStack[stackCnt] = steps ? steps : 1;              
    stackCnt++;
}


void movi(byte dir, float dist, unsigned int period, boolean overwrite = true){
    pushStack(dir, dist*STEPS_PER_INCH, period, period);   

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


void steeri (float ratio){
    long periodCnt = (stepCtrl) ? masterCnt:slaveCnt;
    long periodStack = (stepCtrl) ? masterStack[STACK_PTR] : slaveStack[STACK_PTR];
    if (ratio >= 1) 
    {
        masterCnt = periodCnt;
        slaveCnt = ((periodCnt * ratio) < MAX_PERIOD) ? (periodCnt * ratio): MAX_PERIOD;
        stepCtrl = true;
        if(stackCnt)
        {
            masterStack[STACK_PTR] = periodStack;
            slaveStack[STACK_PTR] = ((periodStack * ratio) < MAX_PERIOD) ? (periodStack * ratio) : MAX_PERIOD;
        }
    }
    else
    {        
        slaveCnt = periodCnt;
        masterCnt = ((periodCnt / ratio) < MAX_PERIOD) ? (periodCnt / ratio): MAX_PERIOD;
        stepCtrl = false;
        if (stackCnt)
        {
            slaveStack[STACK_PTR] = periodStack;
            masterStack[STACK_PTR] = ((periodStack / ratio) < MAX_PERIOD) ? (periodStack / ratio) : MAX_PERIOD;
        }
        
    } 
    
}

void accelMovi(byte dir, float dist, boolean overwrite = true, unsigned int n = 20, unsigned int startPeriod = 1000, unsigned char startSteps = 64){
    unsigned long steps = dist * STEPS_PER_INCH;
    unsigned int accelStage = 1;          // start accel from this stage
    unsigned int decelStage = 1;          // start decel from this stage
    unsigned int decelSteps;               // distance require for normal stopping from current speed
    float ratio = 2;                      // ratio = decelSteps/accelSteps
    float midSteps;                        // top speed cruising distance
    float totalSteps;                      // distance required for accel and decel from rest to top speed

    //Serial.print("----------pushing accelMovi-----------"); //Serial.print("\n");
    //Serial.print("dist: "); //Serial.print(dist);//Serial.print("\t");
    //Serial.print("steps: "); //Serial.print(dist*STEPS_PER_INCH);//Serial.print("\t");
    //Serial.print("input stage: "); //Serial.print(n);//Serial.print("\t");
    
    if (TIMSK3 && dir==dirStack[stackCnt]){
        unsigned int periodCnt = (masterCnt<slaveCnt) ? masterCnt : slaveCnt;
        unsigned int periodNow = PRESCALER * periodCnt / E_CLOCK;
        decelStage = (startPeriod / periodNow);
        float decelSteps = decelStage*(decelStage+1)*ratio*startSteps/2;
        n = (n>decelStage) ? n : decelStage;
        accelStage = decelStage;
        if (overwrite){
            while (stackCnt && dir==dirStack[STACK_PTR])
                stackCnt--;
            
            // if not enough distance for deceleration, make a hard brake
            if (decelSteps > steps)
            {
                ratio = 2*decelSteps/(decelStage*(decelStage+1));
                for (int i = 1; i <= decelStage; i++)
                    pushStack(dir, ratio*i*startSteps, startPeriod/i, startPeriod/i);
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
    else {
        byte dirNow = dirStack[stackCnt];
        while(stackCnt && dirNow==dirStack[STACK_PTR])
            stackCnt--;
    }

    totalSteps = n*(n-1)*(1+ratio)*startSteps/2;   
    // if not enough distance for n(acceleration stepCnt), recalculate a lower n 
    if (totalSteps > steps){
        n = (sqrt(1+(4*2*steps/((1+ratio)*startSteps))) + 1) / 2;
        totalSteps = n*(n-1)*(1+ratio)*startSteps/2;  
    }
    midSteps = steps - totalSteps;
    


    //Serial.print("midDist: "); //Serial.print(midSteps/STEPS_PER_INCH);//Serial.print("\t");
    //Serial.print("totalDist: "); //Serial.print(totalSteps/STEPS_PER_INCH);//Serial.print("\t");
    //Serial.print("actual stage: "); //Serial.print(n);//Serial.print("\t");
    //Serial.print("accelStage: "); //Serial.print(accelStage);//Serial.print("\t");
    //Serial.print("decelStage: "); //Serial.print(decelStage);//Serial.print("\t");
    //Serial.print("\n");

    
    // push accel program in reverse motion order 
    for (unsigned int stage = decelStage; stage < n; stage++){
        pushStack(dir, ratio*stage*startSteps, startPeriod/stage, startPeriod/stage);
    }
    pushStack(dir, midSteps, startPeriod/n, startPeriod/n);
    for (unsigned int stage = n-1; stage >= accelStage; stage--){
        pushStack(dir, stage*startSteps, startPeriod/stage, startPeriod/stage);
    }

    if (!TIMSK3){
        //popStack();
        startTimer();
    }
    else if (overwrite){
        popStack();
    }
    //Serial.print("----------pushed accelMovi-----------"); //Serial.print("\n");
    //Serial.print("\n");
}

void showStack(unsigned char top = stackCnt){
    unsigned long steps = 0;
    float ratio = (float)slaveCnt/masterCnt;
    Serial.print("----------showing stacks-----------");Serial.print("\n");
    Serial.print("dist: ");Serial.print(stepCnt/STEPS_PER_INCH);Serial.print("\t");
    Serial.print("stepCnt: ");Serial.print(stepCnt);Serial.print("\t");
    Serial.print("masterCnt: ");Serial.print((long)PRESCALER * masterCnt / E_CLOCK);Serial.print("\t");
    Serial.print("slaveCnt: ");Serial.print((long)PRESCALER * slaveCnt / E_CLOCK);Serial.print("\t");
    Serial.print("ratioCnt: ");Serial.print(ratio);Serial.print("\t");
    Serial.print("stackCnt: ");Serial.print(stackCnt);Serial.print("\t");
    Serial.print("\n");
    //steps += stepCnt;
    for (unsigned char i = top; i<255; i--){
        ratio = (float)slaveStack[i]/masterStack[i];
        /*
        if (ratio>=1){
            Serial.print("dist[");Serial.print(i);Serial.print("]: ");Serial.print(stepStack[i]/STEPS_PER_INCH);Serial.print("\t");
            Serial.print("stepStack[");Serial.print(i);Serial.print("]: ");Serial.print(stepStack[i]);Serial.print("\t");
            steps += stepStack[i];
        }
        else{
            stepStack[i] /= ratio;
            Serial.print("dist[");Serial.print(i);Serial.print("]: ");Serial.print(stepStack[i]/STEPS_PER_INCH);Serial.print("\t");
            Serial.print("stepStack[");Serial.print(i);Serial.print("]: ");Serial.print(stepStack[i]);Serial.print("\t");
            steps += stepStack[i];
            stepStack[i] *= ratio;
        }
        */
        Serial.print("dist[");Serial.print(i);Serial.print("]: ");Serial.print(stepStack[i]/STEPS_PER_INCH);Serial.print("\t");
        Serial.print("stepStack[");Serial.print(i);Serial.print("]: ");Serial.print(stepStack[i]);Serial.print("\t");
        Serial.print("masterStack[");Serial.print(i);Serial.print("]: ");Serial.print((long)PRESCALER * masterStack[i] / E_CLOCK);Serial.print("\t");
        Serial.print("slaveStack[");Serial.print(i);Serial.print("]: ");Serial.print((long)PRESCALER * slaveStack[i] / E_CLOCK);Serial.print("\t");
        Serial.print("ratioStack[");Serial.print(i);Serial.print("]: ");Serial.print(ratio);Serial.print("\t");
        Serial.print("\n");
        steps += stepStack[i];
    }
    Serial.print("dist: ");Serial.print((float)steps/STEPS_PER_INCH);Serial.print("\t");
    Serial.print("steps: ");Serial.print(steps);Serial.print("\n");
    Serial.print("----------end of stacks-----------");Serial.print("\n");
    Serial.print("\n");
}

//recycling bin
/*

void steeri3 (float angle){
    unsigned int periodCnt;
    float angleNow;
    if (masterCnt<slaveCnt){
        periodCnt = masterCnt;
        angleNow = (slaveCnt/masterCnt)-1;
    }
    else {
        periodCnt = slaveCnt;
        angleNow = (masterCnt/slaveCnt)-1;
        stepCnt *= angleNow+1;
        angleNow *= -1;
    }

    unsigned int periodStack;
    if (stackCnt){
        if (masterStack[STACK_PTR]<slaveStack[STACK_PTR]){
            periodStack = masterStack[STACK_PTR];
        }
        else {
            periodStack = slaveStack[STACK_PTR];
            float angleStack = (masterStack[STACK_PTR]/slaveStack[STACK_PTR])-1;
            stepStack[STACK_PTR] *= angleStack+1;
        }
    }
    
    if (angle >= 0) 
    {
        masterCnt = periodCnt;
        slaveCnt = masterCnt * (angle+1);
        if(stackCnt)
        {
            masterStack[STACK_PTR] = periodStack;
            slaveStack[STACK_PTR] = periodStack * (angle+1);
        }
    }
    else
    {
        angle *= -1;
        slaveCnt = periodCnt;
        masterCnt = slaveCnt * (angle+1); 
        stepCnt /= (angle+1);  
        if (stackCnt)
        {
            slaveStack[STACK_PTR] = periodStack;
            masterStack[STACK_PTR] = periodStack * (angle+1); 
            stepStack[STACK_PTR] /= (angle+1);
        }    
    } 
}


void steeri2 (float dist, char ratio, unsigned char startSteps = 64){
    unsigned long steps = dist * STEPS_PER_INCH;
    unsigned int periodCnt;
    char ratioNow;
    if (masterCnt<slaveCnt){
        periodCnt = masterCnt;
        ratioNow = slaveCnt/masterCnt;
    }
    else {
        periodCnt = slaveCnt;
        ratioNow = masterCnt/slaveCnt;
        stepCnt *= ratioNow;
        ratioNow *= -1;
    }

    unsigned int periodStack;
    if (stackCnt){
        if (masterStack[STACK_PTR]<slaveStack[STACK_PTR]){
            periodStack = masterStack[STACK_PTR];
        }
        else {
            periodStack = slaveStack[STACK_PTR];
            char ratioStack = masterStack[STACK_PTR]/slaveStack[STACK_PTR];
            stepStack[STACK_PTR] *= ratioStack;
        }
    }
    

    char ratioChange = ratio - ratioNow;
    char ratioSign = (ratioChange>0) ? 1 : -1;
    unsigned char ratioDiff = ratioChange * ratioSign;
    unsigned long totalSteps = ratioDiff*(ratioDiff+1)*startSteps/2;
    if (totalSteps > steps){
        startSteps = 2*totalSteps/(ratioDiff*(ratioDiff+1));
    }
    unsigned long afterSteps = steps - totalSteps;
    unsigned char overwrite = 0;
    for (unsigned long tempSteps = stepCnt; (tempSteps<steps) && (stackCnt-overwrite>0); overwrite++){
        tempSteps += stepStack[stackCnt-overwrite];
    }

    unsigned int periodStack[overwrite];
    if (overwrite){
        if (masterStack[stackCnt-overwrite]<slaveStack[stackCnt-overwrite]){
            periodStack[overwrite-1] = masterStack[stackCnt-overwrite];
        }
        else {
            periodStack[overwrite-1] = slaveStack[stackCnt-overwrite];
            char ratioStack = masterStack[stackCnt-overwrite]/slaveStack[stackCnt-overwrite];
            stepStack[STACK_PTR] *= ratioStack;
        }
        overwrite--;
    }



    for (unsigned char stage = ratioDiff; stage && overwrite; stage--){
        
            pushStack(dirStack[stackCnt], stage*startSteps, periodCnt, stage);
        if (tempSteps){  
            overwrite--;
        }
    }


    if (ratio >= 0) 
    {
        masterCnt = periodCnt;
        slaveCnt = masterCnt * ratio;
        if(stackCnt)
        {
            masterStack[STACK_PTR] = periodStack;
            slaveStack[STACK_PTR] = periodStack * ratio;
        }
    }
    else
    {
        ratio *= -1;
        slaveCnt = periodCnt;
        masterCnt = slaveCnt * ratio; 
        stepCnt /= ratio;  
        if (stackCnt)
        {
            slaveStack[STACK_PTR] = periodStack;
            masterStack[STACK_PTR] = periodStack * ratio; 
            stepStack[STACK_PTR] /= ratio;
        }    
    } 
}









 */


#endif
