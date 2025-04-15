#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define RST_PIN 9
#define SS_PIN 10
int RelayPin = 6;
const int buzzer = 7; //buzzer to arduino pin 7

byte readCard[4];
//String MasterTag = "3ECD72C";	// REPLACE this Tag ID with your Tag ID!!!
String MasterTag = "6958182";	// REPLACE this Tag ID with your Tag ID!!!
String tagID = "";

// Create instances
MFRC522 mfrc522(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27,20,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup() 
{
  // Initiating
  pinMode(buzzer, OUTPUT); // Set buzzer - pin 9 as an output
  pinMode(RelayPin, OUTPUT);
  SPI.begin(); // SPI bus
  mfrc522.PCD_Init(); // MFRC522
   lcd.init();                      // initialize the lcd 
  // Print a message to the LCD.
  lcd.backlight();
lcd.print("Test LCD");
}

void loop() 
{
  
  //Wait until new tag is available
  while (getID()) 
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    
    if (tagID == MasterTag) 
    {
      
      lcd.print(" Access Granted!");
      digitalWrite(RelayPin, HIGH);
      tone(buzzer, 1500);
	      delay(0500);
        noTone(buzzer);
    }
    else
    {
      lcd.print(" Access Denied!");
        tone(buzzer, 0200);
	      delay(0500);
        noTone(buzzer);

    }
    
      lcd.setCursor(0, 1);
      lcd.print(" ID : ");
      lcd.print(tagID);
      
    delay(2000);

    lcd.clear();
    digitalWrite(RelayPin, LOW);
    lcd.print(" Access Control ");
    lcd.setCursor(0, 1);
    lcd.print("Scan Your Card>>");
  }
}

//Read new tag if available
boolean getID() 
{
  // Getting ready for Reading PICCs
  if ( ! mfrc522.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
  return false;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) { //Since a PICC placed get Serial and continue
  return false;
  }
  tagID = "";
  for ( uint8_t i = 0; i < 4; i++) { // The MIFARE PICCs that we use have 4 byte UID
  //readCard[i] = mfrc522.uid.uidByte[i];
  tagID.concat(String(mfrc522.uid.uidByte[i], HEX)); // Adds the 4 bytes in a single String variable
  }
  tagID.toUpperCase();
  mfrc522.PICC_HaltA(); // Stop reading
  return true;
}