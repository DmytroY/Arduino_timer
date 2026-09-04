#ifndef LARGE_LCD_H
#define LARGE_LCD_H

#include <Arduino.h>
#include <LiquidCrystal.h>

class LargeLCD {
public:
  // Constructor specifying Arduino pins connected to the LiquidCrystal display
  LargeLCD(uint8_t rs, uint8_t enable, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);

  void begin();
  void clear();
  void bigDigit(int digit, int startPos);
  void separator2(int startPos);
  void sp1(int startPos);
  void sp2(int startPos);
  void sp3(int startPos);

  // - length: minimum field width (0 = auto / dynamic width)
  // - padWithZero: true for leading '0', false for leading spaces
  void bigNumber(int number, int startPos, int length = 0, bool padWithZero = false);

  void printString(const char* str,  int row = 0, int col = 0);

private:
  LiquidCrystal lcd;

  // Custom character building helper methods
  void createCustomChars();
  
  // Digit segment renderers
  void d0(int pos); void d1(int pos); void d2(int pos);
  void d3(int pos); void d4(int pos); void d5(int pos);
  void d6(int pos); void d7(int pos); void d8(int pos);
  void d9(int pos);

  // Type alias for class member function pointers
  typedef void (LargeLCD::*DigitFunc)(int pos);
  static const DigitFunc digitFunctions[10];
};

#endif