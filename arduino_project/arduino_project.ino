#include <Adafruit_GFX.h>​ //библиотека для дисплея/display (экранчик) https://github.com/adafruit/Adafruit-GFX-Library
#include <Adafruit_PCD8544.h>​ //библиотека конкретно для этого экранчика ldc nokia 5110
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);​

void setup() { // put your setup code here, to run once:​
  Serial.begin(9600);​

  // ускорение Arduino для более быстрого считывания значаний через analogRead
  // defines for setting and clearing register bits
  #ifndef cbi​
  #define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))​
  #endif​
  #ifndef sbi​
  #define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))​
  #endif​

  // set prescale to 16​
  sbi(ADCSRA,ADPS2);​
  cbi(ADCSRA,ADPS1);​
  cbi(ADCSRA,ADPS0);​

  // рисуем на экране
  display.begin();​
  display.clearDisplay();​
  display.setContrast(50);​
  display.setTextSize(1);​
  display.setTextColor(BLACK);​
  display.display();​
}

void loop() { // put your main code here, to run repeatedly:

  //часть 1 - считывание (ускоренный analogRead)
  int values[500];​
  unsigned long time1 = millis();​

  ​for (int i = 0; i < 500; i++) {​
    // 16 * 13 * 1/16E6 = 0.000013000 (13 µs)​
    values[i] = analogRead(A0); ​
    delayMicroseconds(87);​
  }​

  unsigned long time2 = millis();​

  Serial.print("time =");​
  Serial.println(time2 - time1);​

  display.clearDisplay();

  //часть 2 - среднее значение avg
  long avg = 0;​
  int value_max = values[0];​
  int value_min = values[0];​
  for (int i = 0; i < 500; i++) {​
    avg += values[i];​
    if (values[i] > value_max) {​
      value_max = values[i];​
    }​
    if (values[i] < value_min) {​
      value_min = values[i];​
    }​
  }​
  avg /= 500;​

  Serial.print("avg =");​
  Serial.println(avg);​

  display.setCursor(0, 0);​
  display.print("v=");​
  display.print(value_min);​
  display.print(",");​
  display.print(avg);​
  display.print(",");​
  display.println(value_max);

  //часть 3 - подсчет пересечений значений analogRead'а с avg
  int avg_hi = (3*avg + value_max) / 4;​
  int avg_lo = (3*avg + value_min) / 4;​

  int cross = 0;​
  if (avg_hi - avg_lo > 1) {​
    int state = values[0] - avg;​
    if (state == 0) {​
      state = 1;​
    }​
    for (int i = 0; i < 500; i++) {​
      if (state < 0) {​
        if (values[i] >= avg_hi) {​
          cross += 1;​
          state = 1;​
        }​
      }​
      else if (state > 0) {​
        if (values[i] <= avg_lo) {​
          cross += 1;​
          state = -1;​
        }​
      }​
    }​
  }​

​  Serial.print("cross = ");​
  Serial.println(cross);​

  // часть 4 - частота
  int freq = (int)(1000.0 * (float)cross / (time2 - time1) / 2);​
  Serial.print("f=");​
  Serial.println(freq);

  display.setCursor(0, 40);​
  display.print(freq);​
  display.println("Hz");​

  // bonus! увеличение экрана
  if (avg_hi - avg_lo > 1) {​
    int voltage = analogRead(A1);​
    int zoom = 10*voltage/1024 + 1;​

​    display.setCursor(60, 40);​
    display.print("z=");​
    display.println(zoom);​

​    int zoomed = 500/zoom;​
    int x0, y0, x1, y1;​
    for (int i = 0; i < zoomed; i++) {​
      x1 = 80*(long)i/zoomed;​
      y1 = 40 - 32*((long)values[i]-value_min)/(value_max-value_min);​
      If (i > 0) {​
        display.drawLine(x0, y0, x1, y1, BLACK);​
      }​
      x0 = x1;​
      y0 = y1;​
    }​
  }​

  Serial.println();​
  Serial.println();​

​  display.display();​
  delay(1000);​
  
}
