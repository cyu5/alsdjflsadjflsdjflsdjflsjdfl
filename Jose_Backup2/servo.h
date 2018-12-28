#ifndef SERVO_H
#define SERVO_H

#define SLIDER 52
#define GRABBER 53
#define MAGNET  41

#define SLIDER_IN 25
#define SLIDER_OUT 88
#define MAGNET_UP 140
#define MAGNET_DOWN 100

#define angleZero             500
#define MicrosecondPerDegree  ((float)2000/180)
#define MILLIS_PER_DEGREE     ((float)500/60)

#define milli2Cycles(milliHold)   ((milliHold)/20)
#define degree2Delay(degreeServo)   (degreeServo*MicrosecondPerDegree + angleZero)
#define degree2Cycles(degreeServo)   (milli2Cycles(degreeServo*MILLIS_PER_DEGREE) + 1)

#define setupServo()  ({\
        pinMode(SLIDER, OUTPUT);\
        pinMode(GRABBER, OUTPUT);\
        pinMode(MAGNET, OUTPUT); \
        digitalWrite(MAGNET, HIGH);\
        actuateServoN(SLIDER, degree2Delay(sliderAngle), milli2Cycles(1000));\
        actuateServoN(GRABBER, degree2Delay(grabberAngle), milli2Cycles(5000));\ 
        })

byte grabberAngle = MAGNET_UP;
byte sliderAngle = SLIDER_OUT;

//servo functions
//Low level Actuation in cycles and delays
void actuateServoN(byte pinServo, unsigned int pulseWidth, unsigned long cycles)
{
  unsigned int lowDelay = 20000 - pulseWidth;
  while(cycles--)
  {
    digitalWrite(pinServo, LOW);
    delayMicroseconds(lowDelay);
    digitalWrite(pinServo, HIGH);
    delayMicroseconds(pulseWidth);
  } 
  digitalWrite(pinServo, LOW);
}

//High level actuation in milliseconds and degrees
void grabberTurn(byte degreeServo)  
{
  byte degreeTurn = abs(grabberAngle - degreeServo);
  unsigned long cycles = degree2Cycles(degreeTurn);
  unsigned int pulseWidth = degree2Delay(degreeServo);

  actuateServoN(GRABBER, pulseWidth, cycles);
  grabberAngle = degreeServo;
}

void sliderTurn(byte degreeServo)  
{
  byte degreeTurn = abs(sliderAngle - degreeServo);
  unsigned long cycles = degree2Cycles(degreeTurn);
  unsigned int pulseWidth = degree2Delay(degreeServo);

  actuateServoN(SLIDER, pulseWidth, cycles);
  sliderAngle = degreeServo;
}

void pickUp()
{
    digitalWrite(MAGNET, LOW);
    grabberTurn(MAGNET_DOWN);
    delay(150);
    grabberTurn(MAGNET_UP);
    delay(150);
    sliderTurn(SLIDER_IN);
}

void leaveToken()
{
  sliderTurn(SLIDER_OUT);
  digitalWrite(MAGNET, HIGH);
  grabberTurn(MAGNET_DOWN);
  delay(150);
  grabberTurn(MAGNET_UP);
}


#endif
