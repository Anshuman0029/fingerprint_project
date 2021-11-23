
#include<LiquidCrystal.h>
LiquidCrystal lcd(8,9,10,11,12,13);
#include <Adafruit_Fingerprint.h>
#include <EEPROM.h>

#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
SoftwareSerial mySerial(2,3);
#else
#define mySerial Serial1
#endif


Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

int S1=4;
int S2=5;
int d1=6;
int d2=7;

int id;
int addr;

void setup()
{
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("  Door Security  ");
  lcd.setCursor(0,1);
  lcd.print("     System   ");  
  delay(3000);
  lcd.clear();  
  pinMode(S1,INPUT_PULLUP);
  pinMode(S2,INPUT_PULLUP);
  pinMode(d1,OUTPUT);
  Serial.begin(9600);    
  finger.begin(57600);
}


boolean flag;

void loop()                 
{
  getFingerprintID();
  delay(50);
  if(digitalRead(S1)==0)  
    { 
     for (int i = 0 ; i < EEPROM.length() ; i++) 
          { 
            id=EEPROM.read(i);            
            if (id==0)
               {
                addr=i;
                break;               
                }                          
             } 

      id=EEPROM.read(addr-1); 
      id++;         
      flag=getFingerprintEnroll();                
      if(flag)
         {
          EEPROM.write(addr,id);
          //Serial.println(id);   
          } 
           
    }
    
  if(digitalRead(S2)==0)  
     { 
      id=1;
      addr=0;
      finger.emptyDatabase();
      //Serial.println("Now database is empty :)");
      lcd.setCursor(0,0);
      lcd.print("Processing.........");
      lcd.setCursor(0,1);
      lcd.print("                 ");
      delay(2000); 
      lcd.setCursor(0,0);
      lcd.print("Now database is  ");
      lcd.setCursor(0,1);
      lcd.print("     Empty     ");
      delay(2000);
      for (int i = 0 ; i < EEPROM.length() ; i++) 
      {
        EEPROM.write(i, 0);
      } 
      EEPROM.write(addr,id);     
    }
    
        
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t getFingerprintEnroll() {
  
  int p = -1;
  lcd.setCursor(0,0);
  lcd.print("   Waiting for  ");
  lcd.setCursor(0,1);
  lcd.print("finger to enroll"); 
    
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    if(p==FINGERPRINT_OK) {      
      //Serial.println("Image taken");
      lcd.setCursor(0,0);
      lcd.print("  Image taken  ");
      lcd.setCursor(0,1);
      lcd.print("                 ");
      break; 
      delay(2000);        
    }
   }


  p = finger.image2Tz(1);
  if(p==FINGERPRINT_OK) { 
      ///Serial.println("Image converted");
      lcd.setCursor(0,0);
      lcd.print("Image converted  ");
      lcd.setCursor(0,1);
      lcd.print("                 ");
      delay(2000);       
    }

   lcd.setCursor(0,0);
   lcd.print("  Remove finger  ");
   lcd.setCursor(0,1);
   lcd.print("                 ");
   delay(2000); 

  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }  
   p = -1;
   lcd.setCursor(0,0);
   lcd.print("  Place same    ");
   lcd.setCursor(0,1);
   lcd.print("  finger again  ");
   delay(2000);
   
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    if(p==FINGERPRINT_OK)
    {
      //Serial.println("Image taken");
      lcd.setCursor(0,0);
      lcd.print("  Image taken  ");
      lcd.setCursor(0,1);
      lcd.print("                 ");      
      break;
      delay(2000); 
      }
     } 

  p = finger.image2Tz(2);  
  if(p==FINGERPRINT_OK)
  {
    //Serial.println("Image converted");
    lcd.setCursor(0,0);
    lcd.print("Image converted  ");
    lcd.setCursor(0,1);
    lcd.print("                 ");
    delay(2000);      
   }

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    //Serial.println("Prints matched!");
    lcd.setCursor(0,0);
    lcd.print("Prints matched!  ");
    lcd.setCursor(0,1);
    lcd.print("                 ");
    delay(2000);
  }  
  else if (p == FINGERPRINT_ENROLLMISMATCH) 
  {
    lcd.setCursor(0,0);
    lcd.print("Did not match  ");
    lcd.setCursor(0,1);
    lcd.print("                 ");
    delay(2000);
    return p;
  } 

  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK)
  {
    //Serial.println("Stored!");
      lcd.setCursor(0,0);
      lcd.print("Stored!           ");
      lcd.setCursor(0,1);
      lcd.print("                 ");      
      delay(2000);
      }
  return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
     // Serial.println("Image taken");
      lcd.setCursor(0,0);
      lcd.print("Image taken.......");
      lcd.setCursor(0,1);
      lcd.print("                 ");
      delay(2000);
      break;
    case FINGERPRINT_NOFINGER:
     // Serial.println("No finger detected");
      lcd.setCursor(0,0);
      lcd.print("Place Finger......");      
      lcd.setCursor(0,1);
      lcd.print("                 ");
      delay(2000);
      return p;
  }
 
  p = finger.image2Tz(); 
  p = finger.fingerSearch();
  
  if (p == FINGERPRINT_OK)
  {  
//////////////////////////////////////////////////////If Match Fingerprint Then //////////////////////////////////////////////     
    lcd.setCursor(0,0);
    lcd.print("Print match!......");      
    lcd.setCursor(0,1);
    lcd.print("                 ");
    Serial.print("Y"); 
    Serial.println();
    digitalWrite(d1,LOW);    
    digitalWrite(d2,HIGH);  
    delay(2000);
    digitalWrite(d1,HIGH);  
    digitalWrite(d2,LOW);  
    delay(2000);
    digitalWrite(d1,HIGH);  
    digitalWrite(d2,HIGH);  
    
    lcd.setCursor(0,0);
    lcd.print("Sending.........");      
    lcd.setCursor(0,1);
    lcd.print("                 ");
    delay(2000);
  //Serial.println("Found a print match!");
         
  } 
  else {
//////////////////////////////////////////////////////If Match  donot Fingerprint Then //////////////////////////////////////////////
    //Serial.println("Unknown error");
    lcd.setCursor(0,0);
    lcd.print("Did not match........");      
    lcd.setCursor(0,1);
    lcd.print("                 ");
    Serial.print("N");
    Serial.println();
    delay(2000);    
    return p;
  }  
  //Serial.println(finger.confidence);
  return finger.fingerID;
}
