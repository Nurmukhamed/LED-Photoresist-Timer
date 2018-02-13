/*
 LED засветка фоторезиста и паяльной маски
 
 https://github.com/phpscriptru/LED-Photoresist-Timer
*/

#include <LiquidCrystal.h>
#include <Bounce.h>
#include <Encod_er.h>

long Preset1 = 20;  // предустановка времени №1 (сек.)
long Preset2 = 140;  // предустановка времени №2 (сек.)
long Preset3 = 240;  // предустановка времени №3 (сек.)
int sec_step = 5; // шаг энкодера (сек.)

unsigned long currentTime; // текущее время
unsigned long loopTime; // время окончания
unsigned long LimitTime = 0; // таймер
boolean working = false; // флаг запуска отсчета
int n1,n2,n3,n4,n5 = LOW;
LiquidCrystal lcd(12, 11, 10, 9, 8, 7); // экран
Encod_er encoder(A2, A1, 2); // энкодер
Bounce bouncer1 = Bounce(6, 5); // дребезг кнопок
Bounce bouncer2 = Bounce(5, 5); 
Bounce bouncer3 = Bounce(4, 5);  
Bounce bouncer4 = Bounce(A0, 5); 

void setup() {

  Preset1 = Preset1*1000;
  Preset2 = Preset2*1000;
  Preset3 = Preset3*1000;
  sec_step = sec_step*1000;
  LimitTime = 0;
  pinMode(A1, INPUT);// входы
  pinMode(A2, INPUT);
  pinMode(A0, INPUT);
  digitalWrite(A0, HIGH);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(6, INPUT);
  pinMode(13, OUTPUT);
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print("  UV LED TIMER  ");
  lcd.setCursor(0,1);
  lcd.print("    ver. 2.0    ");
  delay(1000);
  lcd.clear();
  currentTime = millis();
}

void loop() {

  bouncer1.update(); // дребезг кнопок
  bouncer2.update();
  bouncer3.update();
  bouncer4.update();
 
  /**
   * Выбран режим отсчета
   */
  if (working == true) {

    currentTime = millis();
    loopTime = currentTime + LimitTime; // когда закончить
    while(currentTime < loopTime) { // сравниваем текущее время с временем окончания

      digitalWrite(13, HIGH); // включаем

      // вывод на экран
      lcd.setCursor(0,0);
      lcd.print("   Exposure...  "); 
      lcd.setCursor(4, 2);  
      if (LimitTime/60/60/1000 < 10) { lcd.print("0"); }
      lcd.print(LimitTime/60/60/1000);
      lcd.print(":");
      if (LimitTime/60/1000%60 < 10) { lcd.print("0"); }
      lcd.print((LimitTime/60/1000)%60);
      lcd.print(":");
      if (LimitTime/1000%60 < 10) { lcd.print("0"); }
      lcd.print(LimitTime/1000%60);
      lcd.display();

      // кнопка энкодера
      bouncer4.update();
      if (bouncer4.read() != n4) {
         n4 = bouncer4.read();
        if (n4 && working == true) {
          working = false;
          digitalWrite(13, HIGH); // включаем
          LimitTime = 0;
          break;
        }
      }

      LimitTime = LimitTime - (millis() - currentTime); // уменьшаем таймер
      currentTime = millis(); // получаем новое время
    }

    digitalWrite(13, LOW); // отключаем 

    lcd.clear();
    lcd.print("     FINISH     ");
    working = false;
    delay (3000);

  /**
   * Выбран режим клавиатуры
   */
  } else {

    digitalWrite(13, LOW); // отключаем  
    
    if (LimitTime > 86400000) LimitTime = 0; // отбрасываем некорректные значения

    // вывод на экран
    lcd.setCursor(0,0);
    lcd.print("   Set timer:   ");
    lcd.setCursor(4, 2);  
    if (LimitTime/60/60/1000 < 10) { lcd.print("0"); }
    lcd.print(LimitTime/60/60/1000);
    lcd.print(":");
    if (LimitTime/60/1000%60 < 10) { lcd.print("0"); }
    lcd.print((LimitTime/60/1000)%60);
    lcd.print(":");
    if (LimitTime/1000%60 < 10) { lcd.print("0"); }
    lcd.print(LimitTime/1000%60);

    // пресет №1
    if (bouncer1.read() != n1) {
      n1 = bouncer1.read();
      if (n1) {
        lcd.setCursor(0,1);
        lcd.print("    preset 1    ");
        LimitTime = Preset1;
        delay(1000);
      }
    }

    // пресет №2
    if (bouncer2.read() != n2) {
      n2 = bouncer2.read();
      if (n2) {
        lcd.setCursor(0,1);
        lcd.print("    preset 2    ");
        LimitTime = Preset2;
        delay(1000);
      }
    }

    // пресет №3
    if (bouncer3.read() != n3) {
      n3 = bouncer3.read();
      if (n3) {
        lcd.setCursor(0,1);
        lcd.print("    preset 3    ");
        LimitTime = Preset3;
        delay(1000);
      }
    }

    // энкодер
    encoder.scanState(); 
    if(encoder.timeRight != 0) {
      encoder.read();
      encoder.timeRight= 0;
      LimitTime += sec_step;
    }
    if(encoder.timeLeft != 0) {
      encoder.read();
      encoder.timeLeft= 0;
      LimitTime -= sec_step;
    } 
    
    // кнопка энкодера
    if (bouncer4.read() != n4) {
      n4 = bouncer4.read();
      if (n4 && LimitTime != 0) {
        working = true;
      }
    }
  }
}
