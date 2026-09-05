#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <RTClib.h>
#include "LargeLCD.h"

//#define LED_PIN 13
#define RELAY_PIN 5
#define BUT0_PIN A0
#define BUT1_PIN A1
#define BUT2_PIN A2

//LiquidCrystal lcd(12, 11, 10, 9, 8, 7);
LargeLCD largeLcd(12, 11, 10, 9, 8, 7);
RTC_DS3231 rtc;

char msg[17]; //  buffer for messsages
int lastMinute = -1;
int lastSecond = -1;
int relayState = 0;
DateTime now;
byte keys = 0;

const char* dow[] = {
  "Sun",
  "Mon",
  "Tue",
  "Wed",
  "Thu",
  "Fri",
  "Sat",
};

const char* poa[] = {
  "day",
  " hh",
  " mm",
  " ss"
};

void displayTime(DateTime now) {
  // Print HH at col 0, fixed 2-digit length, padded with ' '
  largeLcd.bigNumber(now.hour(), 0, 2, false);
  // Print MM at col 9, fixed 2-digit length, padded with '0'
  largeLcd.bigNumber(now.minute(), 9, 2, true);
}

void blink() {
  if (lastSecond % 2 == 0) {
    largeLcd.separator2(7);
  } else {
    largeLcd.sp2(7);
  }
}

byte readKeys() {
  byte k = 0;
  k = !digitalRead(BUT0_PIN) + 2 * !digitalRead(BUT1_PIN) + 4 * !digitalRead(BUT2_PIN);
  if(!k) return 0;
  delay(20);
  while (!(digitalRead(BUT0_PIN) && digitalRead(BUT1_PIN) && digitalRead(BUT2_PIN))) {
    // wait for release buttons
  }
  delay(20);
  return k;
}

void toggleRelay() {
  if(relayState){
    digitalWrite(RELAY_PIN, 0);
    relayState = 0;
  } else {
    digitalWrite(RELAY_PIN, 1);
    relayState = 1;
  }
}

void setTime() {
  largeLcd.clear();
  largeLcd.printString("NEXT    -     + ", 1);
  int poaN = 0; // NN of parameter we will modify
  while(poaN < 4 ){
    now = rtc.now();
    int h = now.hour();
    int m = now.minute();
    int s = now.second();
    int dowN = now.dayOfTheWeek();
    Serial.println("day of week");
    Serial.print(dowN);
    
    char dow_cstr[4]; // c-style string for name of day
    char poa_cstr[4]; // c-style string for name of parameter we will modify (Point Of Attention)
    
    strncpy(dow_cstr, dow[dowN], sizeof(dow_cstr)); // get day name based on number
    dow_cstr[sizeof(dow_cstr)-1] = '\0'; // ensure null-termination

    strncpy(poa_cstr, poa[poaN], sizeof(poa_cstr)); // get day name based on number
    poa_cstr[sizeof(poa_cstr)-1] = '\0'; // ensure null-termination

    snprintf(msg, sizeof(msg), "%s %02d:%02d:%02d/%s", dow_cstr, h, m, s, poa_cstr);
    largeLcd.printString(msg, 0);

    keys = readKeys();
    if(keys == 0b0001) {
      poaN++; // next parameter
    }
    if(keys == 0b0010) {
      if(poaN == 0) rtc.adjust(now - TimeSpan(1, 0, 0, 0)); // minus day
      if(poaN == 1) rtc.adjust(now - TimeSpan(0, 1, 0, 0)); // minus hour
      if(poaN == 2) rtc.adjust(now - TimeSpan(0, 0, 1, 0)); // minus minute
      if(poaN == 3) rtc.adjust(now - TimeSpan(0, 0, 0, 1)); // minus sec
    }
    if(keys == 0b0100) {
      if(poaN == 0) rtc.adjust(now + TimeSpan(1, 0, 0, 0)); // plus day
      if(poaN == 1) rtc.adjust(now + TimeSpan(0, 1, 0, 0)); // plus hour
      if(poaN == 2) rtc.adjust(now + TimeSpan(0, 0, 1, 0)); // plus minute
      if(poaN == 3) rtc.adjust(now + TimeSpan(0, 0, 0, 1)); // plus sec
    }
  }
}

void setRelay() {
  largeLcd.clear();
  largeLcd.printString(" --setRelay()-- ", 1);
  delay(1000);
}

void menu() {
  largeLcd.clear();
  largeLcd.printString(" -- Settings -- ", 0);
  largeLcd.printString("BACK  TIME  RLAY", 1);
  unsigned long startTime = millis();
  while((millis() - startTime) < 10000){
    keys = readKeys();
    if(keys == 0b0001) return;
    if(keys == 0b0010) {
      setTime();
      return;
    }
    if(keys == 0b0100) {
      setRelay();
      return;
    }
  }
}

void setup() {
  Serial.begin(9600);
  largeLcd.begin();
  Wire.begin();
  Wire.setWireTimeout(30000, true); //30ms timeout prevent hanging if line interrupted.

  if (!rtc.begin()) {
    largeLcd.printString("RTC Error!");
    while (1);
  }

  // Set the RTC to the PC time the code was compiled if it lost power
  if (rtc.lostPower()) { rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));}

  pinMode(BUT0_PIN, INPUT_PULLUP);
  pinMode(BUT1_PIN, INPUT_PULLUP);
  pinMode(BUT2_PIN, INPUT_PULLUP);
  digitalWrite(RELAY_PIN, LOW);
  relayState = 0;
  pinMode(RELAY_PIN, OUTPUT);
  
  largeLcd.clear();
}

void loop() {
  now = rtc.now();

  // Update Hours and Minutes only when the minute changes
  if (now.minute() != lastMinute) {
    lastMinute = now.minute();
    displayTime(now);
  }

  // Handle the blinking colon every second
  if (now.second() != lastSecond) {
    lastSecond = now.second();
    blink();
  }

  keys = readKeys();
  if(!keys) return;
  if(keys == 0b0100) toggleRelay();
  if(keys == 0b0001) {
    menu();
    largeLcd.clear();
    displayTime(now);
  } 


}