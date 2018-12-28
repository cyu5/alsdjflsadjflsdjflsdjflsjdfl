#ifndef NAVIGATION_H
#define NAVIGATION_H

#include "lineSensor.h"
#include "interrupt_motion.h"
#include "colorSensor.h"

#define LCURVE 2
#define LEFT 0
#define MIDDLE NUM_LINES/2
#define RIGHT NUM_LINES
#define RCURVE NUM_LINES-3
#define BACK NUM_LINES+2
#define FORK  3
#define CROSS 4
#define SPLIT 2

char pastColor = YELLOW;
char currColor = YELLOW;
char destColor = GRAY;
char spot = 1;
char landmark = FORK;
char maneuver = MIDDLE;


void navigation(){
    while (sum != 4 && spot != 3)
    {
      lineFollow();
      if (sum == 4) spot++;
      while
    }
    
    
}





#endif
