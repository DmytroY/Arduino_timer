#include <Arduino.h>
#include "LargeLCD.h"

//#define LED_PIN 13

//LiquidCrystal lcd(12, 11, 10, 9, 8, 7);
LargeLCD largeLcd(12, 11, 10, 9, 8, 7);

char msg[17]; //  buffer for messsages

void setup() {
  Serial.begin(9600);
  largeLcd.begin();
  largeLcd.clear();
}

void loop() {

  // for (int h = 1; h < 21; h+=3){
  //   for (int m = 1; m <= 21; m+=3) {
  //     largeLcd.bigNumber(h, 0, 2);
  //     largeLcd.separator2(7);
  //     largeLcd.bigNumber(m, 9, 2, true);
  //     delay(500);
  //     largeLcd.sp2(7);
  //     delay(500);
  //   }
  // }
  
  largeLcd.clear();
  delay(200);
  char msg[17];
  int hour = 12;
  int minute = 7;
  int sec = 34;

  largeLcd.printString("current time is");
    
  snprintf(msg, sizeof(msg), "%02d:%02d:%02d" , hour, minute, sec);
  largeLcd.printString(msg, 1, 4);

  delay(1000);
    
}