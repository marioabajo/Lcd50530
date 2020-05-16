/*  
   Seiko C338008  /  Samsung 0282A display library
   ( based on the Mitsubishi M50530 controller )
   
   by Mario Abajo (2011)

   Changelog:
   2012/5/6 - Adaptation to Arduino 1.0, and api ampliation
*/

#include "Lcd50530.h"
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

/* Instructions accepted by the display
Functions impl.  Ini - M - Description
                 ----------------------
getCursor        RC  - 3 - Read cursor address
setCursor        WC  - 3 - Write cursor address
                 RS  - 2 - Read display start address
                 WS  - 2 - Write display start address
read             RD  - 1 - Read RAM data
write            WD  - 1 - Write RAM data
readflags        RB  - 0 - Read busy flag and other flags
SetFunctionMode  SF  - 0 - Set function mode
SetEntryMode     SE  - 0 - Set entry mode
SetDisplayMode   SD  - 0 - Set display mode
scrollDisplay*   MA  - 0 - Shift cursor/display start address
underline        SU  - 0 - Set underline mode
setUnderline     WU  - 0 - Write RAM underline bit
blinkFreq        SB  - 0 - Set blinking frequency
home             MH  - 0 - Cursor home and display start address home
clear            CH  - 0 - Clear cursor of DD RAM display data home and display start address home
nop              NOP - 0 - No operation
*/

Lcd50530::Lcd50530(byte _ioc1pin, byte _ioc2pin, byte _rwpin, byte _enablepin,
                        byte d0, byte d1, byte d2, byte d3, byte d4, byte d5, byte d6, byte d7)
{
  ioc1=_ioc1pin;
  ioc2=_ioc2pin;
  rw=_rwpin;
  ena=_enablepin;
  data[0]=d0;
  data[1]=d1;
  data[2]=d2;
  data[3]=d3;
  data[4]=d4;
  data[5]=d5;
  data[6]=d6;
  data[7]=d7;
  _8bits=1;

  _assign_pins();
}

Lcd50530::Lcd50530(byte _ioc1pin, byte _ioc2pin, byte _rwpin, byte _enablepin,
                        byte d4, byte d5, byte d6, byte d7)
{
  ioc1=_ioc1pin;
  ioc2=_ioc2pin;
  rw=_rwpin;
  ena=_enablepin;
  data[0]=255;
  data[1]=255;
  data[2]=255;
  data[3]=255;
  data[4]=d4;
  data[5]=d5;
  data[6]=d6;
  data[7]=d7;
  _8bits=0;

  _assign_pins();
}

/**************** HIGH LEVEL FUNCTIONS ***********************/

void Lcd50530::begin(void)
/* Initialize the lcd */
{
  pinMode(ioc1,OUTPUT);
  pinMode(ioc2,OUTPUT);
  pinMode(rw,OUTPUT);
  pinMode(ena,OUTPUT);

  cStyle=1;
  cOn=1;
  cBlink=1;
  cUnd=1;
  cOld=1;
  displayOn=1;

  delay(200);

  /* This must be the first initialization command */
  while (readBusy());
  /* Set thw bus width 8/4 bits */
  SetFunctionMode(_8bits);
  SetEntryMode(0,1,0,0);    // Input data mode
  /* Display on, cursor on, etc... */
  SetDisplayMode(displayOn,cOn,cUnd,cBlink,cStyle);
  clear();                  // Clear
}

void Lcd50530::cursor()
/* Show the cursor */
{
  cOn=1;
  cStyle=cOld;
  SetDisplayMode(displayOn,cOn,cUnd,cBlink,cStyle);
}

void Lcd50530::noCursor()
/* Hide the cursor */
{
  cOn=0;
  cOld=cStyle;
  cStyle=0;
  SetDisplayMode(displayOn,cOn,cUnd,cBlink,cStyle);
}

void Lcd50530::blink()
/* Cursor blinking on */
{
  cBlink=1;
  cStyle=cOld;
  SetDisplayMode(displayOn,cOn,cUnd,cBlink,cStyle);
}

void Lcd50530::noBlink()
/* Cursor blinking off */
{
  cBlink=0;
  cOld=cStyle;
  cStyle=0;
  SetDisplayMode(displayOn,cOn,cUnd,cBlink,cStyle);
}

void Lcd50530::cursorStyle(bool mode)
/* Set the cursor style: underline or whole character */
{
  cStyle=mode;
  SetDisplayMode(displayOn,cOn,cUnd,cBlink,cStyle);
}

void Lcd50530::display()
/* Turn on the display */
{
  displayOn=1;
  SetDisplayMode(displayOn,cOn,cUnd,cBlink,cStyle);
}

void Lcd50530::noDisplay()
/* Turn off the display */
{
  displayOn=0;
  SetDisplayMode(displayOn,cOn,cUnd,cBlink,cStyle);
}

void Lcd50530::createChar(byte num, byte inf[8])
/* Define a character in the CGRAM, up to 8 */
{
  byte i;

  /* Verify the limits */
  if (num>7)
    num=7;

  /* Get actual memory position */
  byte pos=getCursor();

  sendcmd(0xC0 + num*8, 3);
  for (i=0;i<8;i++)
  {
    sendcmd(inf[i],2);
  }

  /* return to the original memory position */
  sendcmd(pos, 3);
}

/**************** MID LEVEL FUNCTIONS ************************/

#if defined(ARDUINO) && ARDUINO >= 100
size_t Lcd50530::write(byte c)
#else
void Lcd50530::write(byte c)
#endif
/* Write a character in the screen */
{
  byte addr;
  
  sendcmd(c,2);

  /* Get cursor address */
  addr=recvcmd(3);
  /* if the cursor pass the limit, put in the next line*/
  if ((addr!= 0) && (addr % 24) == 0)
  {
    if (addr>=168)
      sendcmd(24,3);
    else
      sendcmd(addr+24,3);
  }
#if defined(ARDUINO) && ARDUINO >= 100
  return 1;
#endif
}

void Lcd50530::setCursor(byte col, byte row)
/* Set the cursor position on the screen */
{
  byte lookup[] = { 0, 48, 96, 144, 24, 72, 120, 168 };
  
  /* Don't exceed the row number */
  if (row>8)
    row=7;
  byte addr = lookup[row] + col;
  if (addr>=192)
    addr=0;
  sendcmd(addr,3);
}

byte Lcd50530::getCursor()
/* Return the position of the cursor (in memory address) */
{
  return recvcmd(3);
}

void Lcd50530::blinkFreq(byte speed)
/* Set blinking frequency */
{
  if (speed>3)
    speed=3;

  sendcmd(4 + speed ,0);
}

void Lcd50530::SetDisplayMode(bool on, bool cur, bool und, bool bli, bool cbl)
/* Configure the display.
    on:  Turn on/off the display
    cur: Turns the cursor on/off
    und: Turn on/off the underline
    bli: Make the cursor blink or not
    cbl: Blink the character in the cursor position
*/
{
  sendcmd(32 + on*16 + cur*8 + und*4 + bli*2 + cbl, 0);
}

void Lcd50530::SetEntryMode(bool c_increment, bool c_decrement, bool h_increment, bool h_decrement)
/* Determines the */
{
  sendcmd(64 + 24*(c_increment | c_decrement) + 32*c_increment + 3*(h_increment | h_decrement) + 4*h_increment, 0);
}

void Lcd50530::SetFunctionMode(bool a)
/* Set display size, font and bus width */
{
  /* fixed size 4x48 = 8x24 and font */
  sendcmd(128 + 64 + a*32 + 16 + 8 + 2, 0);
}

void Lcd50530::home(void)
/* Set the cursor at home position */
{
  sendcmd(3,0);
}

void Lcd50530::clear(void)
/* Clear display */
{
  sendcmd(1,0);
}

void Lcd50530::underline(void)
/* Start text underlinining */
{
  sendcmd(15, 0);
}

void Lcd50530::noUnderline(void)
/* Stop text underlinining */
{
  sendcmd(12, 0);
}

void Lcd50530::clearUnderline(void)
/* Clear the underline of the current cursor address */
{
  sendcmd(8 ,0);
}

void Lcd50530::clearUnderlineInc(void)
/* Clear the underline of the current cursor address and increment */
{
  sendcmd(9 ,0);
}

void Lcd50530::setUnderline(void)
/* Set the underline of the current cursor address */
{
  sendcmd(10 ,0);
}

void Lcd50530::setUnderlineInc(void)
/* Set the underline of the current cursor address and increment */
{
  sendcmd(11 ,0);
}

void Lcd50530::scrollDisplayRigth(void)
{
  sendcmd(19 ,0);
}

void Lcd50530::scrollDisplayLeft(void)
{
  sendcmd(18 ,0);
}

void Lcd50530::nop(void)
/* No operation */
{
  sendcmd(0 ,0);
}

/**************** LOW LEVEL FUNCTIONS ************************/

void Lcd50530::_assign_pins(void)
{
  uint8_t i; 
 
  for (i=4*(!_8bits);i<8;i++)
  {
    _lcd_port[i]=portOutputRegister(digitalPinToPort(data[i]));
    _lcd_mask[i]=digitalPinToBitMask(data[i]);
    _data_read_port[i]=portInputRegister(digitalPinToPort(data[i]));
  }
  _lcd_port[IOC1]=portOutputRegister(digitalPinToPort(ioc1));
  _lcd_port[IOC2]=portOutputRegister(digitalPinToPort(ioc2));
  _lcd_port[RW]=portOutputRegister(digitalPinToPort(rw));
  _lcd_port[ENA]=portOutputRegister(digitalPinToPort(ena));
  _lcd_mask[IOC1]=digitalPinToBitMask(ioc1);
  _lcd_mask[IOC2]=digitalPinToBitMask(ioc2);
  _lcd_mask[RW]=digitalPinToBitMask(rw);
  _lcd_mask[ENA]=digitalPinToBitMask(ena);
}

void Lcd50530::_semiPulse(void)
{
  ENABLE_LOW;
  _delay_us(1);
  ENABLE_HIGH;
  _delay_us(1);
}

void Lcd50530::dataPinWrite(uint8_t pin, bool value)
/* Fast digitalWrite */
{
  if (value)
    *_lcd_port[pin] |= _lcd_mask[pin];
  else
    *_lcd_port[pin] &= ~_lcd_mask[pin];
}

bool Lcd50530::dataPinRead(uint8_t pin)
/* Fast digitalRead */
{
  return *_data_read_port[pin] & _lcd_mask[pin];
}

void Lcd50530::pulseEnable(void)
{
  _semiPulse();

  ENABLE_LOW;
  _delay_us(1);
}

void Lcd50530::write8bits(byte value)
/* Write data to the lcd in 8bit bus mode */
{
  for (byte i=0;i<8;i++)
  {
    pinMode(data[i],OUTPUT);
    dataPinWrite(i, ((value >> i) & 0x01));
  }
  
  pulseEnable();
}

void Lcd50530::write4bits(byte value)
/* Write data to the lcd in 4bit bus mode */
{
  for (byte i=4;i<8;i++)
  {
    pinMode(data[i],OUTPUT);
    dataPinWrite(i, ((value >> i) & 0x01));
  }
  
  pulseEnable();
  _delay_us(50);

  for (byte i=0;i<4;i++)
  {
    dataPinWrite(i+4, ((value >> i) & 0x01));
  }

  pulseEnable();
}

byte Lcd50530::read4bits(void)
/* Read data from the lcd in 4bit bus mode */
{
  byte value=0;

  _semiPulse(); 
  
  for (byte i=4;i<8;i++)
  {
    pinMode(data[i],INPUT);
    value += dataPinRead(i);
  }

  _semiPulse(); 
  
  for (byte i=0;i<4;i++)
  {
    value += dataPinRead(i+4);
  }

  ENABLE_LOW;
  _delay_us(1);

  return value;
}

byte Lcd50530::read8bits(void)
/* Read data from the lcd in 8bit bus mode */
{
  byte value=0;

  _semiPulse(); 

  for (byte i=0;i<8;i++)
  {
    pinMode(data[i],INPUT);
    value += dataPinRead(i);
  }

  ENABLE_LOW;
  _delay_us(1);
  
  return value;
}

byte Lcd50530::readFlags(void)
/* Read the lcd processor status flags: BUSY and 4bit nibble waiting status */
{
  bool busy_flag;
  bool _4m2_flag;

  dataPinWrite(IOC1,0);
  dataPinWrite(IOC2,0);
  dataPinWrite(RW,1);

  _semiPulse(); 

  pinMode(data[7],INPUT);
  busy_flag = dataPinRead(7);
  pinMode(data[6],INPUT);
  _4m2_flag = dataPinRead(6);

  /* This is a litle hack. When the display initializes, sometimes the internal
  processor is waiting for the second nibble (4bits) of the command instead of 
  the first. It happens randomnes when first start. To solve this, the first 
  command send in initialization is always this, and if the proccesor is not 
  waiting for more data, we end here the first instrucction, so the next one's 
  can execute correctly. */
  if (! _4m2_flag)
  {
    ENABLE_LOW;
    _delay_us(50);
    ENABLE_HIGH;
    _delay_us(20);
    ENABLE_LOW;
    _delay_us(50);
  }

  return busy_flag + (_4m2_flag << 1);
}

bool Lcd50530::readBusy(void)
/* Read busy flag */
{
  return readFlags() & 0x01;
}

bool Lcd50530::read4mu2(void)
/* Read nibble status flag */
{
  return (readFlags() >> 1) & 0x01;
}

void Lcd50530::sendcmd(byte value, byte mode)
/* Send a command with params to the lcd processor */
{
  while (readBusy());

  dataPinWrite(IOC1,mode & 0x01);
  dataPinWrite(IOC2,mode & 0x02);
  dataPinWrite(RW,0);
  
  if (_8bits)
    write8bits(value);
  else
    write4bits(value);
}

byte Lcd50530::recvcmd(byte mode)
/* Receive data from a command to the lcd processor */
{
  while (readBusy());

  dataPinWrite(IOC1,mode & 0x01);
  dataPinWrite(IOC2,mode & 0x02);
  dataPinWrite(RW,1);
  
  if (_8bits)
    return read8bits();
  else
    return read4bits();
}

