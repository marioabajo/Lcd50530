#ifndef Lcd50350_h
#define Lcd50350_h

#include <inttypes.h>
#include "Print.h"

#define byte uint8_t

// Internal definitions THERY ARE NOT PORTS
#define IOC1 8
#define IOC2 9
#define RW   10
#define ENA  11
#define ENABLE_LOW  *_lcd_port[ENA] &= ~_lcd_mask[ENA]
#define ENABLE_HIGH *_lcd_port[ENA] |= _lcd_mask[ENA]

class Lcd50530 : public Print {
  public:
    Lcd50530(byte _ioc1pin, byte _ioc2pin, byte _rwpin, byte _enablepin,
             byte d0, byte d1, byte d2, byte d3, byte d4, byte d5, byte d6, byte d7);
    Lcd50530(byte _ioc1pin, byte _ioc2pin, byte _rwpin, byte _enablepin,
             byte d4, byte d5, byte d6, byte d7);
    void begin(void);
#if defined(ARDUINO) && ARDUINO >= 100
    size_t write(byte c);
#else
    void write(byte c);
#endif
    void cursor();
    void noCursor();
    void blink();
    void noBlink();
    void cursorStyle(bool mode);
    void display();
    void noDisplay();
    void setCursor(byte col, byte row);
    byte getCursor();
    void blinkFreq(byte speed);
    void SetDisplayMode(bool on, bool cur, bool und, bool bli, bool cbl);
    void SetEntryMode(bool c_increment, bool c_decrement, bool h_increment, bool h_decrement);
    void home(void);
    void clear(void);
    void underline(void);
    void noUnderline(void);
    void clearUnderline(void);
    void clearUnderlineInc(void);
    void setUnderline(void);
    void setUnderlineInc(void);
    void createChar(byte num, byte inf[8]);
    void scrollDisplayRigth(void);
    void scrollDisplayLeft(void);
    void nop(void);
    
  private:
    void _assign_pins(void);
    void _semiPulse(void);
    void dataPinWrite(uint8_t pin, bool value);
    bool dataPinRead(uint8_t pin);
    void SetFunctionMode(bool _8bits);
    void pulseEnable(void);
    void write8bits(byte value);
    void write4bits(byte value);
    byte read8bits(void);
    byte read4bits(void);
    byte readFlags(void);
    bool readBusy(void);
    bool read4mu2(void);
    void sendcmd(byte value, byte mode);
    byte recvcmd(byte mode);
    
    byte ioc1;
    byte ioc2;
    byte rw;
    byte ena;
    byte data[8];
    bool _8bits;
    bool cStyle;
    bool cOn;
    bool cBlink;
    bool cUnd;
    bool cOld;
    bool displayOn;
    volatile uint8_t *_lcd_port[12];  // 8 data port + ico1 + ioc2 + rw + ena
    uint8_t _lcd_mask[12];
    volatile uint8_t *_data_read_port[8];

    /*volatile uint8_t *_ioc1_port;
    uint8_t _ioc1_mask;
    volatile uint8_t *_ioc2_port;
    uint8_t _ioc2_mask;
    volatile uint8_t *_rw_port;
    uint8_t _rw_mask;
    volatile uint8_t *_ena_port;
    uint8_t _ena_mask;*/
};

#endif
