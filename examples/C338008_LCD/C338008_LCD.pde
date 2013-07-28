/*
  Lcd 50530 Library
 
 This example demostrates the use of this library to drive M50530
 chip based Lcd's display, specially the Seiko C338008 and
 Samsung 0282A displays.
 This are 8x24 character displays.
 
 This sketch prints the first 196 ascii characters present in the
 scrren three times at different speeds. After this, it will print
 the alphabet character in different possitions of the screen. And
 at last, it will print the famous "hello world" message.
 
 The circuit:
 * LCD ioc1 pin to digital pin 10
 * LCD ioc2 pin to digital pin 11
 * LCD RW pin to digital pin 12
 * LCD Enable pin to digital pin 13
 * LCD D4 pin to digital pin 6
 * LCD D5 pin to digital pin 7
 * LCD D6 pin to digital pin 8
 * LCD D7 pin to digital pin 9
 * 4.7K ohm resistor from 12v to display's pin 14 (contrast)
 * Vcc to display's pin 15
 * Gnd to display's pin 1 and 16
 
 Library created in 13 Nov 2011
 by Mario Abajo

 This example code is in the public domain.
*/

#include <Lcd50530.h>

/*************************************************************/

// 8bits bus initialization
//Lcd50530 lcd(10,11,12,13,2,3,4,5,6,7,8,9);

// 4bits bus initialization
Lcd50530 lcd(10,11,12,13,6,7,8,9);

byte data0[8] = {
  B01110,
  B11011,
  B10001,
  B10001,
  B11111,
  B11111,
  B11111,
};

void setup() {
  lcd.begin();
  lcd.createChar(0, data0);
}

byte i=0;

void loop() {
  byte i,j;

  lcd.home();
  // 1 - continuous writing
  for (j=1;j<21;j+=5)
  {
    for (i=0;i<24*8;i++)
    {
      lcd.write(i);
      delay(100/(j*2));
    }
    delay(1000);
    lcd.clear();
  }
  
  // 2 - position writing
  j=0;
  for (i=0;i<24;i++)
  {
      lcd.setCursor(i,j);
      lcd.write(i+65);
      delay(100);
      if (i<7 || i>=14 && i<21)
        j++;
      else
        j--;
  }
  delay(1000);
  lcd.clear();

  // 3 - print hello world
  lcd.setCursor(5,4);
  lcd.print("Hello world! :)");
  delay(2000);

}

