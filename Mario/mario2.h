#ifndef MARIO_H
#define MARIO_H

#include "pitch.h"


#define MSR0_SPEAKER0_MLD    NOTE_E5,    NOTE_E5,    NOTE_E5,    NOTE_REST,  NOTE_C5,    NOTE_E5,    NOTE_G5, NOTE_REST,      NOTE_G4, NOTE_REST 
//#define MSR0_SPEAKER2_MLD    NOTE_FS4,   NOTE_FS4,   NOTE_FS4,   NOTE_REST,  NOTE_FS4,   NOTE_FS4,   NOTE_B4, NOTE_REST,      NOTE_REST, NOTE_REST
#define MSR0_SPEAKER1_MLD    NOTE_D3,    NOTE_D3,    NOTE_D3,    NOTE_REST,  NOTE_D3,    NOTE_D3,    staccato_letter(NOTE_G3), staccato_letter(NOTE_G2) 
#define MSR0_TMP             _16th_NOTE, _8th_NOTE,  _16th_NOTE, _16th_NOTE, _16th_NOTE, _8th_NOTE,  staccato(_8th_NOTE),     staccato(_8th_NOTE) 

#define MSR1_SPEAKER0_MLD    NOTE_C5,    NOTE_REST,  NOTE_G4,    NOTE_REST,  NOTE_E4,    NOTE_REST,  NOTE_A4,    NOTE_REST,  NOTE_B4,    NOTE_REST,  NOTE_AS4,   NOTE_A4
//#define MSR1_SPEAKER2_MLD    NOTE_E4,    NOTE_REST,  NOTE_C4,    NOTE_REST,  NOTE_G3,    NOTE_REST,  NOTE_C4,    NOTE_REST,  NOTE_D4,    NOTE_REST,  NOTE_CS4,   NOTE_C4
#define MSR1_SPEAKER1_MLD    NOTE_G3,    NOTE_REST,  NOTE_E3,    NOTE_REST,  NOTE_C3,    NOTE_REST,  NOTE_F3,    NOTE_REST,  NOTE_G3,    NOTE_REST,  NOTE_FS3,   NOTE_F3
#define MSR1_TMP             _8th_NOTE,  _16th_NOTE, _16th_NOTE, _8th_NOTE,  _8th_NOTE,  _16th_NOTE, _16th_NOTE, _16th_NOTE, _16th_NOTE, _16th_NOTE, _16th_NOTE, _8th_NOTE

#define MSR2_SPEAKER0_MLD    NOTE_G4,NOTE_E5,NOTE_G5, NOTE_A5,   NOTE_F5,    NOTE_G5,    NOTE_REST,  NOTE_E5,    NOTE_REST,  NOTE_C5,    NOTE_D5,    NOTE_B4,    NOTE_REST  
//#define MSR2_SPEAKER2_MLD    NOTE_C4,NOTE_G4,NOTE_B4, NOTE_C5,   NOTE_A4,    NOTE_B4,    NOTE_REST,  NOTE_A4,    NOTE_REST,  NOTE_E4,    NOTE_F4,    NOTE_D4,    NOTE_REST  
#define MSR2_SPEAKER1_MLD    NOTE_E3,NOTE_C4,NOTE_E4, NOTE_F4,   NOTE_D4,    NOTE_E4,    NOTE_REST,  NOTE_C4,    NOTE_REST,  NOTE_A3,    NOTE_B3,    NOTE_G3,    NOTE_REST  
#define MSR2_TMP             triplet(_8th_NOTE),      _8th_NOTE, _16th_NOTE, _16th_NOTE, _16th_NOTE, _16th_NOTE, _16th_NOTE, _16th_NOTE, _16th_NOTE, _16th_NOTE, _8th_NOTE

#define speakerPin0  10
#define speakerPin1  8
#define speakerPin2  10
byte speakerPin(byte pinNum){
    if (pinNum == 0) return speakerPin0;
    else if (pinNum == 1) return speakerPin1;
    else if (pinNum == 2) return speakerPin2;
}
#define NUM_SPEAKERS 3
#define DEBUG false

unsigned int MarioThemeMelody0 [] = {
  MSR0_SPEAKER0_MLD,  MSR1_SPEAKER0_MLD,  MSR2_SPEAKER0_MLD,  MSR1_SPEAKER0_MLD,  MSR2_SPEAKER0_MLD
  };
  
unsigned int MarioThemeMelody1 [] = {
  MSR0_SPEAKER1_MLD,  MSR1_SPEAKER1_MLD,  MSR2_SPEAKER1_MLD,  MSR1_SPEAKER1_MLD,  MSR2_SPEAKER1_MLD
  };
  
unsigned int MarioThemeMelody2 [] = {
  MSR0_SPEAKER2_MLD,  MSR1_SPEAKER2_MLD,  MSR2_SPEAKER2_MLD,  MSR1_SPEAKER2_MLD,  MSR2_SPEAKER2_MLD
  };

unsigned int MarioThemeMelody(byte pinNum, unsigned int note){
    if (pinNum == 0) return MarioThemeMelody0[note];
    else if (pinNum == 1) return MarioThemeMelody1[note];
    else if (pinNum == 2) return MarioThemeMelody2[note];
}
byte MarioThemeTempo [] = {
  MSR0_TMP,
  MSR1_TMP,
  MSR2_TMP,
  MSR1_TMP,
  MSR2_TMP
  };


void playMarioTheme(){
    setBarLine(100, 4, 4);
    for (byte i=0; i<NUM_SPEAKERS; i++)
    {
        pinMode(speakerPin(i), OUTPUT);
        digitalWrite(speakerPin(i), LOW);
    }
    
    unsigned int noteNum = sizeof(MarioThemeTempo)/sizeof(byte);
    for (unsigned int notePtr=0; notePtr<noteNum; notePtr++)
    {
        byte noteType = MarioThemeTempo[notePtr];
        unsigned int duration = millisPerNote/noteType;    //in milliseconds
        unsigned int noteFreq[NUM_SPEAKERS];
        unsigned int period[NUM_SPEAKERS]; 
        for (byte i=0; i<NUM_SPEAKERS; i++)
        {
            noteFreq[i] = MarioThemeMelody(i, notePtr);
            period[i] = 1000000/noteFreq[i];             //in microsecond
        }
        
        if (DEBUG)
        {
            Serial.print("note#");Serial.print(notePtr);Serial.print('\t');
            Serial.print("noteType:");Serial.print(noteType);Serial.print('\t');
            Serial.print("duration:");Serial.print(duration);Serial.print('\t');
            for (byte i=0; i<NUM_SPEAKERS; i++)
            {
                Serial.print("noteFreq#");Serial.print(i);Serial.print(':');Serial.print(noteFreq[i]);Serial.print('\t');
                Serial.print("period#");Serial.print(i);Serial.print(':');Serial.print(period[i]);Serial.print('\t');
            }
            Serial.print('\n');
        }
        
        unsigned long startedAt = millis();
        unsigned long startTime[NUM_SPEAKERS];    //in micros
        bool toggleMark[NUM_SPEAKERS];
        for (byte i=0; i<NUM_SPEAKERS; i++)
        {
            startTime[i] = startedAt*1000;
            toggleMark[i] = false;
        }
        
        while((millis()-startedAt) < duration)
        {
            for (byte i=0; i<NUM_SPEAKERS; i++)
            {
                if (noteFreq[i] != NOTE_REST)
                {
                    
//                    if ((micros()-startTime[i]) >= period[i])
//                    {
//                        digitalWrite(speakerPin(i), HIGH);
//                        startTime[i] += period[i];
//                        digitalWrite(speakerPin(i), LOW);
//                    }
                    
                    
                    if ((micros()-startTime[i]) >= period[i]/2)
                    {
                        startTime[i] += period[i]/2;
                        if (toggleMark[i])
                        {
                            digitalWrite(speakerPin(i), HIGH);
                            //toggleMark[i] = false;
                        }
                        else
                        {
                            digitalWrite(speakerPin(i), LOW);
                            //toggleMark[i] = true;
                        }
                        toggleMark[i] = !toggleMark[i];
                    }
                }
            }
        }
        
        for (byte i=0; i<NUM_SPEAKERS; i++)
        {
            digitalWrite(speakerPin(i), LOW);
        } 
        delay(40);
    }
}

/*
volatile unsigned int notePtr;
volatile unsigned int noteDuration;
volatile unsigned int notePeriod;
volatile unsigned int noteNum;
volatile unsigned long startofNote;
void setMarioTheme(){
    TCCR5A = 0;      
    TCNT5  = 0;       
    TCCR5B = 0x03;    
    TIMSK5 = 0;
    setBarLine(150, 4, 4);
    pinMode(buzzerPin1, OUTPUT);
    digitalWrite(buzzerPin1, LOW);
    notePtr = 0;
    noteNum = sizeof(MarioThemeMelody)/sizeof(unsigned int);
}

void startMarioTheme(){
    OCR5A = TCNT5 + (E_CLOCK*10/PRESCALER);        
    OCR5B = TCNT5 + (E_CLOCK*10/PRESCALER);         
    OCR5C = TCNT5 + (E_CLOCK*10/PRESCALER);         
    TIMSK5 |= 0x0E;
}


ISR(TIMER5_COMPA_vect) 
{
        while (notePtr++ < noteNum)
        {
            
        }
        digitalWrite(buzzerPin1, HIGH);
        if  (micros() - startofNote >= noteDuration)
        {
            startofNote = micros();
            freq[0] = MarioSongFreq[0][++notePtr];
        }
        OCR5A += freq[0];
        digitalWrite(buzzerPin1, LOW);    
}


void playMarioTheme2(){

    unsigned int noteNum = sizeof(MarioThemeMelody)/sizeof(unsigned int);
    for (unsigned int notePtr=0; notePtr<noteNum; notePtr++)
    {
        byte noteType = MarioThemeTempo[notePtr];
        unsigned int noteFreq = MarioThemeMelody[notePtr];
        unsigned int duration = millisPerNote/noteType;    //in milliseconds
        unsigned int period = 1000000/noteFreq;             //in microsecond
        unsigned long startedAt = millis();
        if (DEBUG)
        {
            Serial.print("note#");Serial.print(notePtr);Serial.print('\t');
            Serial.print("noteFreq:");Serial.print(noteFreq);Serial.print('\t');
            Serial.print("noteType:");Serial.print(noteType);Serial.print('\t');
            Serial.print("duration:");Serial.print(duration);Serial.print('\t');
            Serial.print("period:");Serial.print(period);Serial.print('\t');
            Serial.print('\n');
        }
        while((millis()-startedAt) < duration)
        {
            if (noteFreq != NOTE_REST)
            {
                unsigned long startTime = micros();
                digitalWrite(buzzerPin1, HIGH);
                while((micros()-startTime) < period/2);
                digitalWrite(buzzerPin1, LOW);
                while((micros()-startTime) < period);
            }
        }
        delay(40);
    }
}


*/





#endif
