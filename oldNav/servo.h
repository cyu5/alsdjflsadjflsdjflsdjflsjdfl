#ifndef SERVO_H
#define SERVO_H

#define angleZero             500
#define MicrosecondPerDegree  11.11


#define INITSLIDER 150 //Out of the way of the magnet
#define ENDSLIDER 50
#define INITGRABBER 105 //Up
#define ENDGRABBER 40

#define SLIDER 52
#define GRABBER 53
#define MAGNET  33

//servo functions
int milli2Cycles(double milliHold)
{
    double cycles = milliHold/20 + 50;
    return cycles;
}

int degree2Delay(float degreeServo)
{
    int highDelay = degreeServo*MicrosecondPerDegree + angleZero;
    return highDelay;
}

//Actuation Functions
//Low level Actuation in cycles and delays
void actuateServoN(int pinServo,int highDelay, double cycles)
{
  int lowDelay = 20000 - highDelay;

  for(double i = 0; i < cycles; i++)
  {
    digitalWrite(pinServo, HIGH);
    delayMicroseconds(highDelay);
    digitalWrite(pinServo, LOW);
    delayMicroseconds(lowDelay);
  } 
}

//High level actuation in milliseconds and degrees
void servoTurnN(int pinServo, int degreeServo, double milliHold)  
{
  int cycles = milli2Cycles(milliHold);
  int highDelay = degree2Delay(degreeServo);

  actuateServoN(pinServo, highDelay, cycles);
}

void grabberTurn(int degreeServo, double milliHold)  
{
  int cycles = milli2Cycles(milliHold);
  int highDelay = degree2Delay(degreeServo);

  actuateServoN(GRABBER, highDelay, cycles);
}

void sliderTurn(int degreeServo, double milliHold)  
{
  int cycles = milli2Cycles(milliHold);
  int highDelay = degree2Delay(degreeServo);

  actuateServoN(SLIDER, highDelay, cycles);
}

void pickUp()
{
    digitalWrite(MAGNET, LOW);
    grabberTurn(ENDGRABBER, 100);
    delay(1000);
    grabberTurn(INITGRABBER,50);
    sliderTurn(ENDSLIDER,50);
}

void leaveToken()
{
  sliderTurn(INITSLIDER, 2000);
  digitalWrite(MAGNET, HIGH);
  grabberTurn(ENDGRABBER, 200);
  grabberTurn(INITGRABBER, 200);
}

void setupServo()
{
  // Setup for the pins as outputs
  pinMode(SLIDER, OUTPUT);
  pinMode(GRABBER, OUTPUT);
  pinMode(MAGNET, OUTPUT); 
  digitalWrite(MAGNET,HIGH);
  sliderTurn(INITSLIDER, 500);
  grabberTurn(75, 3000);
}


#endif
