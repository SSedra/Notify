#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <MFRC522.h>
#include <SPI.h>


LiquidCrystal_I2C lcd(0x27, 20, 4);
MFRC522 rfid(5,2);

int nbDot=0;
unsigned long tmpDernierPrint=0;

void setup() {
  // init ecran lcd
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();

  lcd.setCursor(3,1);
  lcd.print("Bonjour !");
  delay(1500);
  lcd.clear();

  lcd.setCursor(3,1);
  lcd.print("launch rfid");
  delay(1500);

  // init capteur rfid
  SPI.begin();
  rfid.PCD_Init();
  lcd.clear();
  lcd.setCursor(3,1);
  rfid.PCD_AntennaOn();
  lcd.print("Device ready");
  delay(2000);
  lcd.clear();
}

void loop() {
  //animation d'attente 
  if (millis() - tmpDernierPrint >= 500) {
    tmpDernierPrint = millis();
    
    lcd.setCursor(2 + (nbDot * 4), 1);
    lcd.print(" * ");
    nbDot++;

    if (nbDot > 3) {
      // Efface uniquement la zone de l'animation pour éviter le clignotement
      lcd.setCursor(2, 1);
      lcd.clear();
      nbDot = 0;
    }
  }

  //check si il y a une carte ou pas 
  if (!rfid.PICC_IsNewCardPresent()) return ;

  //animation erreur lecture
  if(!rfid.PICC_ReadCardSerial())
  {
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("ERREUR LECTURE CARTE");
    delay(1000);
    return;
  } 

  //reinit pour une autre loop
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
