#ifndef LCD_H
#define LCD_H

#if ARDUINO >= 100
  #include <Arduino.h>
#else
  #include <WProgram.h>
#endif

#include <LiquidCrystal.h>

struct Data1602 {
  int  first;       // Координата начала
  char stroka1[16];  // Первая строка
  char stroka2[16];  // Вторая строка
};

class LCD {
  private:
    LiquidCrystal *display; 
  public:
    bool isset;
    LCD();
    void init();
    void splashScreen();
    void updateScreen(Data1602* fullData);
};

#endif
