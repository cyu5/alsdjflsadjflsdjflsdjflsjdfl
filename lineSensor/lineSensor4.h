#ifndef LINE_SENSOR_H
#define LINE_SENSOR_H

#define NUM_SENSORS 8
#define TIMEOUT 1000

float position[256];               
int steerTable[256];

void setupLine(){
    DDRA = 0xFF;          // make sensor line an output
    PORTA = 0xFF;         // drive sensor line high

    delayMicroseconds(10);              // charge lines for 10 us
    
    DDRA = 0;             // make sensor line an input
    PORTA = 0;            // important: disable internal pull-up!

    DDRC = 0xFF;          // make sensor line an output
    PORTC = 0xFF;         // drive sensor line high

    delayMicroseconds(10);              // charge lines for 10 us
    
    DDRC = 0;             // make sensor line an input
    PORTC = 0;            // important: disable internal pull-up!

    steerTable[0] = 100;
    for(byte sensorValue=1; sensorValue!=0; sensorValue++)
    {
        byte i = 0;                          
        byte avg = 0;
        byte sum = 0;
/*
        for (char i=0; i<NUM_SENSORS; i++){
        Serial.print((bool)(sensorValue&(1<<i)));
        }
*/        
        byte value = sensorValue;
        while(value)
        {
            if (value & 1) 
            {
                avg += i;
                sum++;
            }
            i++;
            value = value >> 1;
        }
            
        position[sensorValue] = (float)avg/sum;
        float ratio = exp(4 * ((float)position[sensorValue] / (NUM_SENSORS-1)) - 2 );
        if (ratio >= 1) steerTable[sensorValue] = 100*ratio;
        else steerTable[sensorValue] = -100/ratio;
/*    
        Serial.print('\t');
        
        Serial.print("P");Serial.print(": ");
        Serial.print(position);
        Serial.print('\t');
    
        Serial.print("R");Serial.print(": ");
        Serial.print(steerTable[sensorValue]);
        Serial.print('\t');
        
        Serial.print('\n');
*/        
    }
    
    
}



void printLineSensor(){

    byte sensorValue = PINA;
    for (char i=0; i<NUM_SENSORS; i++)
    {
        Serial.print((bool)(sensorValue&(1<<i)));
    }

    Serial.print('\t');
    
    Serial.print("P");Serial.print(": ");
    Serial.print(position[sensorValue]);
    Serial.print('\t');
    
    Serial.print('\n');
}


#endif
