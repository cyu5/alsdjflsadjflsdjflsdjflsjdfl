#ifndef MARIO_H
#define MARIO_H

#include "pitch.h"

#define DEBUG false

#define MSR0_MLD0    NOTE_E5,    NOTE_E5,    NOTE_E5,    NOTE_REST,  NOTE_C5,    NOTE_E5,    NOTE_G5, NOTE_REST,      NOTE_G4, NOTE_REST 
#define MSR0_TMP0    _16th_NOTE, _8th_NOTE,  _16th_NOTE, _16th_NOTE, _16th_NOTE, _8th_NOTE,  staccato(_8th_NOTE),     staccato(_8th_NOTE) 
#define MSR0_MLD1    NOTE_D3,    NOTE_D3,    NOTE_D3,    NOTE_REST,  NOTE_D3,    NOTE_D3,    staccato_letter(NOTE_G3), staccato_letter(NOTE_G2) 
#define MSR0_TMP1    _16th_NOTE, _8th_NOTE,  _16th_NOTE, _16th_NOTE, _16th_NOTE, _8th_NOTE,  staccato(_8th_NOTE),     staccato(_8th_NOTE) 


#define MSR1_MLD0    NOTE_C5,    NOTE_REST,  NOTE_G4,    NOTE_REST,  NOTE_E4,    NOTE_REST,  NOTE_A4,    NOTE_REST,  NOTE_B4,    NOTE_REST,  NOTE_AS4,   NOTE_A4
#define MSR1_TMP0    _8th_NOTE,  _16th_NOTE, _16th_NOTE, _8th_NOTE,  _8th_NOTE,  _16th_NOTE, _16th_NOTE, _16th_NOTE, _16th_NOTE, _16th_NOTE, _16th_NOTE, _8th_NOTE
#define MSR1_MLD1    NOTE_G3,    NOTE_REST,  NOTE_E3,    NOTE_REST,  NOTE_C3,    NOTE_REST,  NOTE_F3,    NOTE_REST,  NOTE_G3,    NOTE_REST,  NOTE_FS3,   NOTE_F3
#define MSR1_TMP1    _8th_NOTE,  _16th_NOTE, _16th_NOTE, _8th_NOTE,  _8th_NOTE,  _16th_NOTE, _16th_NOTE, _16th_NOTE, _16th_NOTE, _16th_NOTE, _16th_NOTE, _8th_NOTE


#define MSR2_MLD0    NOTE_G4,NOTE_E5,NOTE_G5, NOTE_A5,   NOTE_F5,    NOTE_G5,    NOTE_REST,  NOTE_E5,    NOTE_REST,  NOTE_C5,    NOTE_D5,    NOTE_B4,    NOTE_REST  
#define MSR2_TMP0    triplet(_8th_NOTE),      _8th_NOTE, _16th_NOTE, _16th_NOTE, _16th_NOTE, _16th_NOTE, _16th_NOTE, _16th_NOTE, _16th_NOTE, _16th_NOTE, _8th_NOTE
#define MSR2_MLD1    NOTE_E3,NOTE_C4,NOTE_E4, NOTE_F4,   NOTE_D4,    NOTE_E4,    NOTE_REST,  NOTE_C4,    NOTE_REST,  NOTE_A3,    NOTE_B3,    NOTE_G3,    NOTE_REST  
#define MSR2_TMP1    triplet(_8th_NOTE),      _8th_NOTE, _16th_NOTE, _16th_NOTE, _16th_NOTE, _16th_NOTE, _16th_NOTE, _16th_NOTE, _16th_NOTE, _16th_NOTE, _8th_NOTE


#define speakerPin0  10
#define speakerPin1  8
#define NUM_SPEAKERS 3
byte speakerPin[NUM_SPEAKERS] = {speakerPin0, speakerPin1};



unsigned int MarioThemeMelody0 [] = {
  MSR0_MLD0,  MSR1_MLD0,  MSR2_MLD0,  MSR1_MLD0,  MSR2_MLD0
  };
  
unsigned int MarioThemeMelody1 [] = {
  MSR0_MLD1,  MSR1_MLD1,  MSR2_MLD1,  MSR1_MLD1,  MSR2_MLD1
  };

unsigned int MarioThemeMelody(byte pinNum, unsigned int note){
    if (pinNum == 0) return MarioThemeMelody0[note];
    else if (pinNum == 1) return MarioThemeMelody1[note];
}

byte MarioThemeTempo0 [] = {
  MSR0_TMP0,  MSR1_TMP0,  MSR2_TMP0,  MSR1_TMP0,  MSR2_TMP0
  };
byte MarioThemeTempo1 [] = {
  MSR0_TMP1,  MSR1_TMP1,  MSR2_TMP1,  MSR1_TMP1,  MSR2_TMP1
  };
byte MarioThemeTempo(byte pinNum, unsigned int note){
    if (pinNum == 0) return MarioThemeTempo0[note];
    else if (pinNum == 1) return MarioThemeTempo1[note];
}


void playMarioTheme(){
    setBarLine(100, 4, 4);
    for (byte i=0; i<NUM_SPEAKERS; i++)
    {
        pinMode(speakerPin[i], OUTPUT);
        digitalWrite(speakerPin[i], LOW);
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
//                        digitalWrite(speakerPin[i], HIGH);
//                        startTime[i] += period[i];
//                        digitalWrite(speakerPin[i], LOW);
//                    }
                    
                    
                    if ((micros()-startTime[i]) >= period[i]/2)
                    {
                        startTime[i] += period[i]/2;
                        if (toggleMark[i])
                        {
                            digitalWrite(speakerPin[i], HIGH);
                            //toggleMark[i] = false;
                        }
                        else
                        {
                            digitalWrite(speakerPin[i], LOW);
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


#endif
