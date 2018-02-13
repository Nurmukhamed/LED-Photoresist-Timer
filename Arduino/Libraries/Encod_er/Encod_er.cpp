/*
Encod_er.h - библиотека для обработки сигналов инкрементного энкодера параллельным процессом
 
 Функция scanState() должна вызываться регулярно в параллельном процессе
 
 timeRight - время/признак поворота энкодера вправо, 
 timeLeft  - время/признак поворота энкодера влево, если = 0, то поворота не было
			 если = 0, то поворота не было
			 если не = 0, то содержит время последнего изменения / 8 периодов вызова scanState()
 position  - абсолютное положение энкодера
 read()    - чтение position 
 
 Библиотека разработана Калининым Эдуардом
 http://mypractic.ru/
 Уроки Ардуино, урок 55.
 
*/


#include "Arduino.h"
#include "Encod_er.h"


// конструктор
Encod_er::Encod_er(byte pinA, byte pinB, byte timeFilter) {
  _pinA = pinA;
  _pinB = pinB;
  _timeFilter = timeFilter;
  pinMode(_pinA, INPUT_PULLUP);  // определяем вывод как вход
  pinMode(_pinB, INPUT_PULLUP);  // определяем вывод как вход
  _countA= 0;
  _countB= 0;
  _changeEncCount= 0;
  _divChangecCount= 0;
  timeRight= 0;
  timeLeft= 0;
  position= 0;
  _flagPressA= false;
  _flagPressB= false;  
}


// метод сканирования состояния энкодера
void Encod_er::scanState() {

  // счетчик времени
  _divChangecCount++;
  if(_divChangecCount > 8) {
    _divChangecCount= 0;
    _changeEncCount++;
    if(_changeEncCount == 0) _changeEncCount= 255;
  }

  // сигнал B энкодера  
  if ( _flagPressB != digitalRead(_pinB) ) {
    //  состояние осталось прежним 
    _countB= 0;  // сброс счетчика подтверждений 
  }
  else {
    // состояние изменилось
    _countB++;   // +1 к счетчику подтверждений

    if ( _countB >= _timeFilter ) {
      // состояние сигнала не мянялось в течение времени _timeFilter
      _flagPressB= ! _flagPressB; // инверсия признака состояния
      _countB= 0;  // сброс счетчика подтверждений
     }    
  }

  // сигнал A энкодера  
  if ( _flagPressA != digitalRead(_pinA) ) {
    //  состояние осталось прежним 
    _countA= 0;  // сброс счетчика подтверждений 
  }
  else {
    // состояние изменилось
    _countA++;   // +1 к счетчику подтверждений

    if ( _countA >= _timeFilter ) {
      // состояние сигнала не мянялось в течение времени _timeFilter
      _flagPressA= ! _flagPressA; // инверсия признака состояния
      _countA= 0;  // сброс счетчика подтверждений

      if ( _flagPressA == true ) {
        // --__ , обработка сигналов энкодера
    
        if( _flagPressB == true) {
          // против часовой стрелки
          position--;
          timeLeft= _changeEncCount;
          if(timeLeft == 0) timeLeft= 1;
          timeRight= 0;
        }
        else {
          // по часовой стрелке
          position++;
          timeRight= _changeEncCount;
          if(timeRight == 0) timeRight= 1;
          timeLeft= 0;
        }      
        _changeEncCount= 0;
        _divChangecCount= 0;                    
      }                       
    }    
  }  
}


// метод чтения положения энкодера
long Encod_er::read() {
  return(position);
}
