#include "lcd.h"
#include <SPI.h>


void LcdInit() {
  SPI.begin();
  SPI.setFrequency(4000000); // 4MHz (max speed for PCD8544)
  pinMode(PIN_DC, OUTPUT);

  LcdCmd(0x21);  // LCD Extended Commands.
  LcdCmd(0x80 | 0x38);  // Set LCD Vop (Contrast). 
  LcdCmd(0x04);  // Set Temp coefficent. //0x04
  LcdCmd(0x14);  // LCD bias mode 1:48. //0x13
  LcdCmd(0x20);
  LcdCmd(0x0C);  // LCD in normal mode.
}

//////////////////////////

// each character is 5 pixel wide, plus one pixel space
void LcdChar(char c, bool invert, const uint8_t fontmap[][5]) {
  unsigned char inv = invert ? 0xFF : 0;
  if (fontmap == nullptr)
    fontmap = LcdCharTable;
  c -= 0x20; // first char in font table
  for (int i = 0; i < 5; i++)
    LcdWrite(inv ^ fontmap[c][i]);

  LcdWrite(inv ^ 0x00);
}

void LcdStr(const char *str, bool invert) {
  for (const char *p = str; *p; p++) 
    LcdChar(*p, invert);
}

// limit to max chars per row
void LcdStrL(const char *str, bool invert) {
  const char *p = str;
  int i;
  for (i = 0; 
      *p && i < 84/6; 
      p++, i++) 
    LcdChar(*p, invert);
}

void LcdRow(int row) {
  LcdCmd(0x40 | (row & 0b111));  // sets Y address (0 - 5)
  LcdCmd(0x80 | 0);              // sets X address to 0
}

void LcdClear(void) {
  for (int index = 0; index < LCD_X * LCD_Y / 8; index++)
    LcdWrite(0);
}

