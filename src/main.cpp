#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <RTClib.h>
#include <EEPROM.h>
#include "LargeLCD.h"

//#define LED_PIN 13
#define RELAY_PIN 5
#define BUT0_PIN A0
#define BUT1_PIN A1
#define BUT2_PIN A2
#define RELAY_ON_EEPROM_ADDR 0
#define RELAY_OFF_EEPROM_ADDR 4

//LiquidCrystal lcd(12, 11, 10, 9, 8, 7);
LargeLCD largeLcd(12, 11, 10, 9, 8, 7);
RTC_DS3231 rtc;

struct ScheduleBitmask {
  uint8_t daysMask; // Bitmask: Bit 1 = Mon ... Bit 7 = Sun
  uint8_t hour;
  uint8_t minute;
};

char msg[17]; //  buffer for messsages
int lastMinute = -1;
int lastSecond = -1;
int relayState = 0;
DateTime now;
byte keys = 0;
ScheduleBitmask relayOnSchedule, relayOffSchedule, tempSchedule;

const char* dayOfWeek[] = {
  "Sun",
  "Mon",
  "Tue",
  "Wed",
  "Thu",
  "Fri",
  "Sat",
};

const char* paramClock[] = {
  "day",
  "hour",
  "minute",
  "secunde"
};

const char* paramRelay[] = {
  "on/off",
  "Mon",
  "Tue",
  "Wed",
  "Thu",
  "Fri",
  "Sat",
  "Sun",
  "hour",
  "minute"
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
  largeLcd.printString("        -     + ", 1);
  int paramClockN = 0; // NN of parameter we will modify
  while(paramClockN < 4 ){
    now = rtc.now();
    int h = now.hour();
    int m = now.minute();
    int s = now.second();
    int dayOfWeekN = now.dayOfTheWeek();
    
    char dayOfWeek_cstr[4]; // c-style string for name of day
    char paramClock_cstr[8]; // c-style string for name of parameter we will modify (Point Of Attention)
    
    strncpy(dayOfWeek_cstr, dayOfWeek[dayOfWeekN], sizeof(dayOfWeek_cstr)); // get day name based on number
    dayOfWeek_cstr[sizeof(dayOfWeek_cstr)-1] = '\0'; // ensure null-termination

    strncpy(paramClock_cstr, paramClock[paramClockN], sizeof(paramClock_cstr)); // get point of attention name based on number
    paramClock_cstr[sizeof(paramClock_cstr)-1] = '\0'; // ensure null-termination

    snprintf(msg, sizeof(msg), "%s    %02d:%02d:%02d", dayOfWeek_cstr, h, m, s);
    largeLcd.printString(msg, 0);
    largeLcd.printString(paramClock_cstr, 1);

    keys = readKeys();
    if(keys == 0b0001) {
      paramClockN++; // next parameter
    }
    if(keys == 0b0010) {
      if(paramClockN == 0) rtc.adjust(now - TimeSpan(1, 0, 0, 0)); // minus day
      if(paramClockN == 1) rtc.adjust(now - TimeSpan(0, 1, 0, 0)); // minus hour
      if(paramClockN == 2) rtc.adjust(now - TimeSpan(0, 0, 1, 0)); // minus minute
      if(paramClockN == 3) rtc.adjust(now - TimeSpan(0, 0, 0, 1)); // minus sec
    }
    if(keys == 0b0100) {
      if(paramClockN == 0) rtc.adjust(now + TimeSpan(1, 0, 0, 0)); // plus day
      if(paramClockN == 1) rtc.adjust(now + TimeSpan(0, 1, 0, 0)); // plus hour
      if(paramClockN == 2) rtc.adjust(now + TimeSpan(0, 0, 1, 0)); // plus minute
      if(paramClockN == 3) rtc.adjust(now + TimeSpan(0, 0, 0, 1)); // plus sec
    }
  }
}

void setRelay() {
  largeLcd.clear();
  largeLcd.printString("        -     + ", 1);

  // Set relay wsitch-ON day and time
  int paramRelayN = 1; // NN of parameter we will modify
  while(paramRelayN < 10 ){
    // assemble and display schedule: days of week, time
    char daysStr[8];
    for (uint8_t day = 1; day < 8; day++){
      if(relayOnSchedule.daysMask & (1 << day)){
        daysStr[day - 1] = '0' + day; // ASCII character from '1' to '7'
      } else {
        daysStr[day - 1] = '_';
      }
    }
    daysStr[7] = '\0';
    snprintf(msg, sizeof(msg), "ON %s %02d:%02d", daysStr, relayOnSchedule.hour, relayOnSchedule.minute);
    largeLcd.printString(msg);
    largeLcd.printString(paramRelay[paramRelayN],1);

    keys = readKeys();
    if(keys == 0b0001) {
      paramRelayN++; // next parameter
      largeLcd.printString("      ",1); //clear place for next parameter display
    }
    // -
    if(keys == 0b0010){
      if(paramRelayN < 8) // days of week
      {
        relayOnSchedule.daysMask = relayOnSchedule.daysMask & ~(1 << paramRelayN);
      }
      if(paramRelayN == 8) {
        relayOnSchedule.hour--;
        if(relayOnSchedule.hour > 250) relayOnSchedule.hour = 23;
      }
      if(paramRelayN == 9) {
        relayOnSchedule.minute--;
        if(relayOnSchedule.minute > 250) relayOnSchedule.minute = 59;
      }
    }
    // +
    if(keys == 0b0100){
      if(paramRelayN > 0 && paramRelayN < 8) {
        relayOnSchedule.daysMask = relayOnSchedule.daysMask | (1 << paramRelayN);
      }
      if(paramRelayN == 8) {
        relayOnSchedule.hour++;
        if(relayOnSchedule.hour > 23) relayOnSchedule.hour = 0;
      }
      if(paramRelayN == 9) {
        relayOnSchedule.minute++;
        if(relayOnSchedule.minute > 59) relayOnSchedule.minute = 0;
      }
    }
  }
  EEPROM.put(RELAY_ON_EEPROM_ADDR, relayOnSchedule); // remark - EEPROM.put writes bits only if are changed to prevent wear


   // Set relay wsitch-OFF day and time
  paramRelayN = 1; // NN of parameter we will modify
  while(paramRelayN < 10 ){
    // assemble and display schedule: days of week, time
    char daysStr[8];
    for (uint8_t day = 1; day < 8; day++){
      if(relayOffSchedule.daysMask & (1 << day)){
        daysStr[day - 1] = '0' + day; // ASCII character from '1' to '7'
      } else {
        daysStr[day - 1] = '_';
      }
    }
    daysStr[7] = '\0';
    snprintf(msg, sizeof(msg), "OFF%s %02d:%02d", daysStr, relayOffSchedule.hour, relayOffSchedule.minute);
    largeLcd.printString(msg);
    largeLcd.printString(paramRelay[paramRelayN],1);

    keys = readKeys();
    if(keys == 0b0001) {
      paramRelayN++; // next parameter
      largeLcd.printString("      ",1); //clear place for next parameter display
    }
    // -
    if(keys == 0b0010){
      if(paramRelayN < 8) // days of week
      {
        relayOffSchedule.daysMask = relayOffSchedule.daysMask & ~(1 << paramRelayN);
      }
      if(paramRelayN == 8) {
        relayOffSchedule.hour--;
        if(relayOffSchedule.hour > 250) relayOffSchedule.hour = 23;
      }
      if(paramRelayN == 9) {
        relayOffSchedule.minute--;
        if(relayOffSchedule.minute > 250) relayOffSchedule.minute = 59;
      }
    }
    // +
    if(keys == 0b0100){
      if(paramRelayN > 0 && paramRelayN < 8) {
        relayOffSchedule.daysMask = relayOffSchedule.daysMask | (1 << paramRelayN);
      }
      if(paramRelayN == 8) {
        relayOffSchedule.hour++;
        if(relayOffSchedule.hour > 23) relayOffSchedule.hour = 0;
      }
      if(paramRelayN == 9) {
        relayOffSchedule.minute++;
        if(relayOffSchedule.minute > 59) relayOffSchedule.minute = 0;
      }
    }
  }
  EEPROM.put(RELAY_ON_EEPROM_ADDR, relayOffSchedule); // remark - EEPROM.put writes bits only if are changed to prevent wear
}

void menu() {
  largeLcd.clear();
  // display key tips on 2nd string
  largeLcd.printString("BACK  TIME RELAY", 1);

  // listen to keys for limited time
  // diaplay relay on/off schedule on 1st string
  unsigned long startTime = millis();
  while((millis() - startTime) < 10000){

    char daysStr[8]; //buffer for display day of week schedule
    if(((millis() - startTime)/1000)%2){
      // display relay ON time
      // convert day of week schedule byte to string for display
      for (uint8_t day = 1; day < 8; day++){
        if(relayOnSchedule.daysMask & (1 << day)){
          daysStr[day - 1] = '0' + day; // ASCII character from '1' to '7'
        } else {
          daysStr[day - 1] = '_';
        }
      }
      daysStr[7] = '\0';
      snprintf(msg, sizeof(msg), "ON %s %02d:%02d", daysStr, relayOnSchedule.hour, relayOnSchedule.minute);
      largeLcd.printString(msg);
    } else {
      // display relay OFF time
      // convert day of week schedule byte to string for display
      for (uint8_t day = 1; day < 8; day++){
        if(relayOffSchedule.daysMask & (1 << day)){
          daysStr[day - 1] = '0' + day; // ASCII character from '1' to '7'
        } else {
          daysStr[day - 1] = '_';
        }
      }
      daysStr[7] = '\0';
      snprintf(msg, sizeof(msg), "OFF%s %02d:%02d", daysStr, relayOffSchedule.hour, relayOffSchedule.minute);
      largeLcd.printString(msg);
    }

    // process keys
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

void processRelay(DateTime now) {
  bool isTodayActive;
  uint8_t currentdayOfWeek = now.dayOfTheWeek();
  if (currentdayOfWeek == 0) currentdayOfWeek = 7; // Convert Sunday from 0 to 7

  // process swithing ON
  isTodayActive = relayOnSchedule.daysMask & (1 << currentdayOfWeek);
  if (isTodayActive &&
    relayOnSchedule.hour == now.hour() &&
    relayOnSchedule.minute == now.minute() &&
    relayState == 0) {    
      digitalWrite(RELAY_PIN, 1);
      relayState = 1;
  }

  // process swithing OFF
  isTodayActive = relayOffSchedule.daysMask & (1 << currentdayOfWeek);
  if (isTodayActive &&
    relayOffSchedule.hour == now.hour() &&
    relayOffSchedule.minute == now.minute() &&
    relayState == 1) {    
      digitalWrite(RELAY_PIN, 0);
      relayState = 0;
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

  EEPROM.get(RELAY_ON_EEPROM_ADDR, relayOnSchedule);
  EEPROM.get(RELAY_OFF_EEPROM_ADDR, relayOffSchedule);

  // reset Relay on-off schedule in case of garbage data present in EEPROM
  if (relayOnSchedule.hour > 23 ||
    relayOnSchedule.minute > 59 ||
    relayOffSchedule.hour > 23||
    relayOffSchedule.minute > 59)
  {
      relayOnSchedule.daysMask = 0b00000000; //do dot swith relay on on any day of week
      relayOnSchedule.hour = 7; // 07:00
      relayOnSchedule.minute = 0;

      relayOffSchedule.daysMask = 0b00000000; //do not switch relay off on any day of week
      relayOffSchedule.hour = 8; // 08:00
      relayOffSchedule.minute = 0;
  }
  
  largeLcd.clear();
}

void loop() {
  now = rtc.now();

  // every minute
  if (now.minute() != lastMinute) {
    lastMinute = now.minute();
    displayTime(now);
  }

  // every second
  if (now.second() != lastSecond) {
    lastSecond = now.second();   
    processRelay(now);
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