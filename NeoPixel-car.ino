#define LED_PIN    4      // куда подключена ардуина
#define LED_NUM    178    //  количество пикселей

#include          <Adafruit_NeoPixel.h>   // подключаем библиотеку
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_NUM, LED_PIN, NEO_GRB + NEO_KHZ800);  // создание обьекта ленты

#define LEFT_BLINKER    7   // Левый Поворотник +12 вольт
#define RIGHT_BLINKER   6  // Правый Поворотник +12 вольт
#define STOP_SIGNAL     9        // Стоп сигнал +12 вольт
#define REVERSE         8         // Задний ход +12 вольт
#define PARKING_LIGHTS  5           // Габариты +12 вольт


#define PIXELS_SPEED    1       // Столько пикселей включается одновременно
#define LED_COUNT_STRIP 30      // Количетсво пикселей центральной полоски (четное число)
#define RUNNING_PIXELS  15      // Количество пикселей бегущего блока при запуске (четное число)

#define BRIGHTNESS        100      // Яркость
#define BRIGHTNESS_SPEED  3      //  Cкорость уменьшения яркости (чем больше тем быстрее будет достигнута мин яркость)

#define DELAY                 1       // Задержка между отрисовкой каждого пикселя (на все режимы)
#define DELAY_PARKING_LIGHTS  500    // Задержка включения габаритов
#define DELAY_START    0    // Задержка эффекта при старте
#define DELAY_REVERSE  3    // Задержка бегущих полосок заднего хода
#define DELAY_FADE     8    // Задержка погашения заднего хода

#define DELAY_STOP_SIGNAL     500      //  Задержка переключнния на Стоп


#define WHITE    255, 180, 180    // Цвет белого при старте
#define ORANGE   255, 45, 0      //  Цвет Поворотника 
#define RED      255, 0, 0
#define BLACK    0, 0, 0

unsigned long timer_parking_lights = 0;

boolean start_flag = true;
boolean isRepeated_TurnSignal    = false;
boolean isRepeated_StopSignal_right    = false;
boolean isRepeated_StopSignal_left    = false;
boolean isRepeated_Reverse       = false;
boolean isRepeated_ParkingLights = false;

int leftI;
int leftJ;
int rightI;
int rightJ;


void clearVars() {
  leftI = 0;
  leftJ = 0;
  rightI = 0;
  rightJ = 0;
  isRepeated_TurnSignal = false;
  isRepeated_StopSignal_right = false;
  isRepeated_StopSignal_left = false;
  isRepeated_Reverse = false;
  isRepeated_ParkingLights = false;
}

void setup() {
  pinMode(LEFT_BLINKER, INPUT);   // Левый Поворотник +12 вольт
  pinMode(RIGHT_BLINKER, INPUT); // Правый Поворотник +12 вольт
  pinMode(STOP_SIGNAL, INPUT);         // Стоп сигнал +12 вольт
  pinMode(REVERSE, INPUT);              // Задний ход +12 вольт
  pinMode(PARKING_LIGHTS, INPUT);         // Габариты +12 вольт

  strip.begin();
  strip.show();
  strip.setBrightness(BRIGHTNESS);
}

void loop() {
  //***********************************************************************************************************************************************
    if ((digitalRead(PARKING_LIGHTS) == HIGH && !isRepeated_ParkingLights) || (digitalRead(PARKING_LIGHTS) == HIGH && (digitalRead(LEFT_BLINKER) || digitalRead(RIGHT_BLINKER)))) {
    if (start_flag) {
      start_effect();
      start_flag = false;
    }
    if ((millis() - timer_parking_lights > DELAY_PARKING_LIGHTS) && (digitalRead(REVERSE) == LOW && digitalRead(LEFT_BLINKER) == LOW
        && digitalRead(RIGHT_BLINKER) == LOW && digitalRead(STOP_SIGNAL) == LOW)) {
      timer_parking_lights = millis();
      parking_lights(0, LED_NUM);
      isRepeated_ParkingLights = true;
    }
  } else if (digitalRead(PARKING_LIGHTS) == LOW && isRepeated_ParkingLights) {
    end_effect();
    isRepeated_ParkingLights = false;
  }
  //***********************************************************************************************************************************************
  if (digitalRead(LEFT_BLINKER) == HIGH && digitalRead(RIGHT_BLINKER) == HIGH) {
    hazard_lights();
  }
  //***********************************************************************************************************************************************
  if (digitalRead(LEFT_BLINKER) == HIGH && digitalRead(RIGHT_BLINKER) == LOW) {
    left_blinker();
    if (digitalRead(STOP_SIGNAL) == HIGH && !isRepeated_StopSignal_left) {
      stop_signal((LED_NUM / 2) + (LED_COUNT_STRIP / 2), LED_NUM);
      isRepeated_StopSignal_left = true;
    } else if (digitalRead(STOP_SIGNAL) == LOW && isRepeated_StopSignal_left) {
      offInRange((LED_NUM / 2) + (LED_COUNT_STRIP / 2), LED_NUM);
      isRepeated_StopSignal_left = false;
    }
    if (digitalRead(PARKING_LIGHTS) == HIGH && digitalRead(STOP_SIGNAL) == LOW)
      onInRange(WHITE, (LED_NUM / 2) + (LED_COUNT_STRIP / 2), LED_NUM);
  }
  //***********************************************************************************************************************************************
  if (digitalRead(RIGHT_BLINKER) == HIGH && digitalRead(LEFT_BLINKER) == LOW) {
    right_blinker();
    if (digitalRead(STOP_SIGNAL) == HIGH && !isRepeated_StopSignal_right) {
      stop_signal(0, (LED_NUM / 2) - (LED_COUNT_STRIP / 2));
      isRepeated_StopSignal_right = true;
    } else if (digitalRead(STOP_SIGNAL) == LOW && isRepeated_StopSignal_right) {
      offInRange(0, (LED_NUM / 2) - (LED_COUNT_STRIP / 2));
      isRepeated_StopSignal_right = false;
    }
    if (digitalRead(PARKING_LIGHTS) == HIGH && digitalRead(STOP_SIGNAL) == LOW)
      onInRange(WHITE, 0, (LED_NUM / 2) - (LED_COUNT_STRIP / 2));
  }
  //***********************************************************************************************************************************************
  if (digitalRead(STOP_SIGNAL) == HIGH && (digitalRead(LEFT_BLINKER) == LOW && digitalRead(RIGHT_BLINKER) == LOW)) {
    stop_signal(0, LED_NUM);
    if (digitalRead(STOP_SIGNAL) == LOW) {
      dimmBrightness(2, RED, 1, 0, LED_NUM);
    }
  }
  //***********************************************************************************************************************************************
  if (digitalRead(REVERSE) == HIGH && (digitalRead(LEFT_BLINKER) == LOW && digitalRead(RIGHT_BLINKER) == LOW && digitalRead(STOP_SIGNAL) == LOW)) {
    reverse();
  }
  //***********************************************************************************************************************************************
}

void show () {
  timer_parking_lights = millis();
  delay(DELAY);
  strip.show();
}

void off() {
  for (int i = 0; i < LED_NUM; i++) {
    strip.setPixelColor(i, strip.Color(BLACK));
  }
  strip.show();
}

void offInRange(byte begin_, byte end_) {
  for (int i = begin_; i < end_; i++) {
    strip.setPixelColor(i, strip.Color(BLACK));
  }
  strip.show();
}

void onInRange(byte R, byte G, byte B, byte begin_, byte end_) {
  for (int i = begin_; i < end_; i++) {
    strip.setPixelColor(i, strip.Color(R, G, B));
  }
  strip.show();
}

void dimmColor(byte speed, byte R, byte G, byte B, byte _delay, byte _begin, byte _end) {
  strip.setBrightness(BRIGHTNESS);
  for (int i = 255; i >= 0; i -= speed) {
    for (int j = _begin; j < _end; j++) {
      strip.setPixelColor(j, strip.Color(R, G, B));
    }
    delay(_delay);
    strip.show();
  }
  offInRange(_begin, _end);
}
void dimmBrightness(byte speed, byte R, byte G, byte B, byte _delay, byte _begin, byte _end) {
  for (int i = BRIGHTNESS - 1; i >= 0; i -= speed) {
    strip.setBrightness(i);
    for (int j = _begin; j < _end; j++) {
      strip.setPixelColor(j, strip.Color(R, G, B));
    }
    delay(_delay);
    strip.show();
  }
  offInRange(_begin, _end);
  strip.setBrightness(BRIGHTNESS);
}

void show_strip() {
  for (int i = 0; i < LED_COUNT_STRIP / 2 + 2; i += PIXELS_SPEED) {
    for (int j = 0; j < i; j++) {
      strip.setPixelColor(LED_NUM / 2 - j, strip.Color(ORANGE));
      strip.setPixelColor(LED_NUM / 2 - 1 + j, strip.Color(ORANGE));
    }
    strip.show();
  }
}

void show_left() {
  for (int i = 0; i < LED_COUNT_STRIP / 2 + 2; i += PIXELS_SPEED) {
    for (int j = 0; j < i; j++) {
      strip.setPixelColor(LED_NUM / 2 - j, strip.Color(ORANGE));
    }
    for (int j = LED_NUM / 2; j < 0; j--) {
      strip.setPixelColor((LED_NUM / 2) - (LED_COUNT_STRIP / 2) - j, strip.Color(BLACK));
    }
    strip.show();
  }
}

void show_right() {
  for (int i = 0; i < LED_COUNT_STRIP / 2 + 2; i += PIXELS_SPEED) {
    for (int j = 0; j < i; j++) {
      strip.setPixelColor(LED_NUM / 2 - 1 + j, strip.Color(ORANGE));
    }
    for (int j = 0; j < LED_NUM; j++) {
      strip.setPixelColor((LED_NUM / 2) + (LED_COUNT_STRIP / 2) + j, strip.Color(BLACK));
    }
    strip.show();
  }
}

void left_blinker() {
  strip.setBrightness(BRIGHTNESS);
  if (!isRepeated_TurnSignal) {
    show_strip();
    for (; leftI < LED_COUNT_STRIP / 2 + 2; leftI += PIXELS_SPEED) {
      for (; leftJ < leftI; leftJ++) {
        strip.setPixelColor(LED_NUM / 2 - 1 + leftJ, strip.Color(ORANGE));
      }
      show ();
    }
  }
  for (; leftI < LED_NUM / 2 + 2 + RUNNING_PIXELS; leftI += PIXELS_SPEED) {
    for (; leftJ < leftI; leftJ++) {
      strip.setPixelColor(LED_NUM / 2 - leftJ, strip.Color(ORANGE));
      if (leftJ > LED_COUNT_STRIP / 2 && leftJ > RUNNING_PIXELS) {
        strip.setPixelColor(LED_NUM / 2 - (leftJ - RUNNING_PIXELS), strip.Color(BLACK));
      }
    }
    show();
  }
  for (leftI = 0; leftI < LED_COUNT_STRIP / 2 + 2; leftI += PIXELS_SPEED) {
    for (leftJ = 0; leftJ < leftI; leftJ++) {
      strip.setPixelColor(LED_NUM / 2 - leftJ, strip.Color(ORANGE));
    }
    show ();
  }
  isRepeated_ParkingLights = false;
  isRepeated_TurnSignal = true;
}

void right_blinker() {
  strip.setBrightness(BRIGHTNESS);
  if (!isRepeated_TurnSignal) {
    show_strip();
    for (; rightI < LED_COUNT_STRIP / 2 + 2; rightI += PIXELS_SPEED) {
      for (; rightJ < rightI; rightJ++) {
        strip.setPixelColor(LED_NUM / 2 - rightJ, strip.Color(ORANGE));
      }
      show ();
    }
  }
  for (; rightI < LED_NUM / 2 + 2 + RUNNING_PIXELS; rightI += PIXELS_SPEED) {
    for (; rightJ < rightI; rightJ++) {
      strip.setPixelColor(LED_NUM / 2 + rightJ, strip.Color(ORANGE));
      if (rightJ >= LED_COUNT_STRIP / 2 && rightJ >= RUNNING_PIXELS)
        strip.setPixelColor(LED_NUM / 2 + (rightJ - RUNNING_PIXELS), strip.Color(BLACK));
    }
    show();
  }
  for (rightI = 0; rightI < LED_COUNT_STRIP / 2 + 2; rightI += PIXELS_SPEED) {
    for (rightJ = 0; rightJ < rightI; rightJ++) {
      strip.setPixelColor(LED_NUM / 2 - 1 + rightJ, strip.Color(ORANGE));
    }
    show ();
  }
  rightI -= 1;
  rightJ -= 1;
  isRepeated_ParkingLights = false;
  isRepeated_TurnSignal = true;
}

void hazard_lights() {
  strip.setBrightness(BRIGHTNESS);
  for (int i = 0; i < LED_NUM / 2 + 2 + RUNNING_PIXELS; i += PIXELS_SPEED) {
    for (int j = 0; j < i; j++) {
      strip.setPixelColor(LED_NUM / 2 + j, strip.Color(ORANGE));
      strip.setPixelColor(LED_NUM / 2 - 1 - j, strip.Color(ORANGE));
      if (j >= LED_COUNT_STRIP && j >= RUNNING_PIXELS) {
        strip.setPixelColor(LED_NUM / 2 + j - RUNNING_PIXELS, strip.Color(BLACK));
        strip.setPixelColor(LED_NUM / 2 - 1 - j + RUNNING_PIXELS, strip.Color(BLACK));
      }
    }
    show();
  }
  isRepeated_ParkingLights = false;
  isRepeated_TurnSignal = true;
}

void reverse() {
  strip.setBrightness(BRIGHTNESS);
  for (int i = 0; i < LED_NUM / 2 + 2 ; i += PIXELS_SPEED) {
    for (int j = 0; j < i; j++) {
      strip.setPixelColor(LED_NUM / 2 - 2 - j, strip.Color(WHITE));
      strip.setPixelColor(LED_NUM / 2 - 1 + j, strip.Color(WHITE));
    }
    delay(DELAY_REVERSE);
    strip.show();
  }
  for (int i = BRIGHTNESS - 1; i >= 0; i -= BRIGHTNESS_SPEED) {
    strip.setBrightness(i);
    for (int j = 0; j <= LED_NUM; j++) {
      strip.setPixelColor(j, strip.Color(WHITE));
    }
    delay(DELAY_FADE);
    strip.show();
  }
  off();
  clearVars();
  isRepeated_ParkingLights = false;
  isRepeated_TurnSignal = false;
}

void parking_lights(byte _begin, byte _end) {
  strip.setBrightness(BRIGHTNESS);
  for (int i = _begin; i < _end; i++) {
    strip.setPixelColor(i, strip.Color(WHITE));
  }
  strip.show();
  isRepeated_ParkingLights = false;
  isRepeated_TurnSignal = false;
}

void stop_signal(byte _begin, byte _end) {
  strip.setBrightness(BRIGHTNESS);
  for (int i = _begin; i < _end; i++) {
    strip.setPixelColor(i, strip.Color(RED));
  }
  strip.show();
  isRepeated_ParkingLights = false;
  isRepeated_TurnSignal = false;
}

void start_effect() {
  strip.setBrightness(BRIGHTNESS);
  for (int i = 0; i < LED_NUM / 2 + RUNNING_PIXELS; i += PIXELS_SPEED) {
    for (int j = 0; j < i; j++) {
      strip.setPixelColor(LED_NUM / 2 + j, strip.Color(WHITE));
      strip.setPixelColor(LED_NUM / 2 - 1 - j, strip.Color(WHITE));
      if (j >= RUNNING_PIXELS) {
        strip.setPixelColor(LED_NUM / 2 + (j - RUNNING_PIXELS), strip.Color(BLACK));
        strip.setPixelColor(LED_NUM / 2 - 1 - (j - RUNNING_PIXELS), strip.Color(BLACK));
      }
    }
    delay(DELAY_START);
    strip.show();
  }

  for (int i = 0; i < LED_NUM / 2 + RUNNING_PIXELS + 1; i += PIXELS_SPEED) {
    for (int j = 0; j < i; j++) {
      if (i >= LED_NUM / 2 + 1) {
        strip.setPixelColor(i - RUNNING_PIXELS - 1, strip.Color(BLACK));
        strip.setPixelColor(LED_NUM - i + RUNNING_PIXELS, strip.Color(BLACK));
      } else {
        strip.setPixelColor(j, strip.Color(WHITE));
        strip.setPixelColor(LED_NUM - j, strip.Color(WHITE));

        strip.setPixelColor(LED_NUM - 1 - j + RUNNING_PIXELS, strip.Color(BLACK));
        strip.setPixelColor(j - RUNNING_PIXELS, strip.Color(BLACK));
      }
    }
    delay(DELAY_START);
    strip.show();
  }

  for (int i = 0; i < LED_NUM / 2 + 2 ; i += PIXELS_SPEED) {
    for (int j = 0; j < i; j++) {
      strip.setPixelColor(LED_NUM / 2 - 2 - j, strip.Color(WHITE));
      strip.setPixelColor(LED_NUM / 2 - 1 + j, strip.Color(WHITE));
    }
    delay(DELAY_START);
    strip.show();
  }
}

void end_effect() {
  strip.setBrightness(BRIGHTNESS);
  for (int i = 0; i < LED_NUM / 2 + 2 ; i += PIXELS_SPEED) {
    for (int j = 0; j < i; j++) {
      strip.setPixelColor(j, strip.Color(BLACK));
      strip.setPixelColor(LED_NUM - j, strip.Color(BLACK));
    }
    show();
  }

  for (int i = 0; i < LED_NUM / 2 + RUNNING_PIXELS; i += PIXELS_SPEED) {
    for (int j = 0; j < i; j++) {
      strip.setPixelColor(LED_NUM / 2 + j, strip.Color(WHITE));
      strip.setPixelColor(LED_NUM / 2 - 1 - j, strip.Color(WHITE));
      if (j >= RUNNING_PIXELS) {
        strip.setPixelColor(LED_NUM / 2 + (j - RUNNING_PIXELS), strip.Color(BLACK));
        strip.setPixelColor(LED_NUM / 2 - 1 - (j - RUNNING_PIXELS), strip.Color(BLACK));
      }
    }
    show();
  }

  for (int i = 0; i < LED_NUM / 2 + RUNNING_PIXELS + 1; i += PIXELS_SPEED) {
    for (int j = 0; j < i; j++) {
      if (i >= LED_NUM / 2 + 1) {
        strip.setPixelColor(i - RUNNING_PIXELS - 1, strip.Color(BLACK));
        strip.setPixelColor(LED_NUM - i + RUNNING_PIXELS, strip.Color(BLACK));
      } else {
        strip.setPixelColor(j, strip.Color(WHITE));
        strip.setPixelColor(LED_NUM - j, strip.Color(WHITE));

        strip.setPixelColor(LED_NUM - 1 - j + RUNNING_PIXELS, strip.Color(BLACK));
        strip.setPixelColor(j - RUNNING_PIXELS, strip.Color(BLACK));
      }
    }
    show();
  }

}
