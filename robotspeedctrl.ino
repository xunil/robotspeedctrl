#include <Encoder.h>
#include <LiquidCrystal.h>

const int rs = A4, en = A5, d4 = A0, d5 = A1, d6 = A2, d7 = A3;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int encoderCWPin = 0, encoderCCWPin = 1;
Encoder enc(encoderCWPin, encoderCCWPin);
long q = -999;

const int leftMotorPWMPin = 5, rightMotorPWMPin = 6;

struct RobotSettings {
  unsigned char signature[5];
  unsigned int fastCoeff;
  unsigned int slowCoeff;
  unsigned int jogOutsideCoeff;
  unsigned int jogInsideCoeff;
  unsigned int turnOutsideCoeff;
  unsigned int turnInsideCoeff;
} robotSettings;

union eepromDefaults {
  RobotSettings defaultSettings = {
    /* signature[] = */ { 'R', 'O', 'B', 'O', 'T' }, // 5 bytes
    /* fastCoeff = */ 100,                           // 2 bytes
    /* slowCoeff = */ 50,                            // 2 bytes
    /* jogOutsideCoeff = */ 85,                      // 2 bytes
    /* jogInsideCoeff = */ 15,                       // 2 bytes
    /* turnOutsideCoeff = */ 65,                     // 2 bytes
    /* turnInsideCoeff = */ 35                       // 2 bytes
  };
  char bytes[17];
};

int motorSpeedPWM = -999;

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



// Check for signature string at the beginning of the EEPROM ("ROBOT")
// If not found, erase the EEPROM with the signature and default values
void setupEEPROM() {
}

void setup() {
  lcd.begin(16, 2);
  pinMode(encoderSwitchPin, INPUT);
  pinMode(leftMotorPWMPin, OUTPUT);
  pinMode(rightMotorPWMPin, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(encoderSwitchPin), encoderSwitchISR, CHANGE);
}

void loop() {
  long newEncoderReading;

  newEncoderReading = enc.read();
  if (newEncoderReading > 255) {
    newEncoderReading = 255;
    enc.write(255);
  } else if (newEncoderReading < 0) {
    newEncoderReading = 0;
    enc.write(0);
  }

  if (newEncoderReading != motorSpeedPWM) {
    motorSpeedPWM = newEncoderReading;
    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print("PWM value: ");
    lcd.print(motorSpeedPWM);
    analogWrite(leftMotorPWMPin, motorSpeedPWM);
    analogWrite(rightMotorPWMPin, motorSpeedPWM);
  }

  if (encoderSwitchStateChange) {
    encoderSwitchStateChange = false;
    long millisSincePressed = encoderSwitchReleased - encoderSwitchPressed;
    if (millisSincePressed > 0) {
      // Encoder push switch released
      // lcd.setCursor(0, 0);
      // lcd.print("Released: ");
      // lcd.print(millisSincePressed);
    } else {
      // Encoder push switch pressed
      lcd.setCursor(0, 0);
      lcd.print("                "); // blank the line
      lcd.setCursor(0, 0);
      lcd.print("Motors stopped");
      motorSpeedPWM = 0;
      enc.write(0);
      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print("PWM value: ");
      lcd.print(motorSpeedPWM);
      analogWrite(leftMotorPWMPin, motorSpeedPWM);
      analogWrite(rightMotorPWMPin, motorSpeedPWM);
    }
  }
}

