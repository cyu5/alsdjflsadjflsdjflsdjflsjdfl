#ifndef LINE_SENSOR_H
#define LINE_SENSOR_H

#include <QTRSensors.h>
#define NUM_SENSORS   8     // number of sensors used, QTR_MAX_SENSORS defined as 16
#define TIMEOUT       1500  // waits for 2500 microseconds for sensor outputs to go high (or line goes white)
                            // recommanded 1000 to 3000
                            // TIMEOUT microseconds per all NUM_SENSORS
                            // best performance: 1000 microsecond per 16 sensors read
#define EMITTER_PIN   2     // emitter is controlled by digital pin 2, QTR_NO_EMITTER_PIN defined as 255

QTRSensorsRC lineSensor((unsigned char[]) {3, 4, 5, 6, 7, 8, 9, 10},
  NUM_SENSORS, TIMEOUT, QTR_NO_EMITTER_PIN); 

#define NUM_LINES  NUM_SENSORS-1                  // max number of line detectable is NUM_SENSORS-1
                                                // assuming a line can only occupy no more than 2 sensors
                                                // and lines can overlap or share a sensor
#define LINE_THRASHOLD (TIMEOUT * 0.25)
#define CAL_TIMEOUT 1000


unsigned int sensorValues[NUM_SENSORS];
unsigned int position = 3500;                     // assume average of all lines starts in the middle
unsigned int line[NUM_LINES];                      // positions of each line
unsigned char lineCnt = 0;                        // number of lines detected

unsigned int prevLine[NUM_LINES];
unsigned char prevLineCnt = 0;
int stackLeft = 0;
int stackRight = NUM_LINES - 1;
int tracking;
int match[NUM_LINES];

#define track(t) tracking=t

void readLineSensor(){


    lineSensor.readCalibrated(sensorValues, 1); //readmode = 1 for emitter on (default)
    lineCnt = 0;
    unsigned long avg[NUM_LINES+1] = {0};     // this is for the weighted total, which is long
                                                // before division
    unsigned int sum[NUM_LINES+1] = {0};     // this is for the denominator which is <= 64000
    unsigned char on_line = 0;               // indicate if the previous sensor has seen a line
    
    for(unsigned char sensorNum=0; sensorNum<NUM_SENSORS; sensorNum++) {
        
        unsigned int shareValue;
        long shareWeightedValue;
        unsigned int value = sensorValues[sensorNum];
        long weightedValue = (long)(value) * (sensorNum * 1000);      // this value tends to be long
        
        // keep track of number of lines seen so far
        if(value > 200) {
            if (on_line==0){
                lineCnt++;                           // record this line
                on_line = 1;                     // tells the next sensor this line has already seen
            }
            else{ 
                if (on_line==1){
                    on_line = 2;                    // tells the next sensor to look for another line
                }
                else {
                    lineCnt++;
                    avg[lineCnt] += shareWeightedValue;
                    sum[lineCnt] += shareValue;
                }
                shareWeightedValue = weightedValue;
                shareValue = value;
            }
            // average in values for this line over two sensor span
            avg[lineCnt] += weightedValue;
            sum[lineCnt] += value;
            // average in values for all sensors
            avg[0] += weightedValue;
            sum[0] += value;
        }
        
        // no line on this sensor
        else {
            on_line = 0;                       // the last line only occupy the till the last sensor
        }
        
    }

    if(lineCnt == 0)
    {
        // If it last read to the left of center, return left.
        if(position < (NUM_SENSORS-1)*1000/2){
            position = 0;
        }
        // If it last read to the right of center, return right.
        else{
            position = (NUM_SENSORS-1)*1000;
        }
    }
    else    position = avg[0]/sum[0];


    unsigned char lineNum;
    // crunch positions calculation
    for (lineNum = 0; lineNum < lineCnt; lineNum++){
        line[lineNum] = avg[lineNum+1]/sum[lineNum+1]; 
    }

    // change numbering of tracking of lost lines from queue to stack
    if (tracking < 0) {
        tracking = stackLeft - tracking - 1;
    }
    else if (tracking >= NUM_LINES){
        tracking = NUM_LINES + stackRight - tracking;
    }
    
    unsigned int closeness;
    unsigned char prevLineNum;
    unsigned int matchValue;
    unsigned char lastMatch = 0;

    for (prevLineNum = 0; prevLineNum < prevLineCnt; prevLineNum++){
        closeness = ((NUM_SENSORS-1)*1000)/2;
        matchValue = prevLine[prevLineNum];
        boolean no_line = true;
        for (lineNum = lastMatch; lineNum < lineCnt; lineNum++){
            no_line = false;
            int temp = line[lineNum] - matchValue;
            temp = abs(temp);
            if (closeness >= temp){
                closeness = temp;
                lastMatch = lineNum;
            }
            else{
                break;
            }
        }
        match[prevLineNum] = lastMatch;
        //Serial.print("pL[");//Serial.print((int)prevLineNum);//Serial.print("]");//Serial.print(matchValue);
        if(!no_line)  
        {
            //Serial.print("-nL[");//Serial.print((int)lastMatch);//Serial.print("]");//Serial.print(line[lastMatch]); 
            //Serial.print("-closeness:");//Serial.print(closeness);
        }

        if (matchValue < 1000) matchValue = 1000+(1000/2);
        else if (matchValue > (NUM_SENSORS-2)*1000) matchValue = ((NUM_SENSORS-2)*1000) - (1000/2);
        
        if (matchValue < closeness){    
                match[prevLineNum] = --stackLeft;
                //Serial.print("left(");
                //Serial.print(stackLeft);
                //Serial.print(")");
        }
        else if ((((NUM_SENSORS-1)*1000) - matchValue) < closeness){  
            for (char temp = prevLineCnt-1; temp >= prevLineNum; temp--){
                match[temp] = ++stackRight;
                //Serial.print("right(");
                //Serial.print(stackRight);
                //Serial.print(")");
            }
            break;
        }
        //Serial.print('\t');
        
    }
    //Serial.print('\n');

    
    boolean matched;
    lastMatch = 0;
    for (lineNum = 0; lineNum < lineCnt; lineNum++){
        matched = false;
        closeness = ((NUM_SENSORS-1)*1000)/2;
        matchValue = line[lineNum];
        boolean no_line = true;
        //Serial.print("nL[");//Serial.print((int)lineNum);//Serial.print("]");//Serial.print(line[lineNum]);//Serial.print("-");
        
        for (prevLineNum = lastMatch; prevLineNum < prevLineCnt; prevLineNum++){
            no_line = false;
            int temp = prevLine[prevLineNum] - matchValue;
            temp = abs(temp);
            if (match[prevLineNum] == lineNum){
                matched = true;
                lastMatch = prevLineNum;
                //Serial.print("pL[");//Serial.print((int)prevLineNum);//Serial.print("]");//Serial.print(prevLine[prevLineNum]); 
                break;
            }
            else if (closeness >= temp){
                closeness = temp;
                lastMatch = prevLineNum;
            }
            else {
                //lastMatch = prevLineNum--;
                break;
            }
            
        }
    
        
        if (!matched){
            if (!no_line){
                //Serial.print("pL[");//Serial.print((int)lastMatch);//Serial.print("]");//Serial.print(prevLine[lastMatch]); 
                //Serial.print("-closeness:");//Serial.print(closeness);
            }
            if (matchValue < closeness){
                if (stackLeft<0){
                    //Serial.print("left(");
                    //Serial.print(stackLeft);
                    //Serial.print(")");
                    stackLeft++;
                }
            }
            else if (((NUM_SENSORS-1)*1000 - matchValue) < closeness){
                if (stackRight>NUM_LINES-1) {
                    //Serial.print("right(");
                    //Serial.print(stackRight);
                    //Serial.print(")");
                    stackRight--;
                }
            }
            
        }
        //Serial.print('\t');
    }
    //Serial.print('\n');


    //Serial.print(" to line ");
    if (tracking >= 0 && tracking < NUM_LINES){
        tracking = match[tracking];
        if (tracking >= 0 && tracking < NUM_LINES){
            //Serial.print(tracking);//Serial.print(" at ");//Serial.print(line[tracking]);
        }
    }
    // converts lost line tracking number from stack ptr back to queue ptr
    // this "if" is separated from the first for self-correction purpose
    // it prevents the mismatching of a line as lost due to high proximity to the edge
    if (tracking < 0) { // && tracking < stackLeft
        tracking = stackLeft - tracking - 1;
        if (tracking < 0){
            //Serial.print(tracking);//Serial.print(" at ");//Serial.print("0");
        }
        else {
            //Serial.print(tracking);//Serial.print(" at ");//Serial.print(line[tracking]); 
        }
    }
    else if (tracking >= NUM_LINES){  // && tracking > stackRight
        tracking = NUM_LINES + stackRight - tracking;
        if (tracking >= NUM_LINES){
            //Serial.print(tracking);//Serial.print(" at ");//Serial.print((int)(NUM_SENSORS-1)*1000);
        }
        else {
            tracking = lineCnt - (NUM_LINES - tracking);
            //Serial.print(tracking);//Serial.print(" at ");//Serial.print(line[tracking]);
        }
        
    }
/*
*/

    //Serial.print('\n');
    
    for (lineNum = 0; lineNum < lineCnt; lineNum++){
        prevLine[lineNum] = line[lineNum];
    }
    prevLineCnt = lineCnt;
    //Serial.print('\n');

}

void printLineSensor(){
    
    for (unsigned char i = 0; i < NUM_SENSORS; i++)
    {
      Serial.print(sensorValues[i]);
      Serial.print('\t');
    }
    
    Serial.print("P");Serial.print(": ");
    Serial.print(position);
    Serial.print('\t');
    
    for (unsigned char i = 0; i < lineCnt; i++)
    {
      Serial.print("L");Serial.print(i);Serial.print(": ");
      Serial.print(line[i]);
      Serial.print('\t');
    }
    
    Serial.print('\n');
    
}

void hardcodeCalibrate(){

    //hardcoded values
    unsigned int calmin[NUM_SENSORS] = {64, 64, 64, 124, 128, 128, 128, 250};

    // Allocate the arrays if necessary.
    if(lineSensor.calibratedMaximumOn == 0)
    {
        lineSensor.calibratedMaximumOn = (unsigned int*)malloc(sizeof(unsigned int)*NUM_SENSORS);
  
        // If the malloc failed, don't continue.
        if(lineSensor.calibratedMaximumOn == 0)
            return; 
    }
    if(lineSensor.calibratedMinimumOn == 0)
    {
        lineSensor.calibratedMinimumOn = (unsigned int*)malloc(sizeof(unsigned int)*NUM_SENSORS);
  
        // If the malloc failed, don't continue.
        if(lineSensor.calibratedMinimumOn == 0)
            return;
    }

    //put pre-coded calibrated values into the object memory
    for (int i = 0; i < NUM_SENSORS; i++){
      lineSensor.calibratedMaximumOn[i] = TIMEOUT;
    }
    
    for (int i = 0; i < NUM_SENSORS; i++){
      lineSensor.calibratedMinimumOn[i] = LINE_THRASHOLD; //= calmin[i];
    }
}

void manualCalibrate (){
    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);    
    for (int i = 0; i < 400; i++)  // make the calibration take about 10 seconds
    {
      lineSensor.calibrate();    // reads all sensors 10 times at 2500 us per read (i.e. ~25 ms per call)
    }
    digitalWrite(13, LOW);
    
    //filter more noise for calmin
    for (int i = 0; i < NUM_SENSORS; i++)
    {
      lineSensor.calibratedMinimumOn[i] *= (float)6/5;
    }
    
}

//recycle bin for functions

/*


void findPosition(){
  
    unsigned char i;      //for loop counter
    unsigned long avg = 0; // this is for the weighted total, which is long
                       // before division
    unsigned int sum = 0; // this is for the denominator which is <= 64000
    lineCnt = 0;      //total number of lines detected
    lineSensor.readCalibrated(sensorValues, 1); //readmode = 1 for emitter on (default)
    

    for(i=0;i<NUM_SENSORS;i++) {
        int value = sensorValues[i];

        // keep track of whether we see the line at all
        if(value > 200) {
            lineCnt++;
        }

        // only average in values that are above a noise threshold
        if(value > 50) {
            avg += (long)(value) * (i * 1000);
            sum += value;
        }
    }

    if(lineCnt == 0)
    {
        // If it last read to the left of center, return left.
        if(position < (NUM_SENSORS-1)*1000/2){
            position = 0;
            return;
        }
        // If it last read to the right of center, return right.
        else{
            position = (NUM_SENSORS-1)*1000;
            return;
        }
    }

    position = avg/sum;    
}
void readLineSensor2(){

    segCnt = 0;      //total number of lines detected
    lineSensor.readCalibrated(sensorValues, 1); //readmode = 1 for emitter on (default)

    for(unsigned char i=0;i<NUM_SENSORS;i++) {
        int value = sensorValues[i];

        // keep track of whether we see the line at all
        if(value > 200) {
            sensorNum[segCnt++] = i;         // record the sensorNum and increment count
        }
    }
}

void findPosition2(unsigned char lineNum = segCnt/2, unsigned char range = NUM_SENSORS/2){

    if(segCnt == 0)
    {
        // If it last read to the left of center, return left.
        if(position < (NUM_SENSORS-1)*1000/2){
            position = 0;
            return;
        }
        // If it last read to the right of center, return right.
        else{
            position = (NUM_SENSORS-1)*1000;
            return;
        }
    }

    // check and correct for out-of-range sensor numbers
    if (range > NUM_SENSORS-1)
        range = NUM_SENSORS-1;
    if (lineNum > segCnt - 1)
        lineNum = segCnt - 1;
    char leftSensor = sensorNum[lineNum] - range;
    char rightSensor = sensorNum[lineNum] + range;
    if (leftSensor < 0)  
        leftSensor = 0;
    if (rightSensor > NUM_SENSORS-1) 
        rightSensor = NUM_SENSORS-1;

    unsigned char i;       //for loop counter
    unsigned long avg = 0; // this is for the weighted total, which is long
                       // before division
    unsigned int sum = 0; // this is for the denominator which is <= 64000
    
    for(i=leftSensor;i<=rightSensor;i++) {
        int value = sensorValues[i];

        // only average in values that are above a noise threshold
        if(value > 50) {
            avg += (long)(value) * (i * 1000);
            sum += value;
        }
    }
    
    position = avg/sum;    
}


 

void calibrationHelper (QTRSensorsRC &lineSensor){
  //read raw values
  lineSensor.read(sensorValues);
  Serial.print("calmin[i]:");
  Serial.print('\t');
  // determine weather the new value is more suitable for calmin, print the calmin values
  for (unsigned char i = 0; i < NUM_SENSORS; i++)
  {
    unsigned int value = sensorValues[i];
    if (value < 200 && value > calmin[i]){
      calmin[i] = value;
    }
    Serial.print(calmin[i]);
    Serial.print('\t'); 
  }
  Serial.println();
  delay(250);
}*/

/*int findPosition(QTRSensorsRC &lineSensor){
  
    position = lineSensor.readLine(sensorValues);
    unsigned int sum = 0;
    char line[NUM_SENSORS] = {0};
    
    for (unsigned char i = 0; i < NUM_SENSORS; i++){
      if (sensorValues[i] > 50) sum += sensorValues[i]; 
      if (sensorValues[i] > 200) line[i] = 1;
    }
    
    if (position > 3800 && position < 3200  && sum < 1200) return CENTERED;
    else if ((position < 3200) && (sum < 1200)) return LEFT; 
    else if (position > 3800 && sum < 1200) return RIGHT; 
}*/

  /*#define aligned 0
  #define left 1
  #define right 2
  #define leftTurn 3
  #define rightTurn 4
  #define tilted 5
  #define onLine 6
  int input;
  if (line == 0)  Serial.print("no line");
  else if (line == 1){
    Serial.print("1 line ");
    if (position > 3800 && position < 3200  && sum < 1200) input = aligned;
    else if ((position < 3200) && (sum < 1200)) input = left; 
    else if (position > 3800 && sum < 1200) input = right; 
    else if (position > 3200 && position < 3800 && sum > 1200 && sum < 4800) input = tilted;
    else if (position > 3200 && position < 3800 && sum > 4800) input = onLine;
    else if (position < 3200 && sum > 1200) input = leftTurn; 
    else if (position > 3800 && sum > 1200) input = rightTurn; 
    if (input == aligned) Serial.print("aligned");
    else if (input == left)  Serial.print("moveLeft");
    else if (input == right) Serial.print("moveRight");
    else if (input == leftTurn) Serial.print("turnLeft");
    else if (input == rightTurn)  Serial.print("turnRight");  
    else if (input == tilted)  Serial.print("tilted");
    else if (input == onLine)  Serial.print("Online");
  }
  else if (line == 2)*/



#endif
