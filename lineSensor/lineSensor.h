#ifndef LINE_SENSOR_H
#define LINE_SENSOR_H

#define NUM_SENSORS 8          
#define TIMEOUT 300

byte position;                
byte lineCnt = 0;
byte sensorSum[256];
byte lineNum[256];
byte lineReading;
int steerTable[256];

void setupLine(){

    steerTable[0] = 100;
    for(byte sensorValue=1; sensorValue!=0; sensorValue++)
    {
        byte i = 0;                          
        byte avg = 0;
        byte sum = 0;
        byte value = sensorValue;
        byte lineCnt=0;
        bool on_line = false;
        while(value)
        {
            if (value & 1) 
            {
                avg += i;
                sum++;
                if (!on_line) lineCnt++;
                on_line = !on_line;
            }
            else on_line = false;
            i++;
            value = value >> 1;
        }
        sensorSum[sensorValue] = sum;
        lineNum[sensorValue] = lineCnt;
        float ratio = exp(3 * ((float)avg/sum / (NUM_SENSORS-1)) - 1.5 );
        if (ratio >= 1) steerTable[sensorValue] = 100*ratio;
        else steerTable[sensorValue] = -100/ratio;  
    }   
}

void readLineSensor(){

    DDRA = 0xFF;          // make sensor line an output
    PORTA = 0xFF;         // drive sensor line high

    delayMicroseconds(10);              // charge lines for 10 us
    
    DDRA = 0;             // make sensor line an input
    PORTA = 0;            // important: disable internal pull-up!

    delayMicroseconds(TIMEOUT);
    lineReading = PINA;
    lineCnt = lineNum[lineReading];
    

    
    /*
    unsigned long startTime = micros();
    
    while (micros() - startTime < TIMEOUT)
    {
        unsigned int time = micros() - startTime;
        for (i = 0; i < NUM_sENSORS; i++)
        {
            if (PINA & (0x01<<i)) 
            if (digitalRead(_pins[i]) == LOW && time < sensor_values[i])
                sensor_values[i] = time;
        }
    }
    */

}

void printLineSensor(){

    for (char i=0; i<NUM_SENSORS; i++){
        Serial.print((bool)(lineReading&(1<<i)));
    }

    Serial.print('\t');
    
    Serial.print("P");Serial.print(": ");
    Serial.print(position);
    Serial.print('\t');
    
    Serial.print('\n');
}


#endif
