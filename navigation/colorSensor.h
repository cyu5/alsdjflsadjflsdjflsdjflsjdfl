#include <Wire.h>
#include "Adafruit_TCS34725.h"

Adafruit_TCS34725 colorSensor = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

#define setupColor() colorSensor.begin()

char color;
#define GRAY 0
#define YELLOW  1
#define CYAN 2
#define RED  3
#define BLUE  4
#define MAGENTA   5
#define GREEN  6
#define NUM_COLORS 7

#define RGB_Red {180, 47, 49} 
#define RGB_Green {74, 103, 84} 
#define RGB_Blue {44, 84, 138} 
#define RGB_Yellow {121, 94, 42} 
#define RGB_Magenta {117, 67, 81} 
#define RGB_Cyan {67, 103, 87} 
#define RGB_Gray {84, 90, 85} 

const unsigned int colorTable[NUM_COLORS][3] = {
              RGB_Gray,
              RGB_Yellow,
              RGB_Cyan, 
              RGB_Red,
              RGB_Blue,
              RGB_Magenta,
              RGB_Green,
              };

/*
unsigned int rgbRed[3] = {180, 47, 49};
unsigned int rgbGreen[3] = {74, 103, 84};
unsigned int rgbBlue[3] = {44, 84, 138};
unsigned int rgbYellow[3] = {121, 94, 42};
unsigned int rgbMagenta[3] = {117, 67, 81};
unsigned int rgbCyan[3] = {67, 103, 87};
unsigned int rgbGray[3] = {84, 90, 85};
//unsigned int rgbBlack[3] = {92, 88, 79};
//unsigned int rgbVoid[3] = {0, 0, 0};
*/

//this function might require an additional delay for the read color to settle
void readColor() {

  uint16_t clear, red, green, blue;
  colorSensor.getRawData(&red, &green, &blue, &clear);    
  uint32_t sum = clear;
  float r, g, b;
  r = red; r /= sum;
  g = green; g /= sum;
  b = blue; b /= sum;
  r *= 256; g *= 256; b *= 256;
  unsigned int rgbColor[3] = {(unsigned int)r, (unsigned int)g, (unsigned int)b};
  unsigned int colorGap = -1;
  unsigned int temp;
  for (char i = 0; i<NUM_COLORS; i++){
      temp = 0;
      for (char j = 0; j < 3; j++) {
          temp += sq(rgbColor[j]-colorTable[i][j]);
      }
      if (colorGap > temp){
          colorGap = temp;
          color = i;
      }
  }

}

void showColor(char color){
  if (color == RED) Serial.print("Red");
  else if (color == BLUE) Serial.print("Blue");
  else if (color == GREEN) Serial.print("Green");
  else if (color == YELLOW) Serial.print("Yellow");
  else if (color == MAGENTA) Serial.print("Magenta");
  else if (color == CYAN) Serial.print("Cyan");
  else if (color == GRAY) Serial.print("Gray");
  //else if (color == VOID) Serial.println("Void");
}

//recycle bin

/*



  int redGap = colorGap(rgbColor, theRed);
  int blueGap = colorGap(rgbColor, theBlue);
  int greenGap = colorGap(rgbColor, theGreen);
  int yellowGap = colorGap(rgbColor, theYellow);
  int magentaGap = colorGap(rgbColor, theMagenta);
  int cyanGap = colorGap(rgbColor, theCyan);
  int grayGap = colorGap(rgbColor, theGray);
  int minGap = min(min(min(min(min(min(min(redGap, blueGap), greenGap), yellowGap), magentaGap), cyanGap), grayGap), blackGap);


  else if (minGap == redGap) return RED;
  else if (minGap == blueGap) return BLUE;
  else if (minGap == greenGap) return GREEN;
  else if (minGap == yellowGap) return YELLOW;
  else if (minGap == magentaGap) return MAGENTA;
  else if (minGap == cyanGap) return CYAN;
  else if (minGap == grayGap) return GRAY;
 */
