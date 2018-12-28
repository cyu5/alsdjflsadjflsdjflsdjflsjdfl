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
#define motorBits B01010101
#define dirBits B10101010
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
        stepCnt = stepStack[STACK_PTR] - 1;\
        periodCnt = periodStack[STACK_PTR];\
        stackCnt--;\
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
volatile unsigned long stepStack[STACK_SIZE]; 
volatile unsigned int periodStack[STACK_SIZE];
volatile unsigned long stepmeter = 0;
volatile int stpercent = 100;
volatile int steerTable[256];

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
            else if (stackCnt)
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
        //masterCnt = ((long)periodCnt*(-1*stpercent)/100 < 65535) ? (periodCnt*(-1*stpercent)/100) : 65535;
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
            else if (stackCnt)
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
        //slaveCnt = ((long)periodCnt*stpercent/100 < 65535) ? (periodCnt*stpercent/100) : 65535;
        OCR3B += slaveCnt;              
}



//interrupt functions           
void pushStack(unsigned int steps, unsigned long period){
    period = constrain(period, MIN_PERIOD, MAX_PERIOD);
    periodStack[stackCnt] = E_CLOCK * period / PRESCALER;
    stepStack[stackCnt] = steps ? steps : 1;              
    stackCnt++;
}


void movi(byte dir, float dist, unsigned int period, boolean overwrite = true){
    PORTL = dir;
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




void accelMovi(byte dir, float dist, boolean overwrite = true, unsigned int n = 20, unsigned int startPeriod = 1000, unsigned char startSteps = 64){
    PORTL = dir;
    unsigned long steps = dist * STEPS_PER_INCH;   // accel + mid + decel 
    unsigned long midSteps;                        // top speed cruising distance
    unsigned long totalSteps;                      // accel + decel
    unsigned int accelStage = 1;                  // accel increment
    unsigned int decelStage = 1;                  // decel decrement
    unsigned int decelSteps;               
    float ratio = 2;                              // ratio = decelSteps/accelSteps

    //Serial.print("----------pushing accelMovi-----------"); //Serial.print("\n");
    //Serial.print("dist: "); //Serial.print(dist);//Serial.print("\t");
    //Serial.print("steps: "); //Serial.print(dist*STEPS_PER_INCH);//Serial.print("\t");
    //Serial.print("input stage: "); //Serial.print(n);//Serial.print("\t");
    
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
                //Serial.print("hardbreak-");
                //Serial.print("ratio: "); Serial.print(ratio);Serial.print("\t");
                //Serial.print("ratio*startSteps: "); Serial.print(ratio*startSteps);Serial.print("\t");
                //Serial.print("decelDist: "); //Serial.print(decelSteps/STEPS_PER_INCH);//Serial.print("\t");
                //Serial.print("decelStage: "); //Serial.print(decelStage);//Serial.print("\t");
                //Serial.print("\n");

                stackCnt = 0;stepCnt = 100;
                for (int i = 1; i <= decelStage; i++)
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
    


    //Serial.print("midDist: "); //Serial.print(midSteps/STEPS_PER_INCH);//Serial.print("\t");
    //Serial.print("totalDist: "); //Serial.print(totalSteps/STEPS_PER_INCH);//Serial.print("\t");
    //Serial.print("actual stage: "); //Serial.print(n);//Serial.print("\t");
    //Serial.print("accelStage: "); //Serial.print(accelStage);//Serial.print("\t");
    //Serial.print("decelStage: "); //Serial.print(decelStage);//Serial.print("\t");
    //Serial.print("\n");

    stackCnt = 0;stepCnt = 100;
    // push accel program in reverse motion order 
    for (unsigned int stage = decelStage; stage < n; stage++){
        pushStack(ratio*stage*startSteps, startPeriod/stage);
    }
    pushStack(midSteps, startPeriod/n);
    for (unsigned int stage = n-1; stage >= accelStage; stage--){
        pushStack(stage*startSteps, startPeriod/stage);
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

void showStack(unsigned char top = STACK_PTR){
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
    steps += stepCnt;
    for (unsigned char i = top; i<255; i--){
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

#define NUM_SENSORS 8
void setupLine(){
    DDRA = 0xFF;          // make sensor line an output
    PORTA = 0xFF;         // drive sensor line high

    delayMicroseconds(10);              // charge lines for 10 us
    
    DDRA = 0;             // make sensor line an input
    PORTA = 0;            // important: disable internal pull-up!

    steerTable[0] = 100;
    for(byte sensorValue=1; sensorValue!=0; sensorValue++)
    {
        byte i = 0;                          
        byte avg = 0;
        byte sum = 0;
        byte value = sensorValue;
        while(value)
        {
            if (value & 1) 
            {
                avg += i;
                sum++;
            }
            i++;
            value = value >> 1;
        }
        float ratio = exp(4 * ((float)avg/sum / (NUM_SENSORS-1)) - 2 );
        if (ratio >= 1) steerTable[sensorValue] = 100*ratio;
        else steerTable[sensorValue] = -100/ratio;  
    }   
}



#define readLineSensor()  ({stpercent = 100;})
#define lineFollow(sensor) ({stpercent = steerTable[sensor];})

void printlineSensor(){
    for (char i=0; i<NUM_SENSORS; i++)
    {
        Serial.print((bool)(PINA&(1<<i)));
    }
    
    Serial.print('\t');
    
    for (char i=0; i<NUM_SENSORS; i++)
    {
        Serial.print((bool)(PINC&(1<<i)));
    }
    
    Serial.print('\n');
    
}


#endif
