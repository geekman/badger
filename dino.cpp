//
// Dino game, like from Chrome
// originally made for Arduboy
// hacked to work on our badge :P
// see https://github.com/flaki/arduboy-rund-ino/
//

#include "lcd.h"
#include "dino.h"

#define BLACK 0
#define WHITE 1

// fake Arduboy class wrapper
// calls our functions for subset of Arduboy functionality
// original code here https://github.com/Arduboy/Arduboy/blob/master/src/Arduboy.cpp
class Arduboy {
  public:
    const int WIDTH = LCD_X, HEIGHT = LCD_Y;

    Arduboy() {
      clearDisplay();
    }

    void clearDisplay() {
      memset(sBuffer, 0, sizeof(sBuffer));
    }

    void print(const char *c) {
      int xPos = 0;
      for (; *c; c++, xPos += 6) {
        drawChar(xPos, 0, *c, WHITE, BLACK, 1);
      }
    }

    void drawChar(int16_t x, int16_t y, unsigned char c, uint8_t color, uint8_t bg, uint8_t size) {
      boolean draw_background = bg != color;
      c -= 0x20;

      if ((x >= WIDTH) ||         // Clip right
          (y >= HEIGHT) ||        // Clip bottom
          ((x + 5 * size - 1) < 0) ||   // Clip left
          ((y + 8 * size - 1) < 0)    // Clip top
         )
      {
        return;
      }

      for (int8_t i = 0; i < 6; i++ )
      {
        uint8_t line;
        if (i == 5)
        {
          line = 0x0;
        }
        else
        {
          line = pgm_read_byte(&LcdCharTable[c][i]);
        }

        for (int8_t j = 0; j < 8; j++)
        {
          uint8_t draw_color = (line & 0x1) ? color : bg;

          if (draw_color || draw_background) {
            for (uint8_t a = 0; a < size; a++ ) {
              for (uint8_t b = 0; b < size; b++ ) {
                drawPixel(x + (i * size) + a, y + (j * size) + b, draw_color);
              }
            }
          }
          line >>= 1;
        }
      }
    }

    void drawPixel(int x, int y, uint8_t color) {
#ifdef PIXEL_SAFE_MODE
      if (x < 0 || x > (WIDTH - 1) || y < 0 || y > (HEIGHT - 1))
      {
        return;
      }
#endif

      uint8_t row = (uint8_t)y / 8;
      if (color)
      {
        sBuffer[(row * WIDTH) + (uint8_t)x] |=   _BV((uint8_t)y % 8);
      }
      else
      {
        sBuffer[(row * WIDTH) + (uint8_t)x] &= ~ _BV((uint8_t)y % 8);
      }
    }

    void drawFastHLine(int16_t x, int16_t y, uint8_t w, uint8_t color) {
      // Do bounds/limit checks
      if (y < 0 || y >= HEIGHT) {
        return;
      }

      // make sure we don't try to draw below 0
      if (x < 0) {
        w += x;
        x = 0;
      }

      // make sure we don't go off the edge of the display
      if ((x + w) > WIDTH) {
        w = (WIDTH - x);
      }

      // if our width is now negative, punt
      if (w <= 0) {
        return;
      }

      // buffer pointer plus row offset + x offset
      register uint8_t *pBuf = sBuffer + ((y / 8) * WIDTH) + x;

      // pixel mask
      register uint8_t mask = 1 << (y & 7);

      switch (color)
      {
        case WHITE:
          while (w--) {
            *pBuf++ |= mask;
          };
          break;

        case BLACK:
          mask = ~mask;
          while (w--) {
            *pBuf++ &= mask;
          };
          break;
      }
    }

    void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t w, uint8_t h, uint8_t color) {
      // no need to dar at all of we're offscreen
      if (x + w < 0 || x > WIDTH - 1 || y + h < 0 || y > HEIGHT - 1)
        return;

      int yOffset = abs(y) % 8;
      int sRow = y / 8;
      if (y < 0) {
        sRow--;
        yOffset = 8 - yOffset;
      }
      int rows = h / 8;
      if (h % 8 != 0) rows++;
      for (int a = 0; a < rows; a++) {
        int bRow = sRow + a;
        if (bRow > (HEIGHT / 8) - 1) break;
        if (bRow > -2) {
          for (int iCol = 0; iCol < w; iCol++) {
            if (iCol + x > (WIDTH - 1)) break;
            if (iCol + x >= 0) {
              if (bRow >= 0) {
                if      (color == WHITE) this->sBuffer[ (bRow * WIDTH) + x + iCol ] |= pgm_read_byte(bitmap + (a * w) + iCol) << yOffset;
                else if (color == BLACK) this->sBuffer[ (bRow * WIDTH) + x + iCol ] &= ~(pgm_read_byte(bitmap + (a * w) + iCol) << yOffset);
                else                     this->sBuffer[ (bRow * WIDTH) + x + iCol ] ^= pgm_read_byte(bitmap + (a * w) + iCol) << yOffset;
              }
              if (yOffset && bRow < (HEIGHT / 8) - 1 && bRow > -2) {
                if      (color == WHITE) this->sBuffer[ ((bRow + 1)*WIDTH) + x + iCol ] |= pgm_read_byte(bitmap + (a * w) + iCol) >> (8 - yOffset);
                else if (color == BLACK) this->sBuffer[ ((bRow + 1)*WIDTH) + x + iCol ] &= ~(pgm_read_byte(bitmap + (a * w) + iCol) >> (8 - yOffset));
                else                     this->sBuffer[ ((bRow + 1)*WIDTH) + x + iCol ] ^= pgm_read_byte(bitmap + (a * w) + iCol) >> (8 - yOffset);
              }
            }
          }
        }
      }
    }

    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color) {
      // well it looks like they only draw horizontal lines, so let's hardcode this
      drawFastHLine(x0, y0, x1 - x0, color);
    }

    void display() {
      LcdRow(0); // home position 0,0
      LcdWrite(sBuffer, sizeof(sBuffer));
    }

  protected:
    unsigned char sBuffer[(LCD_Y * LCD_X) / 8];
};

PROGMEM const unsigned char cactus_1[] =
  // w:  12  h:  24
{ 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xfe, 0x00, 0xc0, 0xc0, 0x80, 0xfe, 0xff, 0xfe, 0x00, 0xff, 0xff, 0xff, 0xff, 0xc0, 0xff, 0xff, 0x7f, 0x01, 0x03, 0x03, 0x83, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00 };

// cloud_1 w:  17  h:  7
PROGMEM const unsigned char cloud_1[] =
{ 0x1c, 0x22, 0x22, 0x22, 0x24, 0x10, 0x12, 0x2a, 0x21, 0x41, 0x41, 0x41, 0x42, 0x4a, 0x24, 0x24, 0x18 };


PROGMEM const unsigned char dino_up[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xfe, 0xf6, 0xfe, 0xfe, 0x7e, 0x7e, 0x7e, 0x7e, 0x7c, 0x00, 0x00, 0xfc, 0xf0, 0xe0, 0xc0, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff, 0xff, 0xff, 0xff, 0x09, 0x19, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x07, 0x0f, 0xff, 0xbf, 0x1f, 0x0f, 0x1f, 0xff, 0x87, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// w:  30  h:  18
PROGMEM const unsigned char dino_tumble[] =
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0xf8, 0xf0, 0xe0, 0xe0, 0xf0, 0xf0, 0xf8, 0xf8, 0xf8, 0xf8, 0xf0, 0xf0, 0xf0, 0xe0, 0xe0, 0xc0, 0xc0, 0x80, 0xc0, 0xf0, 0xa8, 0xd8, 0xa8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf0, 0x00, 0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x01 };



// top w:  20  h:  18
PROGMEM const unsigned char dino_top[] =
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xfb, 0xff, 0xff, 0xbf, 0xbf, 0xbf, 0x3f, 0x3e, 0x7e, 0xf8, 0xf0, 0xe0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x04, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

// leg_0 w:  20  h:  5
PROGMEM const unsigned char dino_leg_0[] =
{ 0x00, 0x00, 0x00, 0x00, 0x01, 0x1f, 0x17, 0x03, 0x01, 0x03, 0x1f, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

// leg_1 w:  20  h:  5
PROGMEM const unsigned char dino_leg_1[] =
{ 0x00, 0x00, 0x00, 0x00, 0x01, 0x0f, 0x0b, 0x01, 0x01, 0x03, 0x1f, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

// leg_2 w:  20  h:  5
PROGMEM const unsigned char dino_leg_2[] =
{ 0x00, 0x00, 0x00, 0x00, 0x01, 0x1f, 0x17, 0x03, 0x01, 0x03, 0x0f, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };



// distance ran
int d, delta;

int cloud_1_y;

int d_jump, d_jump_t;

int d_run;
int d_tumble_t;

int ox;

int frame = 0;

char text[20];

Arduboy arduboy;

void dino_setup() {
  d = 0;
  delta = 0;

  cloud_1_y = 2;

  d_jump = 0;
  d_jump_t = 0;

  d_tumble_t = 0;
  d_run = 0;

  ox = arduboy.WIDTH;
}

void dino_loop(int jump) {
  if (!d_run && jump) {
    d_run = 1;
  }

  // reset game
  if (d_tumble_t > 8 && jump) {
    dino_setup();
    return;
  }

  ++frame;
  if (frame > 16000) frame = 0;

  // increase distance whilst running
  if (d_run && (++delta > 4)) {
    delta = 0; ++d;
  }

  // obstacles
  if (d_run) {
    ox -= (frame % 2) * (d / 100) + 2;
    if (ox < -15) ox += 140 + random(0, 60);
  }

  // jump!
  if (!d_jump_t && jump) {
    d_jump_t = 1;
    d_jump = 5;

    //arduboy.tunes.tone(440, 40);

  } else if (d_jump_t) {
    if (d_jump_t == 3) ;//arduboy.tunes.tone(880, 80);

    ++d_jump_t;

    if (d_jump_t < 6 / 2) {
      d_jump += 6;
    } else if (d_jump_t < 9 / 2) {
      d_jump += 2;
    } else if (d_jump_t < 13 / 2) {
      d_jump += 1;
    } else if (d_jump_t == 16 / 2 || d_jump_t == 18 / 2) {
      d_jump += 1;
    } else if (d_jump_t == 20 / 2 || d_jump_t == 22 / 2) {
      d_jump -= 1;
    } else if (d_jump_t > 38 / 2) {
      d_jump = 0;
      d_jump_t = 0;
    } else if (d_jump_t > 32 / 2) {
      d_jump -= 6;
    } else if (d_jump_t > 29 / 2) {
      d_jump -= 2;
    } else if (d_jump_t > 25 / 2) {
      d_jump -= 1;
    }
  }

  // hit detect
  if (!d_tumble_t && ox > -10 && ox < 16 && d_jump_t < 14 / 3) {
    d_tumble_t = 1;
  }

  if (d_tumble_t) {
    if (d_tumble_t == 1) {
      //arduboy.tunes.tone(246, 80);
    } else if (d_tumble_t == 6 / 2) {
      //arduboy.tunes.tone(174, 200);
    }

    ++d_tumble_t;
    if (d_jump > -4) {
      d_jump -= 1;
      ox -= 1;
    } else {
      d_run = 0;
    }
  }

  arduboy.clearDisplay();

  // hud
  //arduboy.setCursor(100, 0);
  sprintf(text,"%d",d);
  arduboy.print(text);


  // parallax clouds
  arduboy.drawBitmap(arduboy.WIDTH - (d % arduboy.WIDTH), cloud_1_y, cloud_1, 17, 7, WHITE);

  if (d % arduboy.WIDTH == 0) {
    cloud_1_y = random(0, 10);
  }

  // terrain
  static const int terrain_y = 45;
  if (d_jump > 4) {
    arduboy.drawLine(0, terrain_y, arduboy.WIDTH - 1, terrain_y, WHITE);
  } else {
    arduboy.drawLine(0, terrain_y, 3, terrain_y, WHITE);
    arduboy.drawLine(12, terrain_y, arduboy.WIDTH - 1, terrain_y, WHITE);
  }

  // obstacles
  arduboy.drawBitmap(ox, terrain_y - 20, cactus_1, 12, 24, WHITE);

  // dino
  int dy = 25 - d_jump; //XXX 40 initially

  // tumbles!
  if (d_tumble_t) {
    arduboy.drawBitmap(0, dy, dino_tumble, 30, 18, WHITE);

    // runs!
  } else {
    arduboy.drawBitmap(0, dy, dino_top, 20, 18, WHITE);

    // Run, Dino, Run!
    if (d_run && !d_jump) {
      if ((frame % 8) / 4) {
        arduboy.drawBitmap(0, dy + 18, dino_leg_1, 20, 5, WHITE);
      } else {
        arduboy.drawBitmap(0, dy + 18, dino_leg_2, 20, 5, WHITE);
      }
    } else {
      arduboy.drawBitmap(0, dy + 18, dino_leg_0, 20, 5, WHITE);
    }
  }

  arduboy.display();
  delay(50);
}

// Applet code goes here
// this is what integrates the game into the menus

void Dino::setup(vector<Applet*> &stack) {
  Applet::setup(stack);
  dino_setup();
  key_up = false;
}

void Dino::loop() {
  dino_loop(key_up);

  // unset it
  key_up = false;
}

void Dino::keypress(Applet::Key key) {
  switch (key) {
    case KeyUp:
      key_up = true;
      break;

    case KeyLeft:
      removeMe();
      break;
  }
}

