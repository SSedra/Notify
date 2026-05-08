#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <MFRC522.h>
#include <SPI.h>
#include <cstdint>


LiquidCrystal_I2C lcd(0x27, 20, 4);
MFRC522 rfid(5,2);

int nbDot=0;
unsigned long tmpDernierPrint=0;

enum Profile{admin ,owner,temp};

typedef struct 
{
  byte UID[7];
  byte lnUID;  
  Profile profile;
  char nomUtilisateur[35];
  bool activite = false;
  int nbAcces = 0;
} badge;

badge reconnue[10];

static void badgeInit(badge tabBadge[], int indiceTabBadge,byte lnUID,byte UID[7],Profile profile,const char nomUtilisateur[35],bool activite,int nbAcces)
{
  tabBadge[indiceTabBadge].lnUID=lnUID;

  for(int i=0;i<lnUID;i++)
    tabBadge[indiceTabBadge].UID[i]=UID[i];

  tabBadge[indiceTabBadge].profile=profile;
  
  for(int i=0;nomUtilisateur[i]!='\0' && i<34;i++)
    tabBadge[indiceTabBadge].nomUtilisateur[i]=nomUtilisateur[i];
  tabBadge[indiceTabBadge].nomUtilisateur[34]='\0';

  tabBadge[indiceTabBadge].activite=activite;

  tabBadge[indiceTabBadge].nbAcces=nbAcces;

}

static int badgeCheck(badge tabBadge[], int lnTabBadge, badge badgeACheke)
{
  for(int i = 0; i < lnTabBadge; i++)
  {
    if(tabBadge[i].lnUID == badgeACheke.lnUID)
    {
      if(memcmp(tabBadge[i].UID,
                badgeACheke.UID,
                badgeACheke.lnUID) == 0)
      {
        if(tabBadge[i].activite)
          return 0;
      }
    }
  }

  return 1;
}

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

  //badge reconnue
    //Sahy Sedra
    byte idSahy[] = {0x04, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66}; // erreur propable : uid probalement non reconnu
    badgeInit(reconnue,0,7,idSahy,temp,"Sahy Sedra",true,3);

    //adlmin
    byte idmin[]={0x01,0x02,0x03,0x04};// erreur propable : uid non reconnu 
    badgeInit(reconnue,1,4,idmin,admin,"boss",true,-1);

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
  badge scan; 
  scan.lnUID=rfid.uid.size;
  for(int i=0; i<scan.lnUID; i++) {
    scan.UID[i] = rfid.uid.uidByte[i];
  }

  
  if (badgeCheck(reconnue, 10, scan)==0) {
    lcd.clear();
    lcd.setCursor(3,1);
    lcd.print("Bienvenue !");
    delay(1500);
    lcd.clear();

    //impression UID
    Serial.print("UID : ");

    for (byte i = 0; i < rfid.uid.size; i++) {
      Serial.print(rfid.uid.uidByte[i], HEX);
      Serial.print(" ");
    }

    Serial.println();

  } else {
    lcd.clear();
    lcd.setCursor(3,1);
    lcd.print("Acces Refuse");
    delay(1500);
    lcd.clear();
  }

  //reinit pour une autre loop
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
