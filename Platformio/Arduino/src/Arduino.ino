// ----------------------------------------------------------------------------
// LED засветка фоторезиста и паяльной маски
// версия 2.1 (Nano)
// (c) 2019 Дмитрий Дементьев <info@phpscript.ru>
// ----------------------------------------------------------------------------

#include <EEPROM.h>
#if ARDUINO >= 100
  #include <Arduino.h>
#else
  #include <WProgram.h>
#endif
#include "config.h"
#include "button.h"
#include "lcd.h"

LCD       lcd;
Data1602  fullDataScreen;
BUTTON    button1;
BUTTON    button2;
BUTTON    button3;

int  Presets[]    = {0, 0, 0};
int  PresetsBtn[] = {1, 2, 3};
unsigned long currentTime;   // текущее время
unsigned long loopTime;      // время окончания
unsigned long LimitTime = 0; // таймер
volatile boolean turn;
volatile boolean up;
boolean start = false;       // флаг запуска отсчета

void setup() {

  Serial.begin(9600);
  Presets[0] = EEPROM.read(0); // берем пресеты из EEPROM
  Presets[1] = EEPROM.read(1);
  Presets[2] = EEPROM.read(2);
  for(int i=0; i<2; i++) {
    if (Presets[i] < 0 || Presets[i] > 86400) Presets[i] = 0;
  }
  pinMode(PinCLK, INPUT); // входы
  pinMode(PinDT, INPUT);
  pinMode(PinSW, INPUT);
  digitalWrite(PinSW, HIGH);
  pinMode(PinPres1, INPUT);
  pinMode(PinPres2, INPUT);
  pinMode(PinPres3, INPUT);
  pinMode(PinMosf, OUTPUT);
  attachInterrupt (0, isr, CHANGE); // прерывание энкодера
  currentTime = millis();
  lcd.init();
  lcd.splashScreen();
  delay(3000);
}

void loop() {

  /**
     выбран режим отсчета
  */
  if (start == true) {

    digitalWrite(PinMosf, HIGH); // включаем

    // отсчет времени
    currentTime = millis();
    loopTime = currentTime + LimitTime*1000; // когда закончить  
    while (currentTime < loopTime) { // сравниваем текущее время с временем окончания

      int h = LimitTime/60/60;
      int m = LimitTime/60%60;
      int s = LimitTime%60;
  
      sprintf(fullDataScreen.stroka1, "%s", "   Working...   ");
      sprintf(fullDataScreen.stroka2, "    %02d:%02d:%02d    ", h,m,s);

      LimitTime = (loopTime - currentTime) / 1000; // уменьшаем таймер
      currentTime = millis(); // получаем новое время

      lcd.updateScreen(&fullDataScreen);
    }

    // окончание работы таймера
    digitalWrite(PinMosf, LOW); // отключаем засветку

    sprintf(fullDataScreen.stroka1, "%s", "     FINISH     ");
    lcd.updateScreen(&fullDataScreen);
    start = false;
    delay (3000);

    /**
       выбран режим выбора времени
    */
  } else {

    digitalWrite(PinMosf, LOW); // отключаем засветку
     
    if (LimitTime > 86400) LimitTime = 0; // отбрасываем некорректные значения
    int h = LimitTime/60/60;
    int m = LimitTime/60%60;
    int s = LimitTime%60;

    sprintf(fullDataScreen.stroka1, "%s", "   Set timer:   ");
    sprintf(fullDataScreen.stroka2, "    %02d:%02d:%02d", h,m,s);

    // пресетs №1,2,3
    int bReturn[10];
    bReturn[0] = button1.checkButton(PinPres1);
    bReturn[1] = button2.checkButton(PinPres2);
    bReturn[2] = button3.checkButton(PinPres3);
    checkButtonClick(bReturn);

    // энкодер
    if (turn) {
      if (up) {
        LimitTime -= secStep;
      } else {
        LimitTime += secStep;
      }
      turn = false;
      if (LimitTime < 0) LimitTime = 0;
    }
    // кнопка энкодера
    if (digitalRead(PinSW) == LOW) {
      start = true;
    }
    lcd.updateScreen(&fullDataScreen);
    delay(250);
  }
}

/**
 * прерывание энкодера
 */
void isr ()  {
  up = (digitalRead(PinCLK) == digitalRead(PinDT));
  turn = true;
}

/**
 * кнопка
 */
void checkButtonClick (int bReturn[])  {
  for(int i=0; i<3; i++) {
    if (bReturn[i] == 1) { // нажатие на кнопку
      sprintf(fullDataScreen.stroka2, "    preset #%d    ", i+1);
      lcd.updateScreen(&fullDataScreen);
      LimitTime = Presets[i];
      delay(1000);
    }
    if (bReturn[i] == 2) { } // двойное нажатие на кнопку
    if (bReturn[i] == 3) { // удержание кнопки
      EEPROM.update(i, LimitTime);
      Presets[i] = LimitTime;
      sprintf(fullDataScreen.stroka2, "    save #%d    ", i+1);
      lcd.updateScreen(&fullDataScreen);
      delay(1000);
    }
    if (bReturn[i] == 4) { } // долгое удержание кнопки
  }
}
