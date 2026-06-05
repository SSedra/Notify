#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <MFRC522.h>
#include <SPI.h>
#include <cstdint>
#include "notify.h"


LiquidCrystal_I2C lcd(0x27, 20, 4);
MFRC522 rfid(5,2);

int nbDot=0;
unsigned long tmpDernierPrint=0;

void setup() {
  Serial.begin(115200);

  // init ecran lcd
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

  //badge creation
    //visiteur :Zandry
    byte idZandry[]={0xAA,0xBB,0xCC,0xDD};
    tBadge ZandryBdge=badgeCreer();
    badgeInit(&ZandryBdge,4,idZandry,temp,"Zandry",true,5);
    //proprio :Sahy Sedra
    byte idSahy[]={0x11,0x22,0x33,0x44};
    tBadge SahyBdge=badgeCreer();
    badgeInit(&SahyBdge,4,idSahy,owner,"Sahy Sedra",true,4);

    //admin
    byte idmin[]={0x01,0x02,0x03,0x04};
    tBadge BossBadge=badgeCreer();
    badgeInit(&BossBadge,4,idmin,admin,"boss",true,-1);

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
if(!rfid.PICC_ReadCardSerial()) {
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("ERREUR LECTURE CARTE");
    delay(1000);
    return;
  } 

  //init badge temporaire pour comparer dans reconnu 
  tBadge scan=badgeChercher(rfid.uid.uidByte,rfid.uid.size);

  //impression UID
    Serial.print("UID : ");

    for (byte i = 0; i < rfid.uid.size; i++) {
      Serial.print(rfid.uid.uidByte[i], HEX);
      Serial.print(" ");
    }

    Serial.println();
  
  if(scan!=nullptr)//si le badge scane fait partie est un enregistre
  {
    if(badgeActivite(scan))//si le badge est actif
    {
      lcd.clear();
      lcd.setCursor(3,1);
      lcd.print("Bienvenue !");
      lcd.setCursor(3,2);
      lcd.print(badgeNom(scan)); // On affiche son nom
      delay(1500);
      lcd.clear();

      if(badgeProfile(scan)!=admin)
      {
        lcd.setCursor(3,1);
        lcd.print("credit :");
        lcd.setCursor(3,2);
        lcd.print(badgeAccesRestant(scan)-1);
        delay(1500);
        lcd.clear();
      }
      

      //on donne l'accés au badge 
      badgeAcceder(scan);
    }
    else//le badge est off et/ou perime
    {
      lcd.clear();
    lcd.setCursor(3,1);
    lcd.print("Acces Refuse");
    delay(1500);
    lcd.clear();
    }
  } else {//si il ne figure pas dans la liste des enregistrées
    lcd.clear();
    lcd.setCursor(3,1);
    lcd.print("Non reconnue");
    delay(1500);
    lcd.clear();
  }

  //reinit pour une autre loop
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
