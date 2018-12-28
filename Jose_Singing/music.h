#ifndef MUSIC_H
#define MUSIC_H

#define buzzerPin1  0 
#define buzzerPin2  0
#define buzzerPin3  0


#define E_CLOCK 16
#define PRESCALER 64
#define startPlayer() ({\
        notePtr = 0;\
        cycles[0] = MarioSongCycles[0][notePtr];\
        freq[0] = (unsigned long) E_CLOCK * MarioSongFreq[0][notePtr] / PRESCALER;\
        cycles[1] = MarioSongCycles[1][notePtr];\
        freq[1] = (unsigned long) E_CLOCK * MarioSongFreq[1][notePtr] / PRESCALER;\
        cycles[2] = MarioSongCycles[2][notePtr];\
        freq[2] = (unsigned long) E_CLOCK * MarioSongFreq[2][notePtr] / PRESCALER;\
        OCR5A = TCNT5 + (E_CLOCK*10/PRESCALER);        /* A is for buzzer1 */\
        OCR5B = TCNT5 + (E_CLOCK*10/PRESCALER);         /* B is for buzzer2 */\
        OCR5C = TCNT5 + (E_CLOCK*10/PRESCALER);         /* B is for buzzer3 */\
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
#define NUM_BUZZERS 3
volatile byte notePtr;
volatile unsigned int MarioSongFreq[NUM_BUZZERS][NUM_NOTES];
volatile unsigned int MarioSongCycles[NUM_BUZZERS][NUM_NOTES];
volatile unsigned int cycles[NUM_BUZZERS];
volatile unsigned int freq[NUM_BUZZERS];
volatile unsigned long startofNote

//interrupt vectors
ISR(TIMER5_COMPA_vect) 
{
        digitalWrite(buzzerPin1, HIGH);
        if  (micros()-startofNote >= noteDuration)
        {
            startofNote = micros();
            freq[0] = MarioSongFreq[0][++notePtr];
        }
        OCR5A += freq[0];
        digitalWrite(buzzerPin1, LOW);    
}

ISR(TIMER5_COMPB_vect)          
{
        digitalWrite(buzzerPin2, HIGH);  
        if (!cycles[0])
        {
            notePtr = (notePtr+1) % NUM_NOTES;
            cycles[1] = MarioSongCycles[1][notePtr];
            freq[1] = (unsigned long) E_CLOCK * MarioSongFreq[1][notePtr] / PRESCALER;
        }
        cycles[1]--;
        OCR5B += freq[1]; 
        digitalWrite(buzzerPin2, LOW);              
}

ISR(TIMER5_COMPC_vect)          // rightwheels slave
{
        digitalWrite(buzzerPin3, HIGH);  
        if (!cycles[2])
        {
            notePtr = (notePtr+1) % NUM_NOTES;
            cycles[2] = MarioSongCycles[2][notePtr];
            freq[2] = (unsigned long) E_CLOCK * MarioSongFreq[2][notePtr] / PRESCALER;
        }
        cycles[2]--;
        OCR5C += freq[2]; 
        digitalWrite(buzzerPin3, LOW);             
}


#endif
