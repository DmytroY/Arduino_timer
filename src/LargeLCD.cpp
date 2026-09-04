#include "LargeLCD.h"

// Segment Definitions
static const byte LT[8]  = {B00111, B01111, B11111, B11111, B11111, B11111, B11111, B11111};
static const byte UB[8]  = {B11111, B11111, B11111, B00000, B00000, B00000, B00000, B00000};
static const byte RT[8]  = {B11100, B11110, B11111, B11111, B11111, B11111, B11111, B11111};
static const byte LL[8]  = {B11111, B11111, B11111, B11111, B11111, B11111, B01111, B00111};
static const byte LB[8]  = {B00000, B00000, B00000, B00000, B00000, B11111, B11111, B11111};
static const byte LR[8]  = {B11111, B11111, B11111, B11111, B11111, B11111, B11110, B11100};
static const byte UMB[8] = {B11111, B11111, B11111, B00000, B00000, B00000, B11111, B11111};
static const byte LMB[8] = {B11111, B00000, B00000, B00000, B00000, B11111, B11111, B11111};

// Member function pointer map
const LargeLCD::DigitFunc LargeLCD::digitFunctions[10] = {
    &LargeLCD::d0,
    &LargeLCD::d1,
    &LargeLCD::d2,
    &LargeLCD::d3,
    &LargeLCD::d4,
    &LargeLCD::d5,
    &LargeLCD::d6,
    &LargeLCD::d7,
    &LargeLCD::d8,
    &LargeLCD::d9
};

LargeLCD::LargeLCD(uint8_t rs, uint8_t enable, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
  : lcd(rs, enable, d4, d5, d6, d7) {}

void LargeLCD::begin() {
    lcd.begin(16, 2);
    createCustomChars();
    lcd.clear();
}

void LargeLCD::clear() {
    lcd.clear();
}

void LargeLCD::createCustomChars() {
    lcd.createChar(0, (byte*)LT);
    lcd.createChar(1, (byte*)UB);
    lcd.createChar(2, (byte*)RT);
    lcd.createChar(3, (byte*)LL);
    lcd.createChar(4, (byte*)LB);
    lcd.createChar(5, (byte*)LR);
    lcd.createChar(6, (byte*)UMB);
    lcd.createChar(7, (byte*)LMB);
}

void LargeLCD::bigDigit(int digit, int startPos) {
  if (digit >= 0 && digit <= 9) {
    (this->*digitFunctions[digit])(startPos);
  }
}

void LargeLCD::bigNumber(int number, int startPos, int length, bool padWithZero) {
  int temp = number;
  int digits[10];
  int actualDigitCount = 0;

  // Extract digits in reverse order
  if (number == 0) {
    digits[actualDigitCount++] = 0;
  } else {
    while (temp > 0) {
      digits[actualDigitCount++] = temp % 10;
      temp /= 10;
    }
  }

  // Determine total character slots to print
  int totalSlots = (length > actualDigitCount) ? length : actualDigitCount;
  int currentPos = startPos;

  // Iterate from left to right across total slots
  for (int i = totalSlots - 1; i >= 0; i--) {
    if (i < actualDigitCount) {
      // Print actual digit
      bigDigit(digits[i], currentPos);
    } else {
      // Print padding (leading zero or leading space)
      if (padWithZero) {
        bigDigit(0, currentPos);
      } else {
        sp3(currentPos); // Prints 3 blank spaces
      }
    }
    currentPos += 3; // Advance position by width of custom digit (3 chars)
    // add margin
    lcd.setCursor(currentPos, 0); lcd.write(254); 
    lcd.setCursor(currentPos, 1); lcd.write(254);
    currentPos++;
  }
}

void LargeLCD::separator2(int pos) {
    lcd.setCursor(pos, 0); lcd.write(4); lcd.write(4);
    lcd.setCursor(pos, 1); lcd.write(1); lcd.write(1);
}

void LargeLCD::sp1(int pos) {
    lcd.setCursor(pos, 0); lcd.write(254);
    lcd.setCursor(pos, 1); lcd.write(254);
}

void LargeLCD::sp2(int pos) {
    lcd.setCursor(pos, 0); lcd.write(254); lcd.write(254);
    lcd.setCursor(pos, 1); lcd.write(254); lcd.write(254);
}

void LargeLCD::sp3(int pos) {
    lcd.setCursor(pos, 0); lcd.write(254); lcd.write(254); lcd.write(254);
    lcd.setCursor(pos, 1); lcd.write(254); lcd.write(254); lcd.write(254);
}

// Individual Digit Builders
void LargeLCD::d0(int p) {
    lcd.setCursor(p, 0); lcd.write((byte)0); lcd.write(1); lcd.write(2);
    lcd.setCursor(p, 1); lcd.write(3); lcd.write(4); lcd.write(5);
}

void LargeLCD::d1(int p) { 
    lcd.setCursor(p, 0); lcd.write(254); lcd.write(2); lcd.write(254);
    lcd.setCursor(p, 1); lcd.write(254); lcd.write(255); lcd.write(254);
}

void LargeLCD::d2(int p) {
    lcd.setCursor(p, 0); lcd.write(6); lcd.write(6); lcd.write(2);
    lcd.setCursor(p, 1); lcd.write(255); lcd.write(7); lcd.write(7);
}

void LargeLCD::d3(int p) {
    lcd.setCursor(p, 0); lcd.write(6); lcd.write(6); lcd.write(2);
    lcd.setCursor(p, 1); lcd.write(7); lcd.write(7); lcd.write(5);
}

void LargeLCD::d4(int p) {
    lcd.setCursor(p, 0); lcd.write(3); lcd.write(4); lcd.write(2);
    lcd.setCursor(p, 1); lcd.write(254); lcd.write(254); lcd.write(255);
}

void LargeLCD::d5(int p) {
    lcd.setCursor(p, 0); lcd.write(255); lcd.write(6); lcd.write(6);
    lcd.setCursor(p, 1); lcd.write(7); lcd.write(7); lcd.write(5);
}

void LargeLCD::d6(int p) {
    lcd.setCursor(p, 0); lcd.write((byte)0); lcd.write(6); lcd.write(6);
    lcd.setCursor(p, 1); lcd.write(3); lcd.write(7); lcd.write(5);
}

void LargeLCD::d7(int p) {
    lcd.setCursor(p, 0); lcd.write(1); lcd.write(1); lcd.write(2);
    lcd.setCursor(p, 1); lcd.write(254); lcd.write((byte)0); lcd.write(254);
}

void LargeLCD::d8(int p) {
    lcd.setCursor(p, 0); lcd.write((byte)0); lcd.write(6); lcd.write(2);
    lcd.setCursor(p, 1); lcd.write(3); lcd.write(7); lcd.write(5);
}

void LargeLCD::d9(int p) {
    lcd.setCursor(p, 0); lcd.write((byte)0); lcd.write(6); lcd.write(2);
    lcd.setCursor(p, 1); lcd.write(254); lcd.write(254); lcd.write(5);
}

void LargeLCD::printString(const char* str,  int row = 0, int col = 0) {
  lcd.setCursor(col, row);
  lcd.print(str);
}