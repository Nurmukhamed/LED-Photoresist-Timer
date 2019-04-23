#include "lcd.h"
#include "config.h"

LCD::LCD() {
  isset = true;
   display = new LiquidCrystal(PinLcd1, PinLcd2, PinLcd3, PinLcd4, PinLcd5, PinLcd6);
}

void LCD::init() {
  if (!isset) {
    return;
  }
  display->clear();
  display->begin(16, 2);
}

void LCD::splashScreen() {
  display->setCursor(0, 0);
  display->print("  UV LED TIMER  ");
  display->setCursor(0, 1);
  display->print("    ver. 2.1    ");
}

void LCD::updateScreen(Data1602* fullData) {
  display->setCursor(0, 0);
  display->print(fullData->stroka1);
  display->setCursor(0, 1);
  display->print(fullData->stroka2);
}
