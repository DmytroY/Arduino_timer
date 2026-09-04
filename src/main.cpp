#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <RTClib.h>
#include "LargeLCD.h"

//#define LED_PIN 13
#define RELAY_PIN 5
#define BUT1_PIN A0
#define BUT2_PIN A1
#define BUT3_PIN A2

//LiquidCrystal lcd(12, 11, 10, 9, 8, 7);
LargeLCD largeLcd(12, 11, 10, 9, 8, 7);
RTC_DS3231 rtc;

char msg[17]; //  buffer for messsages
int lastMinute = -1;
int lastSecond = -1;

void setup() {
  Serial.begin(9600);
  largeLcd.begin();

  if (!rtc.begin()) {
    largeLcd.printString("RTC Error!");
    while (1);
  }

  // Set the RTC to the time the code was compiled if it lost power
  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  pinMode(BUT1_PIN, INPUT_PULLUP);
  pinMode(BUT2_PIN, INPUT_PULLUP);
  pinMode(BUT3_PIN, INPUT_PULLUP);
  digitalWrite(RELAY_PIN, LOW);
  pinMode(RELAY_PIN, OUTPUT);
  
  largeLcd.clear();
}

void loop() {
  DateTime now = rtc.now();

  // Update Hours and Minutes only when the minute changes
  if (now.minute() != lastMinute) {
    lastMinute = now.minute();
    
    // Print HH at col 0, fixed 2-digit length, padded with '0'
    largeLcd.bigNumber(now.hour(), 0, 2, true);
    
    // Print MM at col 9, fixed 2-digit length, padded with '0'
    largeLcd.bigNumber(now.minute(), 9, 2, true);
  }

  // Handle the blinking colon every second
  if (now.second() != lastSecond) {
    lastSecond = now.second();
    
    if (lastSecond % 2 == 0) {
      // Draw colon at col 6
      largeLcd.separator2(7);
      digitalWrite(RELAY_PIN, HIGH);
    } else {
      // Erase colon by printing 3 blank spaces over it on both rows
      largeLcd.printString("  ", 0, 7);
      largeLcd.printString("  ", 1, 7);
      digitalWrite(RELAY_PIN, LOW);
    }
  }

  // Small delay to prevent loop from running unnecessarily fast
  delay(100);
}