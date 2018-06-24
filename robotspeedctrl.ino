#include <Encoder.h>
#include <LiquidCrystal.h>

const int rs = A4, en = A5, d4 = A0, d5 = A1, d6 = A2, d7 = A3;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

Encoder enc(0, 1);
long q = -999;

const int encoderSwitchPin = 2;
unsigned long encoderSwitchPressed;
unsigned long encoderSwitchReleased;
bool encoderSwitchStateChange = false;
void encoderSwitchISR() {
  int state = digitalRead(encoderSwitchPin);
  if (state == HIGH) {
    encoderSwitchReleased = millis();
  } else {
    encoderSwitchPressed = millis();
  }

  encoderSwitchStateChange = true;
}

void setup() {
  lcd.begin(16, 2);
  pinMode(encoderSwitchPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(encoderSwitchPin), encoderSwitchISR, CHANGE);
}

void loop() {
  long newQ;

  newQ = enc.read();
  if (newQ != q) {
    q = newQ;
    lcd.setCursor(0, 1);
    lcd.print("     ");
    lcd.setCursor(0, 1);
    lcd.print("Q=");
    lcd.print(q);
  }

  if (encoderSwitchStateChange) {
    encoderSwitchStateChange = false;
    long millisSincePressed = encoderSwitchReleased - encoderSwitchPressed;
    if (millisSincePressed > 0) {
      lcd.setCursor(0, 0);
      lcd.print("Released: ");
      lcd.print(millisSincePressed);
    } else {
      lcd.setCursor(0, 0);
      lcd.print("                "); // blank the line
      lcd.setCursor(0, 0);
      lcd.print("Pressed");
    }
  }
}

