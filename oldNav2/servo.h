#ifndef SERVO_H
#define SERVO_H


#define SLIDER 52
#define GRABBER 53
#define MAGNET  33

#define SLIDER_IN 28 
#define SLIDER_OUT 110
#define MAGNET_UP 110
#define MAGNET_DOWN 35

#define angleZero             500
#define cycleZero             1
#define MicrosecondPerDegree  ((float)2000/180)
#define MILLIS_PER_DEGREE     ((float)550/60)

#define milli2Cycles(milliHold)   (milliHold/20)
#define degree2Delay(degreeServo)   (degreeServo*MicrosecondPerDegree + angleZero)
#define degree2Cycles(degreeServo)   (degreeServo*MILLIS_PER_DEGREE*1000/20000 + cycleZero)

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
  //cycles = (cycles>3) ? cycles : 3;
  while(cycles--)
  {
    digitalWrite(pinServo, HIGH);
    delayMicroseconds(pulseWidth);
    digitalWrite(pinServo, LOW);
    delayMicroseconds(lowDelay);
  } 
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
    grabberTurn(MAGNET_UP);
    sliderTurn(SLIDER_IN);
}

void leaveToken()
{
  sliderTurn(SLIDER_OUT);
  digitalWrite(MAGNET, HIGH);
  grabberTurn(MAGNET_DOWN);
  grabberTurn(MAGNET_UP);
}


#endif
