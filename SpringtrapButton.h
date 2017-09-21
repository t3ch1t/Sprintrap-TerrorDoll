#include <LowPower.h>
#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>
#include "Arduino.h"

SoftwareSerial mySoftwareSerial(11, 10); // RX, TX to talk to DFPlayerMini
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);
volatile boolean scareNick = false; //keeps track of scare rountine run count
int eye1 = 8; //first eye
int eye2 = 9; //second eye
int bRedButton = 2; //the big red button for interrupt
long prevMillis1 = 0; //first millis tracker
long prevMillis2 = 0; //second millis tracker
long snoozeInt = 600000; //how long to sleep between eye blinks
long eyeFadeD = 300000; //how long to sleep between eye fade
int eyeBright = 0;
int fadeAmount = 5;
volatile int buttonState = HIGH;

void setup()
{
  pinMode(eye2, OUTPUT); //set pin 9 to ouput for eye2 led
  pinMode(eye1, OUTPUT); //set pin 8 to output for eye1 led
  pinMode(bRedButton, INPUT);
  digitalWrite(eye1, HIGH);
  digitalWrite(eye2, HIGH);
  attachInterrupt(0, bRBPress, LOW); //setup interrupt for big red button on pin 2
  mySoftwareSerial.begin(9600); 
  Serial.begin(115200);
  
  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true);
  }
  Serial.println(F("DFPlayer Mini online."));
}

void scareRoutine() {
      myDFPlayer.volume(20); //set audio volume 2/3
      digitalWrite(eye1, LOW); // turn eye1 on
      digitalWrite(eye2, LOW); // turn eye1 on
      delay(5000);
      myDFPlayer.play(2); //play glitch sound
      digitalWrite(eye1, HIGH); // turn eye1 off  
      delay(1000);
      digitalWrite(eye2, HIGH); // turn off eye2
      delay(3000);
      digitalWrite(eye1, LOW); // turn eye1 on
      digitalWrite(eye2, LOW); //turn eye2 on
      myDFPlayer.volume(30); //set audio volume max
      myDFPlayer.play(1); //play Springtrap scream
      delay(3000);
      digitalWrite(eye1, HIGH); //turn eye1 off
      digitalWrite(eye2, HIGH); //turn eye2 off
      scareNick = false; //increment scareNick by 1
      delay(8000);
}

void eyeBlink() {
    digitalWrite(eye1, LOW); // turn eye1 on
    digitalWrite(eye2, LOW); // turn eye1 on
    delay(100);
    digitalWrite(eye1, HIGH); //turn eye1 off
    digitalWrite(eye2, HIGH); //turn eye2 off
    delay(100);
    digitalWrite(eye1, LOW); // turn eye1 on
    digitalWrite(eye2, LOW); // turn eye1 on
    delay(100);
    digitalWrite(eye1, HIGH); //turn eye1 off
    digitalWrite(eye2, HIGH); //turn eye2 off
}

void sleepy() {
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
}

void bRBPress() {
  delay(100);
  int newButtonState = digitalRead(bRedButton);
  if(buttonState != newButtonState){
      scareNick = true;
  }
}

void eyeFade() {
  analogWrite(eye2, eyeBright); //set brightness of eye2 (pin 9)
  eyeBright = eyeBright + fadeAmount;
  if (eyeBright <= 0 || eyeBright >= 150){
    fadeAmount = -fadeAmount;
  }
  delay(30);
}

void loop()
{
  unsigned long nowMillis1 = millis(); //millis counter for eye blink
  unsigned long nowMillis2 = millis(); //millis counter for eye fade
  
  if(scareNick == true){ 
    scareRoutine(); //run scare routine once
  }
  if(scareNick == false && nowMillis2 - prevMillis2 < eyeFadeD) {
    prevMillis2 = nowMillis2; //remember the time
  } 
  else if(scareNick == false && nowMillis2 - prevMillis2 <eyeFadeD) {
    for(int i = 0; i <=60; i++) { //run the eyeFade function 60 times. 30 for ramp up, 30 for ramp down
      eyeFade(); 
    }
  }
  if(scareNick == false && nowMillis1 - prevMillis1 < snoozeInt){
    prevMillis1 = nowMillis1; //remember the time
    sleepy(); //go to sleep for 8 secs.
  } 
  else if(scareNick == false && nowMillis1 - prevMillis1 >= snoozeInt){
    prevMillis1 = nowMillis1; //remember the time
    eyeBlink(); //blink eyes
  }

}

