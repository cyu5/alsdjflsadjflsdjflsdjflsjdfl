#ifndef LINE_SENSOR_H
#define LINE_SENSOR_H

#define NUM_SENSORS   8          
#define CALOUT 10

byte position;                
byte lineCnt = 0;                       
byte steerTable[256];

void setupLine(){
    DDRA = 0xFF;          // make sensor line an output
    PORTA = 0xFF;         // drive sensor line high

    delayMicroseconds(10);              // charge lines for 10 us
    
    DDRA = 0;             // make sensor line an input
    PORTA = 0;            // important: disable internal pull-up!
    
}

void readLineSensor(){

    byte value = PINA;
    position = CALOUT*(NUM_SENSORS-1)/2;
    if (!value) return;

    //calculate position
    bool on_line = false;
    unsigned char i = 0;
    lineCnt = 0;
    byte avg = 0;                              // this is for the weighted total
    byte sum = 0;                             // this is for the denominator

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
        value = value >> 1;
        i++;
    }

    if(lineCnt) position = CALOUT*avg/sum;

}

void printLineSensor(){

    for (char i=0; i<NUM_SENSORS; i++){
        Serial.print((bool)(PINA&(1<<i)));
    }

    Serial.print('\t');
    
    Serial.print("P");Serial.print(": ");
    Serial.print(position);
    Serial.print('\t');
    
    Serial.print('\n');
}


#endif
