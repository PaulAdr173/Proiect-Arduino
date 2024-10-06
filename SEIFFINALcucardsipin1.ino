#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Servo.h>

// Define keypad
const byte KEYPAD_ROWS = 4;
const byte KEYPAD_COLS = 3;
byte rowPins[KEYPAD_ROWS] = {5, 4, 3, 2};
byte colPins[KEYPAD_COLS] = {A3, A2, A1};
char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

// Define LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Define servo
Servo myServo;

// Define RFID-RC522 pins
#define RST_PIN         9          
#define SS_PIN          10  
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Define master password
const char masterPassword[] = "1234";

void setup() {
  lcd.begin();
  lcd.print("Enter Password:");
  myServo.attach(6);

  SPI.begin();      
  mfrc522.PCD_Init();  
}

void loop() {
  char key = keypad.getKey();

  if (key != NO_KEY) {
    lcd.setCursor(0, 1);
    lcd.print("*");

    static char password[5];
    static int index = 0;
    password[index] = key;
    index++;

    if (index == 4) {
      password[index] = '\0'; // Null-terminate the string

      if (strcmp(password, masterPassword) == 0) {
        lcd.clear();
        lcd.print("Access Granted");
        myServo.write(90);
        delay(2000);
        myServo.write(0);
      } else {
        lcd.clear();
        lcd.print("Access Denied");
      }

      lcd.clear();
      lcd.print("Enter Password:");
      index = 0;
    }
  }

  // Check for RFID card
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    lcd.clear();
    lcd.print("RFID Card Detected");
    delay(1000);
    lcd.clear();
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
  }
}
