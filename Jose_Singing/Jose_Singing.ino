#include "lineFollow.h"

#define 

void setup()
{
  Serial.begin(9600);         //for debugging
  setupTimer();
  setupLine();
//  cycles = 30 * STEPS_PER_INCH; 
//  measureOne();
//  measureTwo();
//  measureThree();
//  startTimer();
//  PORTL=0;

  MarioDrivi(100);
  showStack();
  while(TIMSK3);
  showStack();
  while(1);
  
}

void loop()
{
  //TWINKLE TWINKLE, LITTLE STAR
  //MICHAEL CERASO
//  //MEASURE 1
//  note(semiQuaver, C5);
//  note(semiQuaver, C5);
//  note(semiQuaver, G5);
//  note(semiQuaver, G5);
//  //MEASURE 2
//  note(semiQuaver, Ab5 - 25);
//  note(semiQuaver, Ab5 - 25);
//  note(quaver, G5); 
//  //Measure 3
//  note(semiQuaver, C5);
//  note(semiQuaver, C5);
//  note(semiQuaver, G5);
//  note(semiQuaver, G5);
//  //
  cycles = -1;
  
  measureOne();
  measureTwo();
  measureThree();


  measureTwo();
  measureThree();


  measureFour();
  measureFive();


  measureFour();
  measureSix();


  measureFour();
  measureFive();


  measureFour();
  measureSix();


  measureSeven();
  measureEight();
  startTimer();
  while(TIMSK3);
}



/*

//
//MAKE THE STEPPER MOTORS SING
//
unsigned int note(int note, int pitch){
    unsigned int steps = 0;
    pitch = pitch/2;
    unsigned long startedAt = millis();
    while((millis()-startedAt) < note)
    {
        PORTL ^= allWheels;
        steps++;
        unsigned long startTime = micros();
        while((micros()-startTime) < pitch);
    }
    delay(50);
    return steps;
}

void pushNote(int note, int pitch, unsigned long steps = 10000){
      unsigned int period = pitch/2;
      unsigned long cycles = 1000*note/period;
      if (cycles < steps)
      {
          pushStack(cycles, period);
          steps -= cycles;
      }
      else 
      {
          pushStack(steps, period);
          steps = 0;
      }
      
      
}

void pushRest(int note){
    pushStack(1, note*1000);
}

void rest(int note){
  delay(note);
}


void measureOne(){
//setRotateRight(); 
note(demiSemiQuaver, E5);
note(semiQuaver, E5);
note(demiSemiQuaver, E5);
rest(demiSemiQuaver);
note(demiSemiQuaver, C5);
note(semiQuaver, E5);
note(quaver, G5); 
note(quaver, G4);
}
void measureTwo(){
//setRotateLeft(); 
note(semiQuaver, C5);
rest(demiSemiQuaver);
note(demiSemiQuaver, G5);
rest(semiQuaver);
note(semiQuaver, E4);
rest(demiSemiQuaver);
note(demiSemiQuaver, highA);
rest(demiSemiQuaver);
note(demiSemiQuaver, B5);
rest(demiSemiQuaver);
note(demiSemiQuaver, Bb5);
note(semiQuaver, highA);
}
void measureThree(){
//setRotateRight(); 
note(triplet, G4);
note(triplet, E5);
note(triplet, G5);
note(semiQuaver, highA);
note(demiSemiQuaver, F5);
note(demiSemiQuaver, G5);
rest(demiSemiQuaver);
note(demiSemiQuaver, E5);
rest(demiSemiQuaver);
note(demiSemiQuaver, C5);
note(demiSemiQuaver, D5);
note(demiSemiQuaver, B5);
rest(semiQuaver);
}
//measure 2 again

//measure 3 again

void measureFour(){
//setStrafeLeft(); 
note(semiQuaver, C4);
note(demiSemiQuaver, G5);
note(demiSemiQuaver, Gb5);
note(demiSemiQuaver, F5);
note(semiQuaver, Eb5);
note(demiSemiQuaver, E5);
rest(demiSemiQuaver);
note(demiSemiQuaver, Ab5);
note(demiSemiQuaver, highA);
note(demiSemiQuaver, C5);
rest(demiSemiQuaver);
note(demiSemiQuaver, highA);
note(demiSemiQuaver, C5);
note(demiSemiQuaver, D5);
}
void measureFive(){
//setStrafeRight(); 
note(semiQuaver, C4);
note(demiSemiQuaver, G5);
note(demiSemiQuaver, Gb5);
note(demiSemiQuaver, F5);
note(semiQuaver, Eb5);
note(demiSemiQuaver, E5);
rest(demiSemiQuaver);
note(demiSemiQuaver, C6);
rest(demiSemiQuaver);
note(demiSemiQuaver, C6);
note(semiQuaver, C6);
note(semiQuaver, G4);
}
//measure 4 again

void measureSix(){
//setStrafeLeft(); 
note(semiQuaver, C4);
note(semiQuaver, Eb5);
rest(demiSemiQuaver);
note(demiSemiQuaver, D5);
rest(semiQuaver);
note(demiSemiQuaver, C5);
rest(demiSemiQuaver);
note(demiSemiQuaver, G4);
note(semiQuaver, G4);
note(semiQuaver, C4);
}
//measure 4 again

//measure 5 again

//measure 4 again

//measure 6 again

void measureSeven(){
//setForward();
note(demiSemiQuaver, C5);
note(semiQuaver, C5);
note(demiSemiQuaver, C5);
rest(demiSemiQuaver);
note(demiSemiQuaver, C5);
note(semiQuaver, D5);
note(demiSemiQuaver, E5);
note(semiQuaver, C5);
note(demiSemiQuaver, lowA);
note(semiQuaver, G5);
note(semiQuaver, G4);
}
void measureEight(){
//setBackward();
note(demiSemiQuaver, C5);
note(semiQuaver, C5);
note(demiSemiQuaver, C5);
rest(demiSemiQuaver);
note(demiSemiQuaver, C5);
note(demiSemiQuaver, D5);
note(demiSemiQuaver, E5);
rest(quaver);
rest(semiQuaver);
}
*/
