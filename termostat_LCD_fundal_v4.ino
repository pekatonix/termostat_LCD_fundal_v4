/*
THERMOSTAR with HUMIDITY CONTROL
ver.1m0 - 28.08.2015, Craiova - Romania, Europe, Earth
ver.1m4 - 29.08.2015
ver.1m4x 0 27.05.2018
niq_ro write this sketch using parts of other sketches :)
http://www.tehnic.go.ro
http://nicuflorica.blogspot.ro/
http://arduinotehniq.blogspot.com/
Introducere iluminat de fundal - https://nicuflorica.blogspot.com/2017/08/iluminat-de-fundal-progresiv-pentru.html
ver.1m5  - 25.10.2018, Targu Mures - Pekatonix
*/

#include <LiquidCrystal.h>  //this library is included in the Arduino IDE
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
/*                                    -------------------
                                      |  LCD  | Arduino |
                                      -------------------
 LCD RS pin to digital pin 12         |  RS   |   D12   |
 LCD Enable pin to digital pin 11     |  E    |   D11   |
 LCD D4 pin to digital pin 5          |  D4   |   D6    |
 LCD D5 pin to digital pin 4          |  D5   |   D4    |
 LCD D6 pin to digital pin 3          |  D6   |   D3    |
 LCD D7 pin to digital pin 2          |  D7   |   D2    |
 LCD R/W pin to ground                |  R/W  |   GND   |
                                      -------------------
*/

// http://arduino.cc/en/Reference/LiquidCrystalCreateChar
byte grad[8] = {
  B01100,
  B10010,
  B10010,
  B01100,
  B00000,
  B00000,
  B00000,
};


// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain
#include "DHT.h"
#define DHTPIN A0     // what pin we're connected to A0
// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11 
#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// if is just sensor:
// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

DHT dht(DHTPIN, DHTTYPE);

// define pins for relay or leds
#define temppeste A1
//#define umidpeste A2

// define variable

float te, t1, tes, dete;   // variable for temperature
int has, hass, dehas;        // variable for humiditiy

byte heat;  // is 1 for heater and 0 for cooler
byte dry;   // is 1 for dryer and 0 for wetter 

// part for menu: http://nicuflorica.blogspot.ro/2015/05/termostat-cu-afisaj-led.html
#define BUT1 6    // - switch
#define BUT2 7    // + switch
#define BUT3 8    // MENU switch

byte meniu = 0; // if MENIU = 0 is clasical 
                // if MENIU = 1 is for temperature set (tes)
                // if MENIU = 2 is for dt temperature (dete)
                // if MENIU = 3 is for humidity set (hass)
                // if MENIU = 4 is for dh humidity (dehas)
                // if MENIU = 5 is for HEAT/COOL
                // if MENIU = 6 id for DRY/WET

// http://tronixstuff.com/2011/03/16/tutorial-your-arduinos-inbuilt-eeprom/
#include <EEPROM.h>

// https://nicuflorica.blogspot.ro/2017/08/iluminat-de-fundal-progresiv-pentru.html
#define fundal 10   // led de fundal
int inceput = 0;  // timpul in secunde cand s-a apasat butonul
int actual = 0;   // timpul actual in secunde
int ultimaapasare;  // se memoreaza timpul cand se apasa ultima dat abutonul
int timplumina = 7; // timp cant ramane fundalul aprins
byte fundalminim = 15;
byte fundalmaxim = 255;
byte nivelfundal = 15;
byte treaptanivel = 5;  // treapta scadere iluminare de la maxim la minim
byte apasat = 1;  // consideram butonul neapasat


void setup()
{

// Initialize LCD display  
 lcd.begin(16, 2);
// create custom symbol
lcd.createChar(0, grad);

// Initialize DHT sensor
  dht.begin();

// define outputs
 pinMode(temppeste, OUTPUT);  
 //pinMode(umidpeste, OUTPUT);

// set the default state for outputs
  digitalWrite(temppeste, LOW);
  //digitalWrite(umidpeste, LOW);

// set push buttons for menu
  pinMode(BUT1, INPUT);
  pinMode(BUT2, INPUT);
  pinMode(BUT3, INPUT);
  
  digitalWrite(BUT1, HIGH); // pull-ups on
  digitalWrite(BUT2, HIGH);
  digitalWrite(BUT3, HIGH);

  lcd.setCursor(0,0);  
  lcd.print("thermostat");
  
  lcd.setCursor(0,1);  
  lcd.print("sketch de niq_ro");

delay(5000);
lcd.clear();

  lcd.setCursor(0,0);  
  lcd.print("modificat de");
  
  lcd.setCursor(0,1);  
  lcd.print("Pekatonix");
delay(5000);
lcd.clear();

pinMode(BUT3, INPUT);
digitalWrite(BUT3, HIGH);

analogWrite(fundal, nivelfundal);  // se aprinde lumina de fundal la minim

inceput = millis()/1000;  // memoram timpul cand incepe sa ruleze programul (in secunde)

/*
// preset value for temperature and humidity
tes = 28.9;   
dete = 0.2;
hass = 58;
dehas = 2;
heat = 1;  // is 1 for heater and 0 for cooler
dry = 1;   // is 1 for dryer and 0 for wetter 
*/

/*
// just first time... after must put commnent (//)
EEPROM.write(201,0);    // tset1
EEPROM.write(202,245);  // tset2
EEPROM.write(203,5);    // dt x 10
EEPROM.write(204,45);   // hass
EEPROM.write(205,5);    // dehas
EEPROM.write(206,1);    // is 1 for heater and 0 for cooler
EEPROM.write(207,1);    // is 1 for dryer and 0 for wetter 
 */
  
byte tset1 = EEPROM.read(201);
byte tset2 = EEPROM.read(202);
tes = 256 * tset1 + tset2;  // recover the number
tes = tes/10;
dete = EEPROM.read(203);
dete = dete/10;
hass = EEPROM.read(204);
dehas = EEPROM.read(205);
heat = EEPROM.read(206);
dry = EEPROM.read(207);  
}


void loop()   {

apasat = digitalRead(BUT2);  // verificam starea butonului

  if (apasat == 0)  // daca e apasat
    {
      nivelfundal = fundalmaxim;
      analogWrite(fundal, nivelfundal);  // se aprinde la maxim fundalul
      Serial.println("S-a apasat butonul .....");
      delay(50);     
      apasat = 1;  // consideram butonul neapasat
      ultimaapasare = millis()/1000;  // se memoreaza timpul cand s-a apasat ultima data butonul
    }

actual = millis()/1000;

if ((actual - ultimaapasare > timplumina) &&  (nivelfundal > fundalminim))  // daca a trecut timpul de aprindere lumina fundal la maxim si nu s-a stins la minim
{
  nivelfundal = nivelfundal - treaptanivel;   // nivel aprindere in scadere
  if (nivelfundal < fundalminim) nivelfundal = fundalminim; // nu trebuie sa scada valaorea sub minim
  analogWrite(fundal, nivelfundal);
  Serial.println(nivelfundal);
}

delay(100);

if (digitalRead(BUT3) == LOW) 
{ meniu = meniu + 1;
lcd.clear();
delay(250);
}
if (meniu >= 7) meniu = 0;

if (meniu == 0)
{
 if (digitalRead(BUT3) == LOW) 
  { meniu = 1;
  delay(250);
  }  
  
   // Reading temperature or humidity 
 has = dht.readHumidity();

 if (digitalRead(BUT3) == LOW) 
  { meniu = 1;
  delay(250);
  }  


te = dht.readTemperature(); 
delay(250);

 if (digitalRead(BUT3) == LOW) 
  { meniu = 1;
  delay(250);
  }  




// part from http://nicuflorica.blogspot.ro/2014/10/ceas-rtc-cu-ds1307-si-date-mediu-cu.html
 lcd.setCursor(0, 0);
  // lcd.print("t=");
 if (te < 0) 
 {t1=-te;}
 else t1=te;
  
    if ( t1 < 10)
   {
     lcd.print(" "); 
   }
   if (te>0) lcd.print("+"); 
   if (te==0) lcd.print(" "); 
   if (te<0) lcd.print("-");
   lcd.print(t1,1);
//   lcd.write(0b11011111);
   lcd.write(byte(0));
   lcd.print("C  ");

   lcd.setCursor(10, 0);
  // lcd.print("H=");
   lcd.print(has);
   lcd.print("%RH");


if (te > tes) 
 {
if (heat == 1) digitalWrite(temppeste, LOW); 
if (heat == 0) digitalWrite(temppeste, HIGH); 
   lcd.setCursor(0, 1);
//   lcd.print("OVER ");
lcd.print("H0T   ");
 } 
if (tes - dete > te)
  {
if (heat == 0) digitalWrite(temppeste, LOW); 
if (heat == 1) digitalWrite(temppeste, HIGH); 

   lcd.setCursor(0, 1);
//   lcd.print("BELOW");
      lcd.print("COLD ");
 } 
if ((te <= tes) and (tes - dete <= te))
 {
   lcd.setCursor(0, 1);
   lcd.print("OK !  ");
 } 


 if (has > hass) 
 {
//if (dry == 1) digitalWrite(umidpeste, HIGH); 
//if (dry == 0) digitalWrite(umidpeste, LOW); 

   lcd.setCursor(10, 1);
//   lcd.print("OVER ");
      lcd.print("WET ");
 } 
 if (has < hass - dehas) 
 {
//if (dry == 0) digitalWrite(umidpeste, HIGH); 
//if (dry == 1) digitalWrite(umidpeste, LOW); 
   lcd.setCursor(10, 1);
//   lcd.print("BELOW");
   lcd.print("DRY ");
 } 
 if ((has <= hass) and (has >= hass - dehas))
 {  lcd.setCursor(10, 1);
   lcd.print("OK !  ");
}

actual = millis()/1000;
if ((actual - ultimaapasare > timplumina) &&  (nivelfundal > fundalminim))  // daca a trecut timpul de aprindere lumina fundal la maxim si nu s-a stins la minim
{
  nivelfundal = nivelfundal - treaptanivel;   // nivel aprindere in scadere
  if (nivelfundal < fundalminim) nivelfundal = fundalminim; // nu trebuie sa scada valaorea sub minim
  analogWrite(fundal, nivelfundal);
  Serial.println(nivelfundal);
}
} // last line for MENIU = 0


if (meniu == 1) {
  while (meniu == 1) {
//   teset(tset);
   lcd.setCursor(0, 0);
   lcd.print("Temperature SET:");
   lcd.setCursor(0, 1);
//   lcd.print("+");
   lcd.print(tes,1);
//   lcd.write(0b11011111);
   lcd.write(byte(0));
   lcd.print("C  ");

   lcd.setCursor(8, 1);
   lcd.print("dt=");
   lcd.print(dete,1);
//   lcd.write(0b11011111);
   lcd.write(byte(0));
   lcd.print("C  ");


   if (digitalRead(BUT1) == LOW)
  { tes = tes - 0.1; 
  delay(250);
  }
   if (digitalRead(BUT2) == LOW)
  { tes = tes + 0.1;
  delay(250);
  }

int tes2 = tes*10;  
byte tset1 = tes2 / 256;
byte tset2 = tes2 - tset1 * 256;
EEPROM.write(201, tset1);  // partea intreaga
EEPROM.write(202, tset2);   // rest

  
  if (digitalRead(BUT3) == LOW) 
  { meniu = 2;
  delay(250);
  lcd.clear();
  }
//  delay(15);
     }
   delay (100);
}   // end loop for MENIU = 1


if (meniu == 2) {
 // if (tmax <= tmin) tmax = tmin + 10;
   while (meniu ==2) {
 //    maxim(dt/10); // this is number to diplay
 //    dete(dt); // this is number to diplay
   lcd.setCursor(0, 0);
   lcd.print("hyst.temp. SET:");
   lcd.setCursor(0, 1);
//   lcd.print("+");
   lcd.print(tes,1);
//   lcd.write(0b11011111);
   lcd.write(byte(0));
   lcd.print("C  ");

   lcd.setCursor(8, 1);
   lcd.print("dt=");
   lcd.print(dete,1);
//   lcd.write(0b11011111);
   lcd.write(byte(0));
   lcd.print("C  ");

    if (digitalRead(BUT1) == LOW) 
   { dete = dete - 0.1;
   delay(250);
  }
   if (digitalRead(BUT2) == LOW) 
   { dete = dete + 0.1;
   delay(250);
  }
   if (digitalRead(BUT3) == LOW) 
   { meniu = 3;
   delay(250);
   lcd.clear();
  }
//   delay(15);
if (dete < 0.1) dete = 0.1;
  EEPROM.write(203,dete*10);      
    }
}  // end loop for MENIU = 2


if (meniu == 3) {
  while (meniu == 3) {
//   teset(tset);
   lcd.setCursor(0, 0);
   lcd.print("Humidity SET:");
   lcd.setCursor(0, 1);
//   lcd.print("+");
   lcd.print(hass,1);
//   lcd.write(0b11011111);
//   lcd.write(byte(0));
   lcd.print("%RH  ");

   lcd.setCursor(7, 1);
   lcd.print("dh=");
   lcd.print(dehas);
//   lcd.write(0b11011111);
//   lcd.write(byte(0));
   lcd.print("%RH");


   if (digitalRead(BUT1) == LOW)
  { hass = hass - 1; 
  delay(250);
  }
   if (digitalRead(BUT2) == LOW)
  { hass = hass + 1;
  delay(250);
  }
  
  EEPROM.write(204,hass);      
  
  if (digitalRead(BUT3) == LOW) 
  { meniu = 4;
  delay(250);
  lcd.clear();
  }
//  delay(15);
     }
   delay (250);
}  // end loop for MENIU =3


if (meniu == 4) {
   while (meniu ==4) {
   lcd.setCursor(0, 0);
   lcd.print("hyst.humid. SET:");
   lcd.setCursor(0, 1);
//   lcd.print("+");
   lcd.print(hass);
//   lcd.write(0b11011111);
   lcd.write(byte(0));
   lcd.print("%RH");

   lcd.setCursor(7, 1);
   lcd.print("dh=");
   lcd.print(dehas);
//   lcd.write(0b11011111);
   lcd.write(byte(0));
   lcd.print("%RH");

 
   if (digitalRead(BUT1) == LOW) 
   { dehas = dehas - 1;
   delay(250);
  }
   if (digitalRead(BUT2) == LOW) 
   { dehas = dehas + 1;
   delay(250);
  }
  
    EEPROM.write(205,dehas);      
    
   if (digitalRead(BUT3) == LOW) 
   { meniu = 5;
   delay(250);
   lcd.clear();
  }
//   delay(15);
if (dehas < 1) dehas = 1;
    }
}  // end loop for MENIU = 4


if (meniu == 5) {
   while (meniu ==5) {
   lcd.setCursor(0, 0);
   lcd.print("thermostat type:");
   lcd.setCursor(0, 1);

if (heat == 1) 
{
   lcd.print("HEATER ");
}
if (heat == 0) 
{
   lcd.print("COOLER ");
}

   if (digitalRead(BUT1) == LOW) 
   { heat = 0;
   delay(250);
  }
   if (digitalRead(BUT2) == LOW) 
   { heat = 1;
   delay(250);
  }
  
    EEPROM.write(206,heat);      
  
   if (digitalRead(BUT3) == LOW) 
   { meniu = 6;
   delay(250);
   lcd.clear();
  }
  }
}   // end loop for end MENIU = 5


if (meniu == 6) {
   while (meniu ==6) {
   lcd.setCursor(0, 0);
   lcd.print("Humidity control");
   lcd.setCursor(0, 1);

if (dry == 1) 
{
   lcd.print("DRYER ");
}
if (dry == 0) 
{
   lcd.print("WETTER ");
}

   if (digitalRead(BUT1) == LOW) 
   { dry = 0;
   delay(250);
  }
   if (digitalRead(BUT2) == LOW) 
   { dry = 1;
   delay(250);
  }
  EEPROM.write(207,dry);     
  
   if (digitalRead(BUT3) == LOW) 
   { meniu = 7;
   delay(250);
   lcd.clear();
  }
  }
}   // end loop for end MENIU = 6

} // last line in main loop
