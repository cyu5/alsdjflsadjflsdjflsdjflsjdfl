#ifndef MUSIC_H
#define MUSIC_H

#define buzzerPin1  0 
#define buzzerPin2  0
#define buzzerPin3  0


#define E_CLOCK 16
#define PRESCALER 64
#define startPlayer() ({\
        cycles[0] = MarioSongCycles[0][phasePtr][measurePtr][notePtr];\
        freq[0] = (unsigned long) E_CLOCK * MarioSongFreq[0][phasePtr][measurePtr][notePtr] / PRESCALER;\
        cycles[1] = MarioSongCycles[1][phasePtr][measurePtr][notePtr];\
        freq[1] = (unsigned long) E_CLOCK * MarioSongFreq[1][phasePtr][measurePtr][notePtr] / PRESCALER;\
        cycles[2] = MarioSongCycles[2][phasePtr][measurePtr][notePtr];\
        freq[2] = (unsigned long) E_CLOCK * MarioSongFreq[2][phasePtr][measurePtr][notePtr] / PRESCALER;\
        OCR5A = TCNT5 + (E_CLOCK*10/PRESCALER);        /* A is for master/left wheels */\
        OCR5B = TCNT5 + (E_CLOCK*10/PRESCALER);         /* B is for slave/right wheels */\
        TIMSK5 |= 0x0E;     /* enable OC interrupt A and B and C */\
        })
#define setupPlayer() ({\
        pinMode(buzzerPin1, OUTPUT);\
        pinMode(buzzerPin2, OUTPUT);\
        pinMode(buzzerPin3, OUTPUT);\
        TCCR5A = 0;       /* no OC action */ \
        TCNT5  = 0;       /* reset count */ \
        TCCR5B = 0x03;    /* normal mode, 64 prescaler */ \
        TIMSK5 = 0;       /* clear TCNT3 interrupts */ \
        })


//Interrupt motion globals

#define NUM_NOTES 15
#define NUM_MEASURES 12 
#define NUM_PHASES 3 
#define NUM_BUZZERS 3
volatile unsigned int MarioSongFreq[NUM_BUZZERS][NUM_PHASES][NUM_MEASURES][NUM_NOTES];
volatile unsigned int MarioSongCycles[NUM_BUZZERS][NUM_PHASES][NUM_MEASURES][NUM_NOTES];
volatile byte notePtr = 0;
volatile byte measurePtr = 0;
volatile byte phasePtr = 0;
volatile unsigned int cycles[NUM_BUZZERS];
volatile unsigned int freq[NUM_BUZZERS];

//interrupt vectors
ISR(TIMER5_COMPA_vect) 
{
        if (cycles[0])
        {
            cycles[0]--;
            OCR5A += freq[0];
        }
        else if (MarioSongCycles[0][phasePtr][measurePtr][++notePtr])
        {
            notePtr = 0;
            cycles[0] = MarioSongCycles[0][phasePtr][measurePtr][notePtr] - 1;
            freq[0] = MarioSongFreq[0][phasePtr][measurePtr][notePtr];
            OCR5A += freq[0];
        }
        else if (++phasePtr < NUM_PHASES)
        {
            MeasurePtr++;
            notePtr = 0;
        }
        
        
}

ISR(TIMER5_COMPB_vect)          // rightwheels slave
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

ISR(TIMER5_COMPC_vect)          // rightwheels slave
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








#endif
