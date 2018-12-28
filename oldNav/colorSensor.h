#include <Wire.h>
#include "Adafruit_TCS34725.h"

Adafruit_TCS34725 colorSensor = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

#define setupColor() colorSensor.begin()

char coinColor;
unsigned int rgbColor[3];
#define YELLOW  0
#define CYAN 1
#define RED  2
#define BLUE  3
#define MAGENTA 4
#define GREEN  5
#define GRAY 6
#define NUM_COLORS 7
#define CENTER 7
#define VOID 8
#define MAGNET 9

#define RGB_Red {171, 48, 51} 
#define RGB_Green {52, 113, 85} 
#define RGB_Blue {32, 76, 144} 
#define RGB_Yellow {144, 139, 62} 
#define RGB_Magenta {106, 64, 86} 
#define RGB_Cyan {55, 103, 89} 
#define RGB_Gray {70, 91, 87} 
#define RGB_MAGNET {96, 91, 67} 

const unsigned int colorTable[NUM_COLORS][3] = {
              RGB_Yellow,
              RGB_Cyan, 
              RGB_Red,
              RGB_Blue,
              RGB_Magenta,
              RGB_Green,
              RGB_Gray
              };

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
  rgbColor[0] = r;
  rgbColor[1] = g;
  rgbColor[2] = b;
  unsigned int colorGap = -1;
  unsigned long sqSum;
  for (char i = 0; i<NUM_COLORS; i++){
      sqSum = 0;
      for (char j = 0; j < 3; j++) 
      {
          sqSum += sq(rgbColor[j]-colorTable[i][j]);
      }
      if (colorGap > sqSum)
      {
          colorGap = sqSum;
          coinColor = i;
      }
  }
  if (colorGap > 100) coinColor = VOID;

}

void showColor(char color = coinColor){
  if (color == RED) Serial.print("Red");
  else if (color == BLUE) Serial.print("Blue");
  else if (color == GREEN) Serial.print("Green");
  else if (color == YELLOW) Serial.print("Yellow");
  else if (color == MAGENTA) Serial.print("Magenta");
  else if (color == CYAN) Serial.print("Cyan");
  else if (color == GRAY) Serial.print("Gray");
  else if (color == VOID) Serial.println("Void");
  Serial.print(" --- ");
  Serial.print("R:");Serial.print(rgbColor[0]); Serial.print('\t'); 
  Serial.print("G:");Serial.print(rgbColor[1]); Serial.print('\t');
  Serial.print("B:");Serial.print(rgbColor[2]); Serial.print('\n');
}


//recycle bin

/*

 */
