#include <Wire.h>
#include "Adafruit_TCS34725.h"

//Adafruit_TCS34725 colorSensor = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
Adafruit_TCS34725 colorSensor = Adafruit_TCS34725(); //integration means waiting time, gain means sentivity to light

#define setupColor() colorSensor.begin()

byte coinColor;
byte rgbColor[4];
byte colorGap = -1;
#define YELLOW  0
#define CYAN 1
#define RED  2
#define BLUE  3
#define MAGENTA 4
#define GREEN  5
#define GRAY 6
#define NUM_COLORS 7
#define VOID 7

#define RGB_RED     {{120 , 165 , 180},  {45  ,  52 ,  60},  {45  ,  52 ,  60}}
#define RGB_GREEN   {{70  ,  80 ,  85},  {100 ,  108, 115},  {55  ,  62 ,  70}}
#define RGB_BLUE    {{35  ,  43 ,  55},  {75  ,  80 ,  85},  {110 , 125 , 135}}
#define RGB_YELLOW  {{90  , 100 , 110},  {90  ,  95 , 100},  {50  ,  55 ,  60}}
#define RGB_MAGENTA {{85  , 105 , 115},  {60  ,  65 ,  75},  {75  ,  85 ,  95}}
#define RGB_CYAN    {{35  ,  45 ,  55},  {90  ,  99 , 105},  {95  , 110 , 115}} 
#define RGB_GRAY    {{60  ,  72 ,  85},  {85  ,  90 ,  95},  {80  ,  85 ,  90}}  

const byte colorTable[NUM_COLORS][3][3] = {
              RGB_YELLOW,
              RGB_CYAN, 
              RGB_RED,
              RGB_BLUE,
              RGB_MAGENTA,
              RGB_GREEN,
              RGB_GRAY
              };


void rgbRead(){
  
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
  rgbColor[3] = clear/10;
}

void readColor(byte reads = 1) {

  colorGap = -1;
  coinColor = VOID;
  while (reads--){ 
      //get raw RGB reading
      rgbRead();  
      //figure out the color
      for (char i = 0; i<NUM_COLORS; i++)
      {
          unsigned long sqSum = 0;
          byte rgbMatch = 0;
          for (char j = 0; j < 3; j++) 
          {
              if (rgbColor[j] >= colorTable[i][j][0] && rgbColor[j] <= colorTable[i][j][2])
              {
                  sqSum += sq(rgbColor[j]-colorTable[i][j][1]);
                  rgbMatch++;
              }
          }
          if (rgbMatch == 3 && sqSum < colorGap)
          {
              colorGap = sqSum;
              coinColor = i;
              return;
          }
      }
      
  }
}


void sampleColor(){
    byte reads = -1;
    byte samples[256][4];

    //get samples
    while (reads--)
    {
        rgbRead();
        for (byte i=0; i<4; i++)
        {
            samples[reads][i] = rgbColor[i];
        }
        Serial.print("R:");Serial.print(rgbColor[0]); Serial.print('\t'); 
        Serial.print("G:");Serial.print(rgbColor[1]); Serial.print('\t');
        Serial.print("B:");Serial.print(rgbColor[2]); Serial.print('\t');
        Serial.print("Sum:");Serial.print(rgbColor[3]); Serial.print('\n');
    }

    //sort samples
    for (byte i=0; i<4; i++)
    {
        for (byte j=1; j!=0; j++)
        {
            byte pos = j;
            byte small = samples[j][i];
            for (byte k = j + 1; k!=0; k++)
            {
                if(samples[k][i] < small) 
                {
                    small = samples[k][i];
                    pos = k;
                }
            }
            byte temp = samples[pos][i];
            samples[pos][i] = samples[j][i];
            samples[j][i] = temp;
        }
    }

    //print sorted samples
//    for (byte j=1; j!=0; j++)
//    {
//        Serial.print("sample ");Serial.print(j); Serial.print('\t'); 
//        Serial.print("R:");Serial.print(samples[j][0]); Serial.print('\t'); 
//        Serial.print("G:");Serial.print(samples[j][1]); Serial.print('\t');
//        Serial.print("B:");Serial.print(samples[j][2]); Serial.print('\t');
//        Serial.print("Sum:");Serial.print(samples[j][3]); Serial.print('\t');
//        Serial.print('\n');
//    }



    //count sorted samples
    byte sampleValues[256][4];
    byte samplePer[256][4];
    byte sampleCnt[256][4];
    byte variation[4];
    for (byte i=0; i<4; i++)
    {
        sampleCnt[1][i] = 0;
        sampleValues[1][i] = samples[1][i];
        variation[i] = 1;
        for (byte j=1; j; j++)
        {
            if (sampleValues[variation[i]][i] != samples[j][i])
            {
                variation[i]++;
                sampleCnt[variation[i]][i] = 0;
                sampleValues[variation[i]][i] = samples[j][i];          
            }
            samplePer[variation[i]][i] = (long)100*j/255;
            sampleCnt[variation[i]][i]++;
        }
    }

    //print simplified sorted samples
    bool moreToGo = true;
    for (byte j=1; j!=0 && moreToGo; j++)
    {
        Serial.print("sample ");Serial.print(j); Serial.print('\t');
        moreToGo = false; 
        for (byte i=0; i<4; i++)
        {
            if (j <= variation[i])
            {
                Serial.print(sampleValues[j][i]);Serial.print(":"); Serial.print(sampleCnt[j][i]);Serial.print(":"); Serial.print(samplePer[j][i]);Serial.print("% "); 
                moreToGo = true; 
            }
            Serial.print('\t');
        }
        Serial.print('\n');
    }

    //calculate mean and thresholds
    unsigned int mean[4] = {0};
    for (byte i=0; i<4; i++)
    {
        for (byte j=1; j!=0; j++)
        {
            mean[i] += samples[j][i];
        }
        mean[i] = mean[i]/255;
    }

    byte mode[4];
    for (byte i=0; i<4; i++)
    {
        byte mostCnt = sampleCnt[1][i];
        mode[i] = sampleValues[1][i];
        for (byte j=1; j<=variation[i]; j++)
        {
            if (mostCnt < sampleCnt[j][i]) 
            {
                mostCnt = sampleCnt[j][i];
                mode[i] = sampleValues[j][i];
            }
        }
    }
    Serial.print("mean "); 
    Serial.print("R:");Serial.print(mean[0]); Serial.print('\t'); 
    Serial.print("G:");Serial.print(mean[1]); Serial.print('\t');
    Serial.print("B:");Serial.print(mean[2]); Serial.print('\t');
    Serial.print("Sum:");Serial.print(mean[3]); Serial.print('\t');
    Serial.print('\n');

    Serial.print("middle "); 
    Serial.print("R:");Serial.print(samples[127][0]); Serial.print('\t'); 
    Serial.print("G:");Serial.print(samples[127][1]); Serial.print('\t');
    Serial.print("B:");Serial.print(samples[127][2]); Serial.print('\t');
    Serial.print("Sum:");Serial.print(samples[127][3]); Serial.print('\t');
    Serial.print('\n');

    Serial.print("median "); 
    Serial.print("R:");Serial.print(sampleValues[variation[0]/2][0]); Serial.print('\t'); 
    Serial.print("G:");Serial.print(sampleValues[variation[1]/2][1]); Serial.print('\t');
    Serial.print("B:");Serial.print(sampleValues[variation[2]/2][2]); Serial.print('\t');
    Serial.print("Sum:");Serial.print(sampleValues[variation[3]/2][3]); Serial.print('\t');
    Serial.print('\n');

    Serial.print("mode "); 
    Serial.print("R:");Serial.print(mode[0]); Serial.print('\t'); 
    Serial.print("G:");Serial.print(mode[1]); Serial.print('\t');
    Serial.print("B:");Serial.print(mode[2]); Serial.print('\t');
    Serial.print("Sum:");Serial.print(mode[3]); Serial.print('\t');
    Serial.print('\n');

    Serial.print("index threshold "); 
    Serial.print(samples[5*255/100][0]);Serial.print(":");Serial.print(samples[95*255/100][0]); Serial.print('\t'); 
    Serial.print(samples[5*255/100][1]);Serial.print(":");Serial.print(samples[95*255/100][1]); Serial.print('\t');
    Serial.print(samples[5*255/100][2]);Serial.print(":");Serial.print(samples[95*255/100][2]); Serial.print('\t');
    Serial.print(samples[5*255/100][3]);Serial.print(":");Serial.print(samples[95*255/100][3]); Serial.print('\t');
    Serial.print('\n');

    Serial.print("value threshold "); 
    Serial.print(sampleValues[25*variation[0]/100][0]);Serial.print(":");Serial.print(sampleValues[75*variation[0]/100][0]); Serial.print('\t'); 
    Serial.print(sampleValues[25*variation[1]/100][1]);Serial.print(":");Serial.print(sampleValues[75*variation[1]/100][1]); Serial.print('\t');
    Serial.print(sampleValues[25*variation[2]/100][2]);Serial.print(":");Serial.print(sampleValues[75*variation[2]/100][2]); Serial.print('\t');
    Serial.print(sampleValues[25*variation[3]/100][3]);Serial.print(":");Serial.print(sampleValues[75*variation[3]/100][3]); Serial.print('\t');
    Serial.print('\n');

}

void showColor(char color = coinColor){
  if (color == RED) Serial.print("RED");
  else if (color == BLUE) Serial.print("BLUE");
  else if (color == GREEN) Serial.print("GREEN");
  else if (color == YELLOW) Serial.print("YELLOW");
  else if (color == MAGENTA) Serial.print("MAGENTA");
  else if (color == CYAN) Serial.print("CYAN");
  else if (color == GRAY) Serial.print("GRAY");
  else if (color == VOID) Serial.print("Void");
  Serial.print(" --- ");
  Serial.print("R:");Serial.print(rgbColor[0]); Serial.print('\t'); 
  Serial.print("G:");Serial.print(rgbColor[1]); Serial.print('\t');
  Serial.print("B:");Serial.print(rgbColor[2]); Serial.print('\n');
}
