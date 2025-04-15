#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <avr/wdt.h>  // Include the Watchdog Timer library for AVR-based Arduinos

#define RST_PIN 9
#define SS_PIN 10
int RelayPin = 6;
const int buzzer = 7; // buzzer to Arduino pin 7

byte readCard[4];
String MasterTag = "6958182"; // REPLACE this Tag ID with your Tag ID
String tagID = "";

// Create instances
MFRC522 mfrc522(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 20, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

// Lock state: false = locked, true = unlocked
bool lockState = false;

int scanCount = 0;  // Track the number of scans

void setup() {
  pinMode(buzzer, OUTPUT); // Set buzzer - pin 7 as an output
  pinMode(RelayPin, OUTPUT); // Set the relay pin (for solenoid lock)
  SPI.begin(); // SPI bus
  mfrc522.PCD_Init(); // Initialize MFRC522
  lcd.init(); // Initialize the LCD
  lcd.backlight();
  lcd.print("RFID Access Control");
  delay(2000); // Show welcome message
  lcd.clear();
  lcd.print("Scan your card...");

  // Initialize the Watchdog Timer with a 2-second timeout
  wdt_enable(WDTO_2S);  // Watchdog timer will reset if not reset within 2 seconds
}

void loop() {
  if (getID()) {
    lcd.clear();
    lcd.setCursor(0, 0);

    if (tagID == MasterTag) {
      // If the master tag is scanned, unlock the lock and set lockState to true
      if (lockState == false) {
        lcd.print("Lock Opened!"); // Show lock open message
        digitalWrite(RelayPin, HIGH); // Unlock the solenoid lock
        tone(buzzer, 1500); // Success sound
        delay(500);
        noTone(buzzer);
        lockState = true; // Update lock state to unlocked

        delay(3000); // Keep the lock open for 3 seconds

      // After 3 seconds, close the lock
      digitalWrite(RelayPin, LOW); // Lock the solenoid lock
      lcd.clear();
      lcd.print("Lock Closed!");
      delay(1000); // Show the lock closed message for 1 second
      lockState = false; // Update lock state to locked
      }
    } else {
      // For non-master tags, show access denied message
      lcd.print("Access Denied!");
      tone(buzzer, 1000); // Denied sound
      delay(500);
      noTone(buzzer);
    }

    lcd.setCursor(0, 1);
    lcd.print("ID: ");
    lcd.print(tagID);

    delay(2000); // Show the result for 2 seconds
    lcd.clear();
    lcd.print("Scan your card...");

    // Increment the scan count and check if it's time to reset
    scanCount++;
    if (scanCount >= 5) {  // Reset after 5 scans (change condition as needed)
      resetSystem();
    }
  }
}

// Read new tag if available
boolean getID() {
  if (!mfrc522.PICC_IsNewCardPresent()) { // Check if a new card is present
    return false;
  }

  if (!mfrc522.PICC_ReadCardSerial()) { // Check if the card can be read
    return false;
  }

  tagID = "";
  for (uint8_t i = 0; i < 4; i++) { // The MIFARE PICCs we use have 4 byte UID
    tagID.concat(String(mfrc522.uid.uidByte[i], HEX)); // Convert to string
  }
  tagID.toUpperCase(); // Ensure all characters are uppercase
  mfrc522.PICC_HaltA(); // Halt the PICC
  return true;
}

// Function to trigger a reset using the Watchdog Timer
void resetSystem() {
  lcd.clear();
  lcd.print("Resetting system...");

  // Trigger the watchdog reset
  wdt_reset();  // Reset the watchdog timer before timeout
  wdt_enable(WDTO_2S);  // Set the timeout period again
  while (true);  // Stay in this loop until the Watchdog Timer resets the system
}
