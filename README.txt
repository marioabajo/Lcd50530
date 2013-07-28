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

 This example code is in the public domain
