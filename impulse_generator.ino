#include <EncButton.h>          // библиотека для работы с энкодером
#include <LiquidCrystal_I2C.h>  // библиотека для работы с экраном
#include <string.h>             // библиотека для работы со строками

// объявление констант для выполнения заданий по ТЗ
#define MANUAL_DURATION 70         // длительность импульса (мкс)
#define MANUAL_PERIOD 150          // период импульса (мкс)
#define MANUAL_BURST_WIDTH 7       // количество импульсов в одной пачке
#define MANUAL_AMOUNT_OF_BURSTS 4  // количество пачек
#define MANUAL_DELAY_TIME 5        // задержка между пачками (мс)

// объявление базавых параметров импульса
int Duration = 10;  // длительность импульса (мкс)
int Period = 20;    // период импульса (мкс)
int Burst = 1;      // количество импульсов в одной пачке
int Amount = 1;     // количество пачек
int Delay = 1;      // задержка между пачками (мс)

// объявление объектов классов
LiquidCrystal_I2C lcd(0x27, 16, 2);
EncButton<EB_TICK, 2, 3, 4> enc;

// объявление выходных пинов
int LedPin = 12;      // пин для работы светодиода
int ImpulsePin = 13;  // пин для выходного импульса

// объявление флагов
bool isBurst = false;   // работа в режиме пачек
bool isSingle = false;  // работа в режиме одиночного сигнала

// объявление указателей номера текущего пункта меню
int menuMode = 0;  // указатель на номер активного пункта стартового меню
int current = 0;   // указатель на номер активного пункта общего меню

// объявление списков меню и их размеров
// меню для работы в режиме пачек
String burstOptions[] = { "BurstMode", "Delay", "Duration", "Period", "Burst", "Amount", "ManualMode", "Exit" };
// меню для работы в одиночного сигнала
String singleOptions[] = { "SingleMode", "Duration", "Period", "Exit" };
int burstOptionsLength = 8;   // размер burstOptions
int singleOptionsLength = 4;  // размер singleOptions


// Функция для изменения длительности импульса
void changeDuration() {
  if (enc.turnH()) {  // обработка поворота энкодера с нажатием
    if (Duration < 1000) {
      Duration += -enc.dir();
    } else if (Duration >= 1000 && Duration < 10000) {
      Duration += -enc.dir() * 10;
    } else if (Duration >= 10000 && Duration <= 999000) {
      Duration += -enc.dir() * 1000;
      // проверка на правильность введённого значения
    } else {
      Duration = 999000;
    }
    if (Duration < 10) {
      Duration = 10;
    }
    // чтобы избежать ошибок при генерации импульса, увеличим период
    // периода на 1 при равенстве периода и длительности импульса
    if (Duration > Period) {
      Period = Duration + 1;
    }
  }
}

// функция изменения периода импульса
void changePeriod() {
  if (enc.turnH()) {  // обработка поворота энкодера с нажатием
    if (Period < 1000) {
      Period += -enc.dir();
    } else if (Period >= 1000 && Period < 10000) {
      Period += -enc.dir() * 10;
    } else if (Period >= 10000 && Period <= 999000) {
      Period += -enc.dir() * 1000;
      // проверка на правильность введённого значения
    } else {
      Period = 999000;
    }

    if (Period < 20) {
      Period = 20;
    }
    // чтобы избежать ошибок при генерации импульса, уменьшим длительность
    // импульса на 1 при равенстве периода и длительности импульса
    if (Duration >= Period) {
      Duration = Period - 1;
    }
  }
}

// функция изменения задержки между пачками
void changeDelay() {
  if (enc.turnH()) {  // обработка поворота энкодера с нажатием
    Delay += -enc.dir();
    // проверка на правильность введённого значения
    if (Delay < 1) {
      Delay = 1;
    }
    if (Delay > 999) {
      Delay = 999;
    }
  }
}

// функция изменения количества импульсов в одной пачке
void changeBurst() {
  if (enc.turnH()) {  // обработка поворота энкодера с нажатием
    // проверка на правильность введённого значения
    Burst += -enc.dir();
    if (Burst < 1) {
      Burst = 1;
    }
    if (Burst > 999) {
      Burst = 999;
    }
  }
}

// функция изменения количества пачек
void changeAmount() {
  if (enc.turnH()) {  // обработка поворота энкодера с нажатием
    // проверка на правильность введённого значения
    Amount += -enc.dir();
    if (Amount < 1) {
      Amount = 1;
    }
    if (Amount > 999) {
      Amount = 999;
    }
  }
}

// функция генерации одиночного импульса
void singleImpulse(int duration, int period) {
  digitalWrite(ImpulsePin, HIGH);  // вывод +5V сигнала на выходной пин
  if (duration < 10000) {
    delayMicroseconds(duration);           // задержка длительности импульса в мкс
    digitalWrite(ImpulsePin, LOW);         // вывод 0V сигнала на выходной пин
    delayMicroseconds(period - duration);  // задержка сигнала исходя из периода в мкс
  } else {
    delay(toThousands(duration));           // задержка длительности импульса в мс
    digitalWrite(ImpulsePin, LOW);          // вывод 0V сигнала на выходной пин
    delay(toThousands(period - duration));  // задержка сигнала исходя из периода в мс
  }
}

// функция для работы ардуино в режиме генерации пачек по нажатии кнопки
void burstMode(int duration, int period, int delayTime, int burstWidth, int amountOfBursts) {
  if (enc.press())  // проверка нажатия кнопки энкодера
  {
    for (int i = 0; i < amountOfBursts; ++i)  // цикл по количеству пачек
    {
      for (int j = 0; j < burstWidth; ++j)  // цикл по количеству импульсов
      {
        singleImpulse(duration, period);  // вызов функции генерации одиночного импульса
      }
      delay(delayTime);  // задержка между пачками в мс
    }
  }
}

// функция для выхода из меню
void exit() {
  if (enc.press()) {  // обработка нажатия энкодера
    // обнуление всех флагов и указателей
    isBurst = false;
    isSingle = false;
    current = 0;
    menuMode = 0;
    // вызов стартового меню
    startupMenu();
  }
}

// функция вывода стартового меню
void startupMenu() {
  // обработка указателя на текущий номер стартового меню
  switch (menuMode) {
    case 0:  // работа в режиме генерации пачек
      if (enc.press()) {
        isBurst = true;
        lcd.clear();
        printMenu(current, burstOptions);
        return;
      }
      break;
    case 1:  // работа в режиме генерации одиночного импульса
      if (enc.press()) {
        isSingle = true;
        lcd.clear();
        printMenu(current, singleOptions);
        return;
      }
      break;
  }

  // вывод знака стрелки рядом с активным пунктом меню
  lcd.setCursor(15, menuMode);
  lcd.write(127);

  // обработка поворота энкодера
  if (enc.turn()) {
    if (menuMode == 0) {
      ++menuMode;
    } else {
      --menuMode;
    }

    lcd.clear();
  }

  // вывод на экран меню
  lcd.setCursor(0, 0);
  lcd.print("BurstMode");
  lcd.setCursor(0, 1);
  lcd.print("SingleMode");
}

// функция перевода из единиц в тысячи
float toThousands(int value) {
  return (float)((int)(value * 0.1)) * 0.01;
}

// функция вывода меню
void printMenu(int current, String options[]) {
  lcd.setCursor(0, 0);
  lcd.print(options[current]);
  lcd.setCursor(9, 0);

  // обрабтка текущего пункта меню
  if (options[current] == "Duration") {
    if (Duration <= 999) {
      lcd.print(Duration);
      lcd.print("us");
    } else if (Duration >= 10000 && Duration <= 999000) {  // проверка на размер значения
      lcd.print((int)toThousands(Duration));
      lcd.print("ms");
    } else {
      lcd.print(toThousands(Duration));
      lcd.print("ms");
    }
  } else if (options[current] == "Period") {
    if (Period <= 999) {
      lcd.print(Period);
      lcd.print("us");
    } else if (Period >= 10000 && Period <= 999000) {  // проверка на размер значения
      lcd.print((int)toThousands(Period));
      lcd.print("ms");
    } else {
      lcd.print(toThousands(Period));
      lcd.print("ms");
    }
  } else if (options[current] == "Delay") {
    lcd.print(Delay);
    lcd.print("ms");
  } else if (options[current] == "Burst") {
    lcd.print(Burst);
  } else if (options[current] == "Amount") {
    lcd.print(Amount);
  }

  // вывод на первую строчку экрана знака стрелки
  lcd.setCursor(15, 0);
  lcd.write(127);
  lcd.setCursor(0, 1);

  // проверка на выход за пределы списка опций
  if (options[current] == "Exit") {
    lcd.print(options[0]);
  } else {
    lcd.print(options[current + 1]);
  }
}

// функция вывода и обработки значений меню в режиме генерации пачек
void burstMenu() {
  // обработка текущего значения в списке опций и вызов соответсвующей функции
  if (burstOptions[current] == "BurstMode") {
    burstMode(Duration, Period, Delay, Burst, Amount);
  } else if (burstOptions[current] == "ManualMode") {
    burstMode(MANUAL_DURATION, MANUAL_PERIOD, MANUAL_DELAY_TIME, MANUAL_BURST_WIDTH, MANUAL_AMOUNT_OF_BURSTS);
  } else if (burstOptions[current] == "Exit") {
    exit();
  } else if (burstOptions[current] == "Delay") {
    changeDelay();
  } else if (burstOptions[current] == "Duration") {
    changeDuration();
  } else if (burstOptions[current] == "Period") {
    changePeriod();
  } else if (burstOptions[current] == "Burst") {
    changeBurst();
  } else if (burstOptions[current] == "Amount") {
    changeAmount();
  }

  // обработка работы энкодера и вывод текущего меню
  if (enc.turn() || enc.turnH() || enc.release()) {
    lcd.clear();
    // обработка поворота энкодера и изменение значения current
    if (enc.left()) {
      if (current + 1 == burstOptionsLength) {
        current = 0;
      } else {
        ++current;
      }
    }
    if (enc.right()) {
      if (current == 0) {
        current = burstOptionsLength - 1;
      } else {
        --current;
      }
    }
    printMenu(current, burstOptions);
  }
}

// функция вывода и обработки значений меню в режиме генерации одиночного имульса
void singleMenu() {
  // обработка текущего значения в списке опций и вызов соответсвующей функции
  if (singleOptions[current] == "SingleMode" && enc.press()) {
    singleImpulse(Duration, Period);
  } else if (singleOptions[current] == "Exit") {
    exit();
  } else if (singleOptions[current] == "Duration") {
    changeDuration();
  } else if (singleOptions[current] == "Period") {
    changePeriod();
  }

  // обработка работы энкодера и вывод текущего меню
  if (enc.turn() || enc.turnH() || enc.release()) {
    lcd.clear();
    // обработка поворота энкодера и изменение значения current
    if (enc.left()) {
      if (current + 1 == singleOptionsLength) {
        current = 0;
      } else {
        ++current;
      }
    }
    if (enc.right()) {
      if (current == 0) {
        current = singleOptionsLength - 1;
      } else {
        --current;
      }
    }
    printMenu(current, singleOptions);
  }
}


// настройка параметров ардуино
void setup() {
  // настройка пинов
  pinMode(LedPin, OUTPUT);
  digitalWrite(LedPin, HIGH);
  pinMode(ImpulsePin, OUTPUT);

  // настройка экрана
  lcd.init();
  lcd.backlight();
  lcd.print("betelgevse");
  lcd.noBlink();
  delay(1000);
  lcd.clear();
}

// основной цикл работы ардуино
void loop() {
  enc.tick();  // обработка энкодера

  // обработка флагов и вызов нужного меню в соответствии с флагами
  if (isBurst) {
    burstMenu();
  } else if (isSingle) {
    singleMenu();
  } else {
    startupMenu();
  }
}