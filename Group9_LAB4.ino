/*
Group9_Laboratory 4:  IR Remote Control, millis() and 
Passive IR Sensor

Coded by: Ariel M. Rosales
Created on: August 02,2022 
Last edit on:August 02,2022
---------------------------------------------------------
INSTRUCTIONS:
1. The system will automatically 'ON', LCD 1 will show the bulb
status and the distance measured by ultrasonic sensor.

2. If the PIR sensor detects any motion, light bulb will turn on
for a specific period of time before it turn off and will turn on
again if the pir sensor sends a signal.

3. Pressing "1" will make the servo motor to go to the RIGHT direction
and at the same time the BLUE LED will turn on for 1 seconds.

4.Pressing "2" will make the servo motor to go to the LEFT direction
and at the same time the RED LED will turn on for 1 seconds.

5. The LCD 2 will show all the activities made in instruction 3&4.

-----------------------------------------------------------
CONDITIONS:

This system will operate if and only if the power supply is provided.
Also, the turning "OFF" condition of this system is directly if the 
power is removed. Both arduino board and all the components will 
become de-energized.

Since, this system uses millis function, it allows the multi-tasking  in performing
different task at the same time such as the ultrasonic is continues in
taking distance measurements,the pir will still be able to sense motion 
while the servo motor is rotating.


------------------START OF PROGRAM--------------------------*/


//libraries and variables
#include <Wire.h>
#include <Servo.h>
int servoPin= 5;
Servo myServo;
int angle =90;    // initial angle  for servo
int angleStep =30;



#include <LiquidCrystal_I2C.h> //for lcd display setup
LiquidCrystal_I2C lcd1(32,16,2);
LiquidCrystal_I2C lcd2(33,16,2);
int rLED=2,bLED=4,relayPin=7;
int t=1000;

//for pir & ultrasonic
int pirPin=3,pirVal;
int trigPin=9,echoPin=10;
long duration;
int distance;


//millis variables
unsigned long fromStartms; //millis from start
unsigned long prevpirms; //millis previous read
unsigned long prevms;
unsigned long pir_interval=100;
unsigned long interval=50;


#include <IRremote.h>
int IRpin=6;
const boolean PCB = 0;
const String RIGHT="1";
const String LEFT="2";

IRrecv irrecv(IRpin);
unsigned  irRemote[] ={ 
             0xFD00FF, //power
             0xFD08F7, //1
             0xFD8877, //2
  
          0xFD48B7,//3
       0xFD28D7,//4
       0xFDA857,//5
  
       0xFD6897,//6
         0xFD18E7,//7
       0xFD9867,//8
  
       0xFD58A7,//9
       0xFD30CF //0
                     };


String IRremoteKey[]={
       "power",
       "1",
       "2",
       "3",
       "4",
       "5",
       "6",
       "7",
       "8",
       "9",
       "0"
        };


decode_results results;

void setup()
{
  pinMode(servoPin, OUTPUT); //for servo 
  myServo.attach(servoPin);
  
  pinMode(rLED, OUTPUT); //for led
  pinMode(bLED, OUTPUT);
  
  pinMode(relayPin, OUTPUT); //for relay 
  
  pinMode(trigPin,OUTPUT);//for ultrasonic & pir
  pinMode(echoPin,INPUT);
  pinMode(pirPin,INPUT);
  
  pinMode(IRpin, INPUT);//for ir sensor
  irrecv.enableIRIn();
  
  Serial.begin(9600); //for serial monitor and lcd display
  //lcd1
  lcd1.begin(16,2);
  lcd1.init();
  lcd1.backlight();
  //lcd2
  lcd2.begin(16,2);
  lcd2.init();
  lcd2.backlight();
}



void loop()
{     
  fromStartms=millis(); 
  
  
  if(fromStartms - prevms >interval)
  {  
     prevms = fromStartms;
     pirVal=digitalRead(pirPin);
   if(pirVal==1)
    {
      digitalWrite(relayPin,HIGH);
      lcd1.setCursor(0,1);
      lcd1.print("LIGHTS ON!");
      
    }
    else
    {
      digitalWrite(relayPin,LOW);
      lcd1.setCursor(0,1);
      lcd1.print("LIGHTS OFF!");
    }  
  }
  
  
  if(fromStartms - prevpirms >=pir_interval)
  {
    prevpirms = fromStartms;
    
      lcd1.setCursor(0,0);
      lcd1.print("Distance= ");
      lcd1.setCursor(10,0);
      lcd1.print(distance);
      digitalWrite(trigPin, LOW); 
      delayMicroseconds(2);
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);
      duration = pulseIn(echoPin, HIGH);
      distance= duration*0.034/2;  
      
  
 if (irrecv.decode(&results)) {
   ValidateCode(results.value);
    irrecv.resume(); // Receive the next value
  }
  }
  
}//loop





 void ValidateCode(int cd)
{
   int found=0;
      for(int i=0; i< sizeof(irRemote)/sizeof(int); i++)
      {
        if(irRemote[i] ==cd)
        {
          lcd2.clear();
          lcd2.setCursor(0,0);
          lcd2.print("Key pressed:");
          lcd2.setCursor(13,0);
          lcd2.print(IRremoteKey[i]);
          ServoAct(IRremoteKey[i]);// take action
          found=1;
        }// if matched
      }
  if(!found){
    if(cd !=0xFFFFFFFF)
      {
    Serial.println("Key unkown");
      }
  }// found
}// ValidateCode end
 
void ServoAct(String value)
 {
 
  while(value == RIGHT){

    if (angle > 0 && angle <= 180) {
      angle = angle - angleStep;
       if(angle < 0){
        angle = 0;
       }else{
       
      myServo.write(angle); // move the servo to desired angle
        
      lcd2.setCursor(0,1);   
      lcd2.print("Moved to: ");
      lcd2.setCursor(10,1);   
      lcd2.print(angle);   // print the angle
      lcd2.setCursor(13,1);    
      lcd2.print("deg");
      digitalWrite(bLED,HIGH);
      delay(t);
      digitalWrite(bLED,LOW);
       }
    }// if
    value =".";
    delay(100);
  }// while for RIGHT
  while(value == LEFT){

   
    if (angle >= 0 && angle <= 180) {
      angle = angle + angleStep;
      if(angle >180){
        angle =180;
       }else{
      myServo.write(angle); // move the servo to desired angle
        
      lcd2.setCursor(0,1);   
      lcd2.print("Moved to: ");
      lcd2.setCursor(10,1);   
      lcd2.print(angle);   // print the angle
      lcd2.setCursor(13,1);    
      lcd2.print("deg");
      digitalWrite(rLED,HIGH);
      delay(t);
      digitalWrite(rLED,LOW);
       }
    }
    value =".";
    delay(100);
  }// while for LEFT
 
  
}//ServoAct

//------------------------END OF PROGRAM-----------------------
//coded by: Ariel M. Rosales
