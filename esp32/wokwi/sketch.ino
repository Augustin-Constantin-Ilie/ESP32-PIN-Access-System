#include <Keypad.h>
#include <TM1637Display.h>
#include <ESP32Servo.h>

// ---------- Settings ----------
const char PIN_CODE[] = "1234";          // correct PIN (4 digits)
const int  PIN_LEN    = 4;
const int  SERVO_CLOSED = 0;             // angle when closed
const int  SERVO_OPEN   = 90;            // angle when open
const unsigned long OPEN_TIME_MS = 3000; // ms, then the servo closes again

// ---------- Pins ----------
#define TM_CLK    22
#define TM_DIO    21
#define SERVO_PIN 23

// ---------- Keypad ----------
const byte ROWS = 4, COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {13, 12, 14, 27};
byte colPins[COLS] = {26, 25, 33, 32};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

TM1637Display display(TM_CLK, TM_DIO);
Servo servo;

String input = "";

// Segment patterns for text
const uint8_t SEG_OPEN[] = {0x3F, 0x73, 0x79, 0x54};  // OPEn
const uint8_t SEG_ERR[]  = {0x79, 0x50, 0x50, 0x00};  // Err

void showInput() {
  uint8_t seg[4] = {0, 0, 0, 0};
  for (int i = 0; i < (int)input.length() && i < 4; i++) {
    seg[i] = display.encodeDigit(input[i] - '0');
  }
  display.setSegments(seg);
}

void setup() {
  display.setBrightness(7);
  display.clear();
  servo.setPeriodHertz(50);
  servo.attach(SERVO_PIN, 500, 2400);
  servo.write(SERVO_CLOSED);
  showInput();
}

void loop() {
  char key = keypad.getKey();
  if (!key) return;

  if (key >= '0' && key <= '9') {
    if (input.length() < PIN_LEN) {
      input += key;
      showInput();
    }
    if (input.length() == PIN_LEN) {
      delay(400);                       // keep the last digit visible briefly
      if (input == PIN_CODE) {
        display.setSegments(SEG_OPEN);
        servo.write(SERVO_OPEN);
        delay(OPEN_TIME_MS);
        servo.write(SERVO_CLOSED);
      } else {
        display.setSegments(SEG_ERR);
        delay(1500);
      }
      input = "";
      showInput();
    }
  }
  else if (key == '*') {                // * = delete everything
    input = "";
    showInput();
  }
  else if (key == '#') {                // # = delete last digit
    if (input.length() > 0) input.remove(input.length() - 1);
    showInput();
  }
}
