#include "challenges.h"

#include <EEPROM.h>
#include "crypto.h"
#include "lcd.h"

//#define DEBUG_VERIFY

static void print_hash(const uint8_t *hash, int size) {
  for (int _j = 0; _j < 3; _j++) Serial.print(hash[_j], HEX);
  Serial.print("...");
  for (int _j = size - 3; _j < size; _j++) Serial.print(hash[_j], HEX);
  Serial.println();
}

//
// hashes the user input string and compares it against the answers
// only a partial match of 6 bytes is performed from the input hash
// returns the matched level (from 0), or -1 if it doesn't match anything
//
static int verify_flag(const char *flaginput, uint8_t *hashout=nullptr) {
  uint8_t hash[16];
  size_t flaglen = strlen(flaginput);

  int level = -1;

  const u8 *vectors[] = { (u8*) flaginput };
  if (md5_vector(1, vectors, &flaglen, hash) == 0) {
    // verify answers
#ifdef DEBUG_VERIFY
    print_hash(hash, sizeof(hash));
#endif

    for (int i = 0; i < 4; i++) {
#ifdef DEBUG_VERIFY
      print_hash(answers[i], 6);
#endif
      if (memcmp(hash, &answers[i], 3) == 0 && 
          memcmp(&hash[sizeof(hash) - 3], &answers[i][3], 3) == 0) {
        level = i;

#ifdef DEBUG_VERIFY
        Serial.print("matched level ");
        Serial.println(level);
#endif

        // copy hash for caller's use
        if (hashout != nullptr)
          memcpy(hashout, hash, sizeof(hash));

        break;
      }
    }
  }
  return level;
}

//
// checks the flaginput string by calling verify_flag
// and subsequently persists the achieved level into the EEPROM
//
static inline int check_flag(const char *flaginput) {
  uint8_t hash[16];
  int level = verify_flag(flaginput, (uint8_t *) &hash);
  if (level > -1) {
    LcdClear();
    LcdRow(2);
    LcdStr("Bingo!");
    LcdRow(3);
    LcdStr("Level ");
    LcdChar(level + '1');

    // first 3 bytes should always be written
    // this is used for checking which level
    for (int i = 0; i < 3; i++) {
      EEPROM.write(i, hash[i]);
      //if (hash[i] < 0x10) Serial.print('0'); Serial.print(hash[i], HEX);
    }
    //Serial.println();

    // write various parts of hash to EEPROM
    // depending on what level was cleared
    int start = 3, end = 16;
    switch (level) {
      case 0: start = 3;  end = start + 3; break;
      case 1: start = 6;  end = start + 4; break;
      case 2: start = 10; end = start + 3; break;
      case 3: start = 13; end = start + 3; break;
    }

#if 0
    Serial.print("writing additional EEPROM: ");
    Serial.print(start); Serial.print(" - "); Serial.print(end);
    Serial.println();
#endif

    for (int i = start; i < end; i++) {
      EEPROM.write(i, hash[i]);
      //if (hash[i] < 0x10) Serial.print('0'); Serial.print(hash[i], HEX);
    }
    //Serial.println();

    // start writing
    EEPROM.commit();
  }

  return level;
}


void Challenge3::loop() {
  static const char *flag = "XCTF{th1s_i5-N07.Ur~FLAgS}";

  if (requireRedraw) {
    LcdClear();
    LcdRow(1);
    for (const char *c = flag + 5; *c && *c != '}'; c++)
      LcdChar(*c, false, challengefont);
    requireRedraw = false;
  } else if (*flaginput) {
    // check

    int level = check_flag(flaginput);
    if (level > -1) {
      delay(1000);
      removeMe();
    }

    *flaginput = '\0';
  }
}

void Challenge3::keypress(Applet::Key key) {
  switch (key) {
    case KeyLeft:
      removeMe();
      break;

    case KeySelect:
      addApplet(new Keyboard(flaginput, "Enter flag:"));
      break;

    default:
      break;
  }
}


void Challenge2::loop() {
  static const char *text = "a bit off:    "
                            "HMYGR3CTFAOTB3"
                            "QSK4ICFMCLMQ3U"
                            "5QSUMX<<<<<<";

  if (requireRedraw) {
    LcdClear();
    LcdRow(0);
    LcdStr(text);
    requireRedraw = false;
  } else if (*flaginput) {
    // check

    int level = check_flag(flaginput);
    if (level > -1) {
      delay(1000);
      removeMe();
    }

    *flaginput = '\0';
  }
}


void Challenge2::keypress(Applet::Key key) {
  switch (key) {
    case KeyLeft:
      removeMe();
      break;

    case KeySelect:
      addApplet(new Keyboard(flaginput, "Enter flag:"));
      break;

    default:
      break;
  }
}

const char Challenge1::txtMsg[] = "C47ch_M3_If_Y0u_C4n";

static void compute_pos(int *row, int *col, int strpos) {
  const int lastCol = 14;
  const int charWidth = 6;

  if (strpos > lastCol - 1) {
    (*row)++;
    strpos -= lastCol;
  }
  *col = strpos * charWidth;
}

void Challenge1::loop() {
  // redraw after elapsed time
  if (millis() - lastDrawn >= 250) {
    invalidate();
    lastDrawn = millis();
  }

  if (requireRedraw) {
    int row, col;

    LcdClear();

    // draw current char
    int showpos = sequence[pos];
    row = 2; col = 0;
    compute_pos(&row, &col, showpos);
    LcdRow(row, col);
    LcdChar(txtMsg[showpos]);

    prevPos = pos;
    pos++;
    if (pos >= flaglen)
      pos = 0;

    requireRedraw = false;
  } else if (*flaginput) {
    // check

    int level = check_flag(flaginput);
    if (level > -1) {
      delay(1000);
      removeMe();
    }

    *flaginput = '\0';
  }
}


void Challenge1::keypress(Applet::Key key) {
  switch (key) {
    case KeyLeft:
      removeMe();
      break;

    case KeySelect:
      addApplet(new Keyboard(flaginput, "Enter flag:"));
      break;

    default:
      break;
  }
}

const uint8_t qrimg[] = {
  0xc5, 0x9d, 0x61, 0x87, 0xd0, 0xcb, 0x23, 0x3f, 0xeb, 0x2d, 0xdf, 0x52, 0x38, 0x27, 0x3c, 0x16, 
  0xc7, 0xd2, 0x9e, 0x6b, 0x58, 0xc3, 0x73, 0x70, 0x93, 0xda, 0x7f, 0x94, 0x0d, 0xe6, 0x81, 0xc8, 
  0xce, 0x4e, 0xfd, 0x31, 0xdc, 0x41, 0xaa, 0x49, 0xa4, 0x75, 0xd9, 0xe4, 0x5f, 0x50, 0x2f, 0x0c, 
  0x51, 0xfd, 0xff, 0x60, 0xb0, 0x05, 0xae, 0xf6, 0x92, 0xcb, 0xec, 0x38, 0xa7, 0x1b, 0x59, 0xf3, 
  0x21, 0x71, 0xeb, 0xe3, 0x6a, 0xdd, 0x14, 0x3d, 0x41, 0xec, 0x9e, 0x6b, 0x3b, 0xbc, 0xcf, 0xeb, 
  0xfb, 0xcb, 0x21, 0xe4, 0x76, 0xed, 0xcb, 0x3f, 0xf7, 0xb5, 0x7b, 0xb5, 0xf1, 0x4d, 0xd3, 0x50, 
  0x3f, 0x7e, 0x95, 0x9d, 0x3f, 0x26, 0xa1, 0x7e, 0x5d, 0x30, 0xa9, 0x34, 0xb2, 0x44, 0x84, 0x15, 
  0xed, 0xf5, 0x4f, 0x77, 0x72, 0x84, 0x3d, 0x9a, 0x68, 0xab, 0xeb, 0xb2, 0x9a, 0xfd, 0xac, 0x9e, 
  0x12, 0x97, 0x09, 0xc7, 0x0e, 0x4a, 0x58, 0x3d, 0x6e, 0xf4, 0x98, 0x73, 0x40, 0x37, 0xb9, 0x76, 
  0x4a, 0xfc, 0xbd, 0x76, 0x67, 0x09, 0xaf, 0x7d, 0x77, 0x22, 0x57, 0x5d, 0x43, 0xce, 0xe3, 0x96, 
  0x5d, 0xb9, 0xc3, 0xd3, 0xca, 0xa7, 0x51, 0x6d, 0x57, 0x9a, 0x95, 0x97, 0x78, 0x94, 0xe8, 0xf0, 0xc2, 0xc2, 0x88, 0x9b, 0xf5, 0xd9, 0x83, 0xca, 0xa9, 0x3d, 0x87, 0xab, 0xf9, 0xc0, 0xe1, 0xc4, 0xc9, 0x1d, 0x3a, 0x07, 0x12, 0xf5, 0x5d, 0xf8, 0xf4, 0x37, 0x36, 0x7b, 0xa1, 0x7f, 0x17, 0x9e, 0x17, 0x23, 0x7e, 0x5b, 0x55, 0x1b, 0x00, 0x5e, 0x34, 0x64, 0xd5, 0x5a, 0x93, 0x88, 0x4d, 0x53, 0x1b, 0x5d, 0xfd, 0x2c, 0x77, 0xbc, 0x6f, 0x39, 0x8c, 0xb7, 0x00, 0x37, 0x99, 0x3d, 0x50, 0x2a, 0x3a, 0xd1, 0xbc, 0x60, 0xfd, 0x20, 0x02, 0xf0, 0x21, 0xcd, 0x61, 0xf5, 0xd6, 0x76, 0xe9, 0x17
};

void BossLevel::loop() {
  if (requireRedraw) {
    void *ctx;

    uint8_t *decrypted = (uint8_t *) malloc(256);
    if (decrypted == NULL)
      goto decrypt_err;

    ctx = aes_decrypt_init(key, AES_BLOCK_SIZE);
    if (ctx != NULL) {
      for (int off = 0; off < 256; off += 16) {
        aes_decrypt(ctx, qrimg + off, decrypted + off);
      }
      aes_decrypt_deinit(ctx);
    }

    LcdClear();

    for (int i = 0; i < 6; i++) {
      LcdRow(i, 6 * 3);
      for (int j = 0; j < 42; j++) {
        LcdWrite(decrypted[i * 42 + j]);
      }
    }

    free(decrypted);

decrypt_err:
    requireRedraw = false;
  }
}

void BossLevel::keypress(Applet::Key key) {
  switch (key) {
    case KeyLeft:
      removeMe();
      break;

    default:
      break;
  }
}

