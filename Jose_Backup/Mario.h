#ifndef MARIO_H
#define MARIO_H


#define demiSemiQuaver 150
#define semiQuaver 300
#define quaver 600
#define triplet 200

#define C4 956
#define E4 758
#define F4 716
#define G4 638
#define lowA 568
#define B4 506
#define C5 478
#define D5 426
#define Eb5 402
#define E5 379
#define F5 358
#define Gb5 338
#define G5 319
#define Ab5 301
#define highA 284
#define Bb5 268
#define B5 253
#define C6 239

#define unison 400
#define majorThird 160
#define majorFifth 133
#define minorSeventh 111
#define octave 100




unsigned long cycles;
byte measureNum = 1;
byte noteNum = 1;
bool measureComplete = false;
#define DEBUG false


void note(unsigned long note, unsigned int pitch){
      unsigned int period = pitch/2;
      unsigned long noteCycle = 1000*note/period;
      if (DEBUG)
      {
          Serial.print("measure#");Serial.print(measureNum); Serial.print("\t");
          Serial.print("note#"); Serial.print(noteNum); Serial.print("\t");
          Serial.print("noteCycle: "); Serial.print(noteCycle);Serial.print("\t");
          Serial.print("cyclesLeft: "); Serial.print(cycles);Serial.print("\n");
      }
      noteCycle = (noteCycle > cycles) ? cycles : noteCycle;
      pushStack(noteCycle, period);
      cycles -= noteCycle;
}

void rest(int note){
    //pushStack(1, note*1000);
}

void measureOne(){
    measureComplete = false;
    while (cycles && !measureComplete)
    {
        switch (noteNum)
        {
            case 1:
                note(demiSemiQuaver, E5);
                break;   
            case 2:
                note(semiQuaver, E5);
                break;
            case 3:
                note(demiSemiQuaver, E5);
                break;
            case 4:
                rest(demiSemiQuaver);
                break;
            case 5:
                note(demiSemiQuaver, C5);
                break;
            case 6:
                note(semiQuaver, E5);
                break;
            case 7:
                note(quaver, G5);
                break;
            case 8:
                note(quaver, G4); 
                measureComplete = true;
                break;
        }
        noteNum++;    
    }
}

void measureTwo(){
    measureComplete = false;
    while (cycles && !measureComplete)
    {
        switch (noteNum)
        {
            case 1:
                note(semiQuaver, C5);
                break;   
            case 2:
                rest(demiSemiQuaver);
                break;
            case 3:
                note(demiSemiQuaver, G5);
                break;
            case 4:
                rest(semiQuaver);
                break;
            case 5:
                note(semiQuaver, E4);
                break;
            case 6:
                rest(demiSemiQuaver);
                break;
            case 7:           
                note(demiSemiQuaver, highA);
                break;
            case 8:
                rest(demiSemiQuaver); 
                break;
            case 9:
                note(demiSemiQuaver, B5);
                break;
            case 10:
                rest(demiSemiQuaver);
                break;
            case 11:
                note(demiSemiQuaver, Bb5);
                break;
            case 12:
                note(semiQuaver, highA);
                measureComplete = true;
                break;
        }
        noteNum++;    
    }
}


void measureThree(){

    measureComplete = false;
    while (cycles && !measureComplete)
    {
        switch (noteNum)
        {
            case 1:
                note(triplet, G4);
                break;   
            case 2:
                note(triplet, E5);
                break;
            case 3:
                note(triplet, G5);
                break;
            case 4:
                note(semiQuaver, highA);
                break;
            case 5:
                note(demiSemiQuaver, F5);
                break;
            case 6:
                note(demiSemiQuaver, G5);
                break;
            case 7:           
                rest(demiSemiQuaver);
                break;
            case 8:
                note(demiSemiQuaver, E5); 
                break;
            case 9:
                rest(demiSemiQuaver);
                break;
            case 10:
                note(demiSemiQuaver, C5);
                break;
            case 11:
                note(demiSemiQuaver, D5);
                break;
            case 12:
                note(demiSemiQuaver, B5);
                break;
            case 13:
                rest(semiQuaver);
                measureComplete = true;
                break;
        }
        noteNum++;    
    }
}

void measureFour(){
    measureComplete = false;
    while (cycles && !measureComplete)
    {
        switch (noteNum)
        {
            case 1:
                note(semiQuaver, C4);
                break;   
            case 2:
                note(demiSemiQuaver, G5);
                break;
            case 3:
                note(demiSemiQuaver, Gb5);
                break;
            case 4:
                note(demiSemiQuaver, F5);
                break;
            case 5:
                note(semiQuaver, Eb5);
                break;
            case 6:
                note(demiSemiQuaver, E5);
                break;
            case 7:           
                rest(demiSemiQuaver);
                break;
            case 8:
                note(demiSemiQuaver, Ab5);
                break;
            case 9:
                note(demiSemiQuaver, highA);
                break;
            case 10:
                note(demiSemiQuaver, C5);
                break;
            case 11:
                rest(demiSemiQuaver);
                break;
            case 12:
                note(demiSemiQuaver, highA);
                break;
            case 13:
                note(demiSemiQuaver, C5);
                break;
            case 14:
                note(demiSemiQuaver, D5);
                measureComplete = true;
                break;
        }
        noteNum++;    
    }

}

void measureFive(){
    measureComplete = false;
    while (cycles && !measureComplete)
    {
        switch (noteNum)
        {
            case 1:
                note(semiQuaver, C4);
                break;   
            case 2:
                note(demiSemiQuaver, G5);
                break;
            case 3:
                note(demiSemiQuaver, Gb5);
                break;
            case 4:
                note(demiSemiQuaver, F5);
                break;
            case 5:
                note(semiQuaver, Eb5);
                break;
            case 6:
                note(demiSemiQuaver, E5);
                break;
            case 7:           
                rest(demiSemiQuaver);
                break;
            case 8:
                note(demiSemiQuaver, C6); 
                break;
            case 9:
                rest(demiSemiQuaver);
                break;
            case 10:
                note(demiSemiQuaver, C6);
                break;
            case 11:
                note(semiQuaver, C6);
                break;
            case 12:
                note(semiQuaver, G4);
                measureComplete = true;
                break;
        }
        noteNum++;    
    }
}

void measureSix(){
    measureComplete = false;
    while (cycles && !measureComplete)
    {
        switch (noteNum)
        {
            case 1:
                note(semiQuaver, C4);
                break;   
            case 2:
                note(semiQuaver, Eb5);
                break;
            case 3:
                rest(demiSemiQuaver);
                break;
            case 4:
                note(demiSemiQuaver, D5);
                break;
            case 5:
                rest(semiQuaver);
                break;
            case 6:
                note(demiSemiQuaver, C5);
                break;
            case 7:           
                rest(demiSemiQuaver);
                break;
            case 8:
                note(demiSemiQuaver, G4);
                break;
            case 9:
                note(semiQuaver, G4);
                break;
            case 10:
                note(semiQuaver, C4);
                measureComplete = true;
                break;
        }
        noteNum++;    
    }
}

void measureSeven(){
    measureComplete = false;
    while (cycles && !measureComplete)
    {
        switch (noteNum)
        {
            case 1:
                note(demiSemiQuaver, C5);
                break;   
            case 2:
                note(semiQuaver, C5);
                break;
            case 3:
                note(demiSemiQuaver, C5);
                break;
            case 4:
                rest(demiSemiQuaver);
                break;
            case 5:
                note(demiSemiQuaver, C5);
                break;
            case 6:
                note(semiQuaver, D5);
                break;
            case 7:           
                note(demiSemiQuaver, E5);
                break;
            case 8:
                note(semiQuaver, C5);
                break;
            case 9:
                note(demiSemiQuaver, lowA);
                break;
            case 10:
                note(semiQuaver, G5);
                break;
            case 11:
                note(semiQuaver, G4);
                measureComplete = true;
                break;
        }
        noteNum++;    
    }
}

void measureEight(){
    measureComplete = false;
    while (cycles && !measureComplete)
    {
        switch (noteNum)
        {
            case 1:
                note(demiSemiQuaver, C5);
                break;   
            case 2:
                note(semiQuaver, C5);
                break;
            case 3:
                note(demiSemiQuaver, C5);
                break;
            case 4:
                rest(demiSemiQuaver);
                break;
            case 5:
                note(demiSemiQuaver, C5);
                break;
            case 6:
                note(demiSemiQuaver, D5);
                break;
            case 7:           
                note(demiSemiQuaver, E5);
                break;
            case 8:
                rest(quaver);
                break;
            case 9:
                rest(semiQuaver);
                measureComplete = true;
                break;
        }
        noteNum++;    
    }
}

void MarioMovi(byte dir, float dist){
    PORTL = dir;
    cycles = dist * STEPS_PER_INCH; 
    if (DEBUG)
    {
        Serial.print("----------pushing MarioMovi-----------"); Serial.print("\n");
        Serial.print("dist: "); Serial.print(dist);Serial.print("\t");
        Serial.print("cycles: "); Serial.print(cycles);Serial.print("\n");
    }
    while (cycles)
    {
        switch (measureNum)
        {
            case 1:
                measureOne();
                break;
            case 2:
                measureTwo();
                break;
            case 3:
                measureThree();
                break;
            case 4:
                measureTwo();
                break;
            case 5:
                measureThree();
                break;
            case 6:
                measureFour();
                break;
            case 7:
                measureFive();
                break;
            case 8:
                measureFour();
                break;
            case 9:
                measureSix();
                break;   
            case 10:
                measureFour();
                break;
            case 11:
                measureFive();
                break;
            case 12:
                measureFour();
                break;
            case 13:
                measureSix();
                break;
            case 14:
                measureSeven();
                break;
            case 15:   
                measureEight();
                break;
        }
        if (measureComplete) 
        {
              noteNum = 1;
              measureNum++;
        }
    }
    
    if (!TIMSK3){
        startTimer();
    }
    else{
        popStack();
    }

 
}







#endif
