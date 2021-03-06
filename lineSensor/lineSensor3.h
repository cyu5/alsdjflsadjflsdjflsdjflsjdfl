#ifndef LINE_SENSOR_H
#define LINE_SENSOR_H

#define NUM_SENSORS   8    
#define TIMEOUT       1500  
#define NUM_LINES     (NUM_SENSORS+1)/2      
#define CALOUT 10

byte position;
byte line[NUM_LINES];                      
byte lineCnt = 0;                       

void setupLine() {

    DDRA = 0xFF;          // make sensor line an output
    PORTA = 0xFF;         // drive sensor line high

    delayMicroseconds(10);              // charge lines for 10 us
    
    DDRA = 0;             // make sensor line an input
    PORTA = 0;            // important: disable internal pull-up!

}

void readLineSensor(){

    position = CALOUT*(NUM_SENSORS-1)/2;
    if (!PINA) return;

    //calculate position
    bool on_line = false;
    unsigned char i;
    lineCnt = 0;
    byte avg = 0;                              // this is for the weighted total
    byte sum = 0;                             // this is for the denominator

    for(i=0;i<NUM_SENSORS;i++) {
        if(PINA & (0x01<<i)) 
        {
            if (!on_line) line[lineCnt++] = CALOUT * i;
            else          line[lineCnt] = (line[lineCnt]+(i*CALOUT)) / 2;
            
            on_line = !on_line;
            avg += i;
            sum++;
        }
        else on_line = false;
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
    
    for (unsigned char i = 0; i < lineCnt; i++)
    {
      Serial.print("L");Serial.print(i);Serial.print(": ");
      Serial.print(line[i]);
      Serial.print('\t');
    }
    
    Serial.print('\n');
}


#endif
