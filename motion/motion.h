#ifndef MOTION_H
#define MOTION_H

#define B1 B00100000          //left-front backward
#define B2 B00000000          //right-front backward
#define B3 B00000010          //left-back backward
#define B4 B00000000          //right-back backward
#define F1 B00000000          //left-front forward
#define F2 B10000000          //right-front forward
#define F3 B00000000          //left-back forward
#define F4 B00001000          //right-back forward
#define M1 B00010000          //left-front motor
#define M2 B01000000          //right-front motor
#define M3 B00000001          //left-back motor
#define M4 B00000100          //right-back motor
#define faceFront F1|F2|F3|F4
#define faceRear B1|B2|B3|B4
#define faceCounterWise B1|F2|B3|F4
#define faceClockWise F1|B2|F3|B4
#define faceLeft B1|F2|F3|B4 
#define faceRight F1|B2|B3|F4
#define allWheels M1|M2|M3|M4
#define leftWheels M1|M3
#define rightWheels M2|M4
#define frontWheels M1|M2
#define rearWheels M3|M4
#define rightDiagonal M2|M3
#define leftDiagonal M1|M4

#define STEPS_PER_INCH 215.6
#define STEPS_PER_DEGREE 23.5
#define INCH_PER_DEGREE (float)STEPS_PER_DEGREE/STEPS_PER_INCH
#define ANGLE_PER_RATIO 45.0

#define constDrive(dist, del)         ((dist) > 0)  ?  mov(faceFront, (dist) * STEPS_PER_INCH, (del))  :  mov(faceRear, -1 * (dist) * STEPS_PER_INCH, (del));
#define constRotate(degree, del)      ((degree) > 0)  ?  mov(faceClockWise, (degree) * STEPS_PER_DEGREE, (del))  :  mov(faceCounterWise, -1 * (degree) * STEPS_PER_DEGREE, (del));
#define constStrafe(dist, del)        ((dist) > 0)  ?  mov(faceRight, (dist) * STEPS_PER_INCH, (del))  :  mov(faceLeft, -1 * (dist) * STEPS_PER_INCH, (del));
#define accelDrive(dist, del, n)      ((dist) > 0)  ?  accelMov(faceFront, (dist) * STEPS_PER_INCH, n, (del))  :  accelMov(faceRear, -1 * (dist) * STEPS_PER_INCH, n, (del));
#define accelRotate(degree, del, n)   ((degree) > 0)  ?  accelMov(faceClockWise, (degree) * STEPS_PER_DEGREE, n, (del))  :  accelMov(faceCounterWise, -1 * (degree) * STEPS_PER_DEGREE, n, (del));
#define accelStrafe(dist, del, n)     ((dist) > 0)  ?  accelMov(faceRight, (dist) * STEPS_PER_INCH, n, (del))  :  accelMov(faceLeft, -1 * (dist) * STEPS_PER_INCH, n, (del));
#define drive(dist)                   ((dist) > 0)  ?  accelMov(faceFront, (dist) * STEPS_PER_INCH)  :  accelMov(faceRear, -1 * (dist) * STEPS_PER_INCH);
#define rotate(degree)                ((degree) > 0)  ?  accelMov(faceClockWise, (degree) * STEPS_PER_DEGREE)  :  accelMov(faceCounterWise, -1 * (degree) * STEPS_PER_DEGREE);
#define strafe(dist)                  ((dist) > 0)  ?  accelMov(faceRight, (dist) * STEPS_PER_INCH)  :  accelMov(faceLeft, -1 * (dist) * STEPS_PER_INCH);
#define drive2(dist)                  ((dist) > 0)  ?  accelMov2(faceFront, (dist) * STEPS_PER_INCH)  :  accelMov2(faceRear, -1 * (dist) * STEPS_PER_INCH);
#define drivep(dist)                  ((dist) > 0)  ?  accelMovp(faceFront, (dist) * STEPS_PER_INCH)  :  accelMovp(faceRear, -1 * (dist) * STEPS_PER_INCH);
#define drivep2(dist)                 ((dist) > 0)  ?  accelMovp2(faceFront, (dist) * STEPS_PER_INCH)  :  accelMovp2(faceRear, -1 * (dist) * STEPS_PER_INCH);


void mov(byte dir, unsigned long steps, unsigned int del){
    PORTL = (PORTL & allWheels) | dir;
    for (unsigned long i = 0;  i < steps;  i++){
      PORTL ^= allWheels;
      delayMicroseconds(del);
    }
}

void steer(byte dir, unsigned long steps, unsigned int del, float ratio, byte master = leftWheels, byte slave = rightWheels){ 
    PORTL = (PORTL & allWheels) | dir;
    unsigned int slavecount = 0, mastercount = 0, stepcount = 0;
    unsigned int slavedelay, masterdelay;
    if (ratio >= 1) 
    {
        masterdelay = del;
        slavedelay = del * ratio;
    }
    else
    {
        slavedelay = del;
        masterdelay = del / ratio; 
        steps *= ratio;
    } 
    //about 1 microsecond for loop
    while(stepcount < steps){
        if(mastercount > masterdelay){
            PORTL ^= master;
            mastercount = 0;
            stepcount++;
        }
        if(slavecount > slavedelay){
            PORTL ^= slave;
            slavecount = 0;
        }
        mastercount++;
        slavecount++;
    }
}

void turn (float arclen, unsigned int angle, unsigned int del){
    //wheel to wheel edge length = 8. 
    //turningDistPerDegree = 2*distPerDegree
    
    //arclength(outerwheels Dist) = degree * turningDistPerDegree * ratio/(ratio-1)
    //arclength(actual) = C0 * arclength(outerwheels Dist)
    
    //arclength(theoretical) = radian * lengthOfBot * ratio/(ratio-1)
    //radius(theoretical) = lengthOfBot * ratio/(ratio-1)
    //radius(actual) = 

    if (arclen > 0 && angle > 0){
        unsigned long arcSteps = arclen * STEPS_PER_INCH;
        float ratio = arcSteps / (arcSteps - (angle*2*STEPS_PER_DEGREE));
        steer(faceFront, arcSteps, del, ratio);
    }
    else if (arclen < 0 && angle > 0){
        unsigned long arcSteps = -1 * arclen * STEPS_PER_INCH;
        float ratio = arcSteps / (arcSteps - (angle*2*STEPS_PER_DEGREE));
        steer(faceRear, arcSteps, del, ratio);
    } 
    else if (arclen > 0 && angle < 0){
        unsigned long arcSteps = arclen * STEPS_PER_INCH;
        float ratio = (arcSteps - (angle*2*STEPS_PER_DEGREE)) / arcSteps;
        steer(faceFront, arcSteps, del, ratio);
    }
    else{
        unsigned long arcSteps = -1 * arclen * STEPS_PER_INCH;
        float ratio = (arcSteps - (angle*2*STEPS_PER_DEGREE)) / arcSteps;
        steer(faceRear, arcSteps, del, ratio);
    }
    
}

void accelMov1 (byte dir, unsigned long steps, unsigned int n=25*2, unsigned int startPeriod=1000*2, unsigned int startSteps=64/2){
    
    //unit-step up the speed, cruise at top speed, then step-down in twice the time
    unsigned long totalSteps = n*(n+1)*3*startSteps/2;    
    unsigned long midSteps = steps - totalSteps;

    //if desired dist is no enough to evenly distribute the steps, calculate n that would (with max steps)
    if (midSteps > steps){
        //2*steps/(3*startSteps) = n(n+1)
        //quadratic formula, n^2 + n + c = 0,   c = -2*steps/(3*startSteps)
        n = (sqrt(1+(4*2*steps/(3*startSteps))) - 1) / 2;
        totalSteps = n*(n+1)*3*startSteps/2;
        midSteps = steps - totalSteps;
    }

    //discretized consecutive mov
    PORTL = (PORTL & allWheels) | dir;
    for (unsigned int i = 1;  i <= n;  i++){    
        for (unsigned int j = 0;  j < i*startSteps;  j++){
            PORTL ^= allWheels;
            delayMicroseconds(startPeriod/i);
        }
    }
    for (unsigned long j = 0;  j < midSteps;  j++){
        PORTL ^= allWheels;
        delayMicroseconds(startPeriod/n);
    }
    for (unsigned int i = n; i > 0; i--){
        for (unsigned int j = 0;  j < 2*i*startSteps;  j++){
            PORTL ^= allWheels;
            delayMicroseconds(startPeriod/i);
        }
        //mov(dir, 2*i*startSteps, startPeriod/i);  //for some reason this works better
                                                    //then using forloop
    }
    
}


void accelMovp (byte dir, unsigned long steps, unsigned char power = 2, unsigned int n=10*4, unsigned long startPeriod=(long)4000*16, unsigned char startSteps=1){
    //upside down n^p speed curve for accel/decel
    //curve(i) = n^p - (n-i)^p, for i from 0 to n
    //accelTime = n*startPeriod*startSteps
    //finalPeriod = startPeriod/pow(n, power)
    //slope = 1/(startPeriod*startSteps)
    //continuity = startperiod / startsteps
    //higher power, more discretized, for same finalPeriod and accelTime
    //n = rt(n, power), startSteps *= pow(n, power-1)
    //Serial.print("----------running accelMovp-----------"); //Serial.print("\n");
    //Serial.print("input stage: "); //Serial.print(n);//Serial.print("\t");
    //Serial.print("steps: "); //Serial.print(steps);//Serial.print("\t");
    
    unsigned long totalSteps = 0;
    for (unsigned int i = 1; i <= n; i++){
        totalSteps += 3 * startSteps * (pow(n, power) - pow(n-i, power));
        if(totalSteps > steps){                   // find a lower n value if not enough distance for acceleration
                                                  // to the top speed
            n = n - ((n-i+1)/power);              // better approx than n--
            i = 0;
            totalSteps = 0;
        }
    }
    unsigned long midSteps = steps - totalSteps;

    //Serial.print("midSteps: "); //Serial.print(midSteps);//Serial.print("\t");
    //Serial.print("totalSteps: "); //Serial.print(totalSteps);//Serial.print("\t");
    //Serial.print("actual stage: "); //Serial.print(n);//Serial.println("\t");
    

    //memory array method
    unsigned long period[2*n+1], Steps[2*n+1];
    for (unsigned int i = 1;  i <= n;  i++){    
            period[i-1] = startPeriod/(pow(n, power) - pow(n-i, power));
            Steps[i-1] = startSteps*(pow(n, power) - pow(n-i, power));
    }
    Steps[n] = midSteps;
    period[n] = startPeriod/pow(n, power);
    for (unsigned int i = n; i > 0; i--){
        period[n+1+(n-i)] = startPeriod/(pow(n, power) - pow(n-i, power));
        Steps[n+1+(n-i)] = 2*startSteps*(pow(n, power) - pow(n-i, power));
    }

    PORTL = (PORTL & allWheels) | dir;
    unsigned long stepCnt = 0;
    unsigned int j;
    unsigned int i;
    unsigned long timeout = micros();
    unsigned long startTime = timeout;
    unsigned long stageTime = timeout;
    for (i = 0; i < 2*n+1; i++){
        for (j = 0;  j < Steps[i];  j++){
              timeout += period[i];
              PORTL ^= allWheels;
              stepCnt++;
              while(micros() < timeout);
        } 
        //Serial.print("stage: "); //Serial.print(i+1);//Serial.print("\t");
        //Serial.print("steps: "); //Serial.print(j);//Serial.print("\t");
        //Serial.print("period: "); //Serial.print(period[i]);//Serial.print("\t");
        //Serial.print("dist: "); //Serial.print((float)j/STEPS_PER_INCH);//Serial.print("\t");
        //Serial.print("time: "); //Serial.print((float)(timeout - stageTime)/1000);//Serial.print("\t");
        //Serial.print("\n");
        stageTime = timeout;
    }
    //Serial.print("stepCnt: "); //Serial.print(stepCnt);//Serial.print("\t");
    //Serial.print("distCnt: "); //Serial.print((float)stepCnt/STEPS_PER_INCH);//Serial.print("inches");//Serial.print("\t");
    //Serial.print("timeCnt: "); //Serial.print((float)(timeout - startTime)/1000);//Serial.print("ms");//Serial.print("\t");
    //Serial.print("avgPer: "); //Serial.print((float)(timeout - startTime)/stepCnt);//Serial.print("\t");
    //Serial.print("\n");
    //Serial.print("----------end of accelMovp-----------"); //Serial.print("\n");
}

void accelMov (byte dir, unsigned long steps, unsigned char power = 2, unsigned int n=10*4, unsigned long startPeriod=(long)4000*16, unsigned char startSteps=1){
    // truncated n^p speed curve, if not enough steps are needed for desired top speed
    // top speed = n^p - (n-n_actual)^p


    //Serial.print("----------running accelMovp2-----------"); //Serial.print("\n");
    //Serial.print("input stage: "); //Serial.print(n);//Serial.print("\t");
    //Serial.print("steps: "); //Serial.print(steps);//Serial.print("\t");

    unsigned long totalSteps = 0;
    unsigned int n_actual = n;
    for (unsigned int i = 1; i <= n; i++){
        totalSteps += 3 * startSteps * (pow(n, power) - pow(n-i, power));
        
        // if not enough distance for acceleration
        // then cut the original curve short
        if(totalSteps > steps){
            n_actual = i-1;
            totalSteps -= 3 * startSteps * (pow(n, power) - pow(n-i, power));
            break;
        }
    }
    unsigned long midSteps = steps - totalSteps;

    //Serial.print("midSteps: "); //Serial.print(midSteps);//Serial.print("\t");
    //Serial.print("totalSteps: "); //Serial.print(totalSteps);//Serial.print("\t");
    //Serial.print("actual stage: "); //Serial.print(n_actual);//Serial.println("\t");
    
    //memory array method
    unsigned long period[2*n_actual+1], Steps[2*n_actual+1];
    for (unsigned int i = 1;  i <= n_actual;  i++){    
            period[i-1] = startPeriod/(pow(n, power) - pow(n-i, power));
            Steps[i-1] = startSteps*(pow(n, power) - pow(n-i, power));
    }
    Steps[n_actual] = midSteps;
    period[n_actual] = startPeriod/(pow(n, power) - pow(n-n_actual-1, power));
    for (unsigned int i = n_actual; i > 0; i--){
        period[n_actual+1+(n_actual-i)] = startPeriod/(pow(n, power) - pow(n-i, power));
        Steps[n_actual+1+(n_actual-i)] = 2*startSteps*(pow(n, power) - pow(n-i, power));
    }

    PORTL = (PORTL & allWheels) | dir;
    unsigned long stepCnt = 0;
    unsigned int j;
    unsigned int i;
    unsigned long timeout = micros();
    unsigned long startTime = timeout;
    unsigned long stageTime = timeout;
    for (i = 0; i < 2*n_actual+1; i++){
        for (j = 0;  j < Steps[i];  j++){
              timeout += period[i];
              PORTL ^= allWheels;
              stepCnt++;
              while(micros() < timeout);
        } 

        //Serial.print("stage: "); //Serial.print(i+1);//Serial.print("\t");
        //Serial.print("steps: "); //Serial.print(j);//Serial.print("\t");
        //Serial.print("period: "); //Serial.print(period[i]);//Serial.print("\t");
        //Serial.print("dist: "); //Serial.print((float)j/STEPS_PER_INCH);//Serial.print("\t");
        //Serial.print("time: "); //Serial.print((float)(timeout - stageTime)/1000);//Serial.print("\t");
        //Serial.print("\n");
        stageTime = timeout;
        
    }
    //Serial.print("stepCnt: "); //Serial.print(stepCnt);//Serial.print("\t");
    //Serial.print("distCnt: "); //Serial.print((float)stepCnt/STEPS_PER_INCH);//Serial.print("inches");//Serial.print("\t");
    //Serial.print("timeCnt: "); //Serial.print((float)(timeout - startTime)/1000);//Serial.print("ms");//Serial.print("\t");
    //Serial.print("avgPer: "); //Serial.print((float)(timeout - startTime)/stepCnt);//Serial.print("\t");
    //Serial.print("\n");
    //Serial.print("----------end of accelMovp2-----------"); //Serial.print("\n");
    
}


void testWheels(){
    PORTL = faceFront;
    for(long i=0;i<5000;i++){
      PORTL ^= M1;
      delayMicroseconds(400);
    }
    for(long i=0;i<5000;i++){
      PORTL ^= M2;
      delayMicroseconds(400);
    }
    for(long i=0;i<5000;i++){
      PORTL ^= M3;
      delayMicroseconds(400);
    }
    for(long i=0;i<5000;i++){
      PORTL ^= M4;
      delayMicroseconds(400);
    }
}

//recycle bin

/*

void accelMov2 (byte dir, unsigned long steps, unsigned int n=5*8, unsigned int startPeriod=1000*64, unsigned int startSteps=64/64){
    //upside down parabolic speed curve for accel/decel
    //curve(i) = n^2 - (n-i)^2, for i from 0 to n
    //curve(i) = 2*i*n - i^2
  
    //Serial.print("steps: "); //Serial.print(steps);//Serial.print("\t");
    //Serial.print("input stage: "); //Serial.print(n);//Serial.print("\t");

    unsigned long totalSteps = 0;
    for (unsigned int i = 1; i <= n; i++){
        totalSteps += 3 * startSteps * (2*i*n - (i*i));
        if(totalSteps > steps){
            n--;
            i = 0;
            totalSteps = 0;
        }
    }
    unsigned long midSteps = steps - totalSteps;

    //Serial.print("midSteps: "); //Serial.print(midSteps);//Serial.print("\t");
    //Serial.print("totalSteps: "); //Serial.print(totalSteps);//Serial.print("\t");
    //Serial.print("actual stage: "); //Serial.print(n);//Serial.println("\t");
    
    unsigned long stepCnt = 0;
    unsigned int i;
    unsigned long j;
    PORTL = (PORTL & allWheels) | dir;
    unsigned long timeout = micros();
    for (i = 1;  i <= n;  i++){   
        for (j = 0;  j < (2*i*n - (i*i));  j++){
            timeout += startPeriod/(2*i*n - (i*i));
            PORTL ^= allWheels;
            stepCnt++;
            while(micros() < timeout);
        }
        //Serial.print("stage: "); //Serial.print(i);//Serial.print("\t");
        //Serial.print("steps: "); //Serial.print(j);//Serial.print("\t");
        //Serial.print("period: "); //Serial.print(startPeriod/(2*i*n - (i*i)));//Serial.println("\t");
    
    }
    for (j = 0;  j < midSteps;  j++){
        timeout += startPeriod/(n*n);
        PORTL ^= allWheels;
        stepCnt++;
        while(micros() < timeout);
    }
    //Serial.print("stage: "); //Serial.print(i+1);//Serial.print("\t");
    //Serial.print("steps: "); //Serial.print(j);//Serial.print("\t");
    //Serial.print("period: "); //Serial.print(startPeriod/(n*n));//Serial.println("\t");

    for (i = n; i > 0; i--){
        //mov(dir, 2*startSteps*(2*i*n - (i*i)), startPeriod/(2*i*n - (i*i)));
        for (j = 0;  j < 2*startSteps*(2*i*n - (i*i));  j++){
            timeout += startPeriod/(2*i*n - (i*i));
            PORTL ^= allWheels;
            stepCnt++;
            while(micros() < timeout);
        }
        //Serial.print("stage: "); //Serial.print((n-i)+n+1);//Serial.print("\t");
        //Serial.print("steps: "); //Serial.print(j);//Serial.print("\t");
        //Serial.print("period: "); //Serial.print(startPeriod/(2*i*n - (i*i)));//Serial.println("\t");
        
    }
    //Serial.print("stepCnt: "); //Serial.print(stepCnt);//Serial.println("\t");
    
}


void accelMovExp (byte dir, unsigned long steps, unsigned int n=20, unsigned int finalPeriod=40, unsigned int finalSteps = 1280){
    //exp up and down to the asymptotic top speed
    //curve(i) = (1-exp(-i))
    unsigned int startSteps = finalSteps*(1-exp(-5/n));
    unsigned int startPeriod = finalPeriod*(1-exp(-5/n));
    unsigned long midSteps = steps;
    for (unsigned int i = 1; i <= n; i++){
        if (midSteps - 2*startSteps*(1-exp(-i*5/n)) >= steps){
            midSteps -= 2*startSteps*(1-exp(-i*5/n));
        }
        else{
            n = --i;
            break;
        }
    }

    //create period and n arrays
    unsigned int period[2*n+2], Steps[2*n+2];
    for (unsigned int i = 1;  i <= n;  i++){    
            period[i] = startPeriod/(1-exp(-i*5/n));
            Steps[i] = startSteps*(1-exp(-i*5/n)));
    }
    Steps[n+1] = midSteps;
    period[n+1] = startPeriod/(1-exp(-5));
    for (unsigned int i = n; i > 0; i--){
        period[n+1+i] = startPeriod/(1-exp(-i*5/n));
        Steps[n+1+i] = startSteps*(1-exp(-i*5/n));
    }
    PORTL = (PORTL & allWheels) | dir;
    for (unsigned long i = 1; i < 2*n+2; i++){
        mov(dir, Steps[i], period[i]);
        //Serial.print("Steps[");//Serial.print(i);//Serial.print("]:");//Serial.print(Steps[i]);//Serial.print("    ");
        //Serial.print("period[");//Serial.print(i);//Serial.print("]:");//Serial.print(period[i]);//Serial.println("    ");
        for (unsigned int j = 0;  j < Steps[i];  j++){
              PORTL ^= allWheels;
              delayMicroseconds(period[i]);
        }
    }


    PORTL = (PORTL & allWheels) | dir;
    for (unsigned int i = 1;  i <= n;  i++){    
        for (unsigned int j = 0;  j < startSteps*(1-exp(-i*5/n));  j++){
            PORTL ^= allWheels;
            delayMicroseconds(startPeriod/(1-exp(-i*5/n)));
        }
    }
    for (unsigned long j = 0;  j < midSteps;  j++){
        PORTL ^= allWheels;
        delayMicroseconds(startPeriod/(1-exp(-5)));
    }
    for (unsigned int i = n; i > 0; i--){
        mov(dir, (1-exp(-i*5/n))*startSteps, startPeriod/(1-exp(-i*5/n)));
    } 
}

#define Ra 0.4            //accel ratio for linearMov()
#define Rd 0.4            //decel ratio for linearMov()
void linearMov (byte dir, float totalDist, unsigned int aveDel){
    PORTL = (PORTL & allWheels) | dir;
    float totalSteps = totalDist * STEPS_PER_INCH;
    float accelStepf = totalSteps * Ra;
    float decelStepf = totalSteps * Rd;
    float cruiseStepf = totalSteps - accelStepf - decelStepf;
    float totalTimef = aveDel * totalSteps;
    unsigned int minDel = aveDel / (1 + Ra + Rd);
    float accelTimef = 2 * minDel * accelStepf;
    float decelTimef = 2 * minDel * decelStepf;
    unsigned long accelConst = accelTimef / sqrt(accelStepf);
    unsigned long decelConst = decelTimef / sqrt(decelStepf);
    float cruiseTimef = totalTimef - accelTimef - decelTimef;


    for (long i=1; i<=accelStepf; i++){
      PORTL ^= allWheels;
      delayMicroseconds((unsigned int)accelConst / sqrt(4 * i));
      ////Serial.print("accel:");//Serial.println(accelConst / sqrt(4 * i));
    }
    for (long i=0; i<cruiseStepf; i++){
      PORTL ^= allWheels;
      delayMicroseconds(minDel);
      //delayMicroseconds((unsigned int)accelConst / sqrt(4 * accelStepf));
      ////Serial.print("stable:");//Serial.println(minDel);
    }
    for (long i=decelStepf; i>0; i--){
      PORTL ^= allWheels;
      delayMicroseconds((unsigned int)decelConst / sqrt(4 * i));
      ////Serial.print("decel:");//Serial.println(decelConst / sqrt(4 * i));
    }
    unsigned int accelPeriod[3000]; //(unsigned int*)malloc(sizeof(unsigned int)*(long)totalSteps);
    unsigned int decelPeriod[3000];
        for (unsigned int i=1; i<=(unsigned int)accelStepf; i++){
                accelPeriod[i] = (unsigned int)(accelConst / sqrt(4 * i));
                ////Serial.print("accel-period[");//Serial.print(i);//Serial.print("]:");//Serial.println(period[i]);
        }
        for (unsigned int i=1; i<=(unsigned int)cruiseStepf; i++){
                period[i+(unsigned int)accelStepf] = minDel;
                ////Serial.print("stable-period[");//Serial.print(i+(unsigned int)accelStepf);//Serial.print("]:");//Serial.println(period[i]);
        }
        for (unsigned int i=decelStepf+1; i>0; i--){
                decelPeriod[i] = (unsigned int)(decelConst / sqrt(4 * i));
                ////Serial.print("decel-period[");//Serial.print((unsigned int)totalSteps - i);//Serial.print("]:");//Serial.println(period[i]);
        }        
        for (unsigned int i=1; i<(unsigned int)accelStepf; i++){
           PORTL ^= allWheels;
           delayMicroseconds(accelPeriod[i]);
           //Serial.print("accelPeriod[");//Serial.print(i);//Serial.print("]:");//Serial.println(accelPeriod[i]);
        }
        for (unsigned int i=1; i<=(unsigned int)cruiseStepf; i++){
            PORTL ^= allWheels;
            delayMicroseconds(minDel);
        }
        for (unsigned int i=decelStepf+1; i>0; i--){
           PORTL ^= allWheels;
           delayMicroseconds(decelPeriod[i]);
        }

        
    //else //Serial.println("Allocation failure!");
    
}

void fastMov(byte dir, float totalDist, unsigned int aveDel){
  PORTL = (PORTL & allWheels) | dir;
  unsigned int minDel = aveDel * (1 - (Ra/2) - (Rd/2));
  ////Serial.println (minDel);
  float totalSteps = totalDist * STEPS_PER_INCH;
  float accelStepf = totalSteps * Ra / (2 - 2*Ra -Rd);
  float decelStepf = totalSteps * Rd / (2 - 2*Rd -Ra);
  float cruisingStepf = totalSteps - accelStepf - decelStepf;

  long accelSteps = accelStepf;
  long decelSteps = decelStepf;
  long cruisingSteps = cruisingStepf;
  
  float accelTimef = totalSteps * aveDel * Ra;
  float decelTimef = totalSteps * aveDel * Rd;
  
  unsigned int accelInterval = 2 * accelTimef / (accelSteps+1);
  unsigned int decelInterval = 2 * decelTimef / (decelSteps+1);

  for (long i=accelSteps; i>0; i--){
    PORTL ^= allWheels;
    delayMicroseconds(minDel+(i*accelInterval/accelSteps));
  }
  for (long i=0; i<cruisingSteps; i++){
    PORTL ^= allWheels;
    delayMicroseconds(minDel);
  }
  for (long i=1; i<=decelSteps; i++){
    PORTL ^= allWheels;
    delayMicroseconds(minDel+(i*decelInterval/decelSteps));
  }
}

void linearMov(byte dir, float totalDist, unsigned int aveDel){
  PORTL = (PORTL & allWheels) | dir;
  unsigned int minDel = aveDel * (1 - (Ra/2) - (Rd/2));
  ////Serial.println (minDel);
  float totalSteps = totalDist * STEPS_PER_INCH;
  float accelStepf = totalSteps * Ra / (2 - 2*Ra -Rd);
  float decelStepf = totalSteps * Rd / (2 - 2*Rd -Ra);
  float cruisingStepf = totalSteps - accelStepf - decelStepf;

  long accelSteps = accelStepf;
  long decelSteps = decelStepf;
  long cruisingSteps = cruisingStepf;
  //Serial.print(accelSteps); //Serial.print("\t");
  float accelTimef = totalSteps * aveDel * Ra;
  float decelTimef = totalSteps * aveDel * Rd;
  //Serial.print((int)accelTimef); //Serial.print("\t");
  float accelConst = minDel*(accelSteps+1);
  float decelConst = minDel*(decelSteps+1);
  
  long accelBlockPeriod = accelTimef / ((long)log(accelSteps+1));
  long decelBlockPeriod = decelTimef / log(decelSteps+1);
  //Serial.print(log(accelSteps+1)); //Serial.print("\t");
  //Serial.print((int)accelTimef); //Serial.print("\t");
  //Serial.print(accelBlockPeriod); //Serial.print("\t");
  for (long i=1; i<=accelSteps; i++){
    PORTL ^= allWheels;
    ////Serial.println(minDel+(accelBlockPeriod/i));
    delayMicroseconds(minDel+(accelBlockPeriod/i));
  }
  for (long i=0; i<cruisingSteps; i++){
    PORTL ^= allWheels;
    delayMicroseconds(minDel);
  }
  for (long i=decelSteps; i>0; i--){
    PORTL ^= allWheels;
    delayMicroseconds(minDel+(decelBlockPeriod/i));
  }
}

*/

#endif
