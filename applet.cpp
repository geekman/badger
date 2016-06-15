#include "applet.h"
#include "lcd.h"
#include <string.h>

void Menu::add(int id, const char *item) {
  m_items.push_back(std::make_pair(id, strdup(item)));
}

void Menu::keypress(Key key) {
  if (m_items.size() == 0)
    return;

  int oldPos = cursorPos;
  switch (key) {
    case KeyDown:  cursorPos++; break;
    case KeyUp:    cursorPos--; break;

    case KeySelect: {
        pair<int, const char*> item = m_items[cursorPos];
        int id = item.first;
        select(id);
        break;
      }

    default:
      return;
  }

  // check bounds
  if (cursorPos < 0)
    cursorPos = 0;
  else if (cursorPos > m_items.size() - 1)
    cursorPos = m_items.size() - 1;

  // check if need to redraw
  if (oldPos != cursorPos)
    invalidate();
}

void Menu::redraw() {
  // figure out which rows to render
  int totalItems = m_items.size();
  int lastIndex = totalItems - 1;
  int halfScreen = (maxItems + 1) / 2;
  int topIndex = cursorPos - halfScreen;
  int bottomIndex = cursorPos + halfScreen;

  // check bounds
  if (topIndex < 0) topIndex = 0;
  if (bottomIndex > lastIndex) bottomIndex = lastIndex;

  int shownItems = bottomIndex - topIndex + 1;
  int excessItems = shownItems - maxItems;
#ifdef DEBUG_MENU
  Serial.print(" c "); Serial.print(cursorPos);
  Serial.print(" t "); Serial.print(topIndex);
  Serial.print(" b "); Serial.print(bottomIndex);
  Serial.print(" tt "); Serial.print(totalItems);
  Serial.print(" ex "); Serial.print(excessItems);
  Serial.println();
#endif

  if (totalItems <= maxItems) {
    // everything fits in a screen. just set bounds
    topIndex = 0;
    bottomIndex = lastIndex;
  } else {
    // either too many or too little items on screen -  need readjustment
    if (excessItems != 0) {
      if (lastIndex - cursorPos < maxItems / 2) { // we are near end
        topIndex += excessItems;
      } else { // near top of list
        bottomIndex -= excessItems;
      }
    }

    // check bounds again, after readjusting
    if (topIndex < 0) {
      Serial.println("\nXXX ti<0");
      topIndex = 0;
    }
    if (bottomIndex > lastIndex) {
      Serial.println("\nXXX li>li");
      bottomIndex = lastIndex;
    }
  }

  int rowCount = 0;
  for (int i = topIndex; i <= bottomIndex; i++, rowCount++) {
    LcdRow(i - topIndex);
#ifdef DEBUG2
    Serial.print("\n");
    Serial.print(" r="); Serial.print(i - topIndex);
    Serial.print(" i="); Serial.print(i);
    Serial.print(" t="); Serial.print(topIndex);
    Serial.print(i == cursorPos ? " *" : "  ");
    Serial.print(":");
#endif

    pair<int, const char*> item = m_items[i];
    const char *itemStr = item.second;
    int j = 0;
    for (const char *c = itemStr;
         j < maxChars;
         *c && c++, j++) {
      LcdChar(*c ? *c : ' ', i == cursorPos);

#ifdef DEBUG2
      if (*c) Serial.print(*c);
      else Serial.print("' '");
#endif
    }
  }

  // clear away rows that are supposed to be empty after bottom index
  for (int i = rowCount; i < maxItems; i++) {
    LcdRow(i);
    for (int j = 0; j < maxChars; j++)
      LcdChar(' ');
  }

  // XXX somehow the last bar of data is not being flushed to the screen
  // XXX send a no-op command to the LCD to flush that last piece of data
  LcdRow(0);

  requireRedraw = 0;
}

void Keyboard::keypress(Key key) {
  int oldKpos = kpos;
  switch (key) {
    case KeyUp:    kpos -= maxLine; break;
    case KeyDown:  kpos += maxLine; break;
    case KeyLeft:  kpos--; break;
    case KeyRight: kpos++; break;

    case KeySelect:
      if (kpos == keyboardLen - 1) { // ENTER
        input[ipos] = '\0';

        // copy entered data into output
        if (output != nullptr)
          strncpy(output, input, ipos + 1);

        completed();
      } else if (kpos == keyboardLen - 2) { // BACKSPACE
        if (ipos > 0) {
          ipos--;
          invalidate();
        } else {
          // nothing to erase
          LcdFlash(100);
        }
      } else {
        char k = keyboard[kpos];
        if (ipos >= sizeof(input) - 1) {
          // input buffer full
          LcdFlash(100);
        } else if (k != ' ') {
          input[ipos++] = k;
          invalidate();
        }
      }
      break;
  }

  // check bounds and wrap around
  if (kpos > keyboardLen - 1)
    kpos -= keyboardLen;
  else if (kpos < 0)
    kpos += keyboardLen;

  if (oldKpos != kpos)
    invalidate();
}

void Keyboard::redraw() {
  LcdClear();

  // draw prompt
  if (prompt != nullptr) {
    LcdRow(0);
    LcdStrL(prompt);
  }

  // draw current input
  int istart = 0;
  if (ipos > maxLine - 1)
    istart = ipos - maxLine + 1;
  LcdRow(1);
  for (int i = istart; i < ipos; i++)
    LcdChar(input[i]);
  LcdChar('_'); // cursor

  // draw keyboard
  LcdRow(3);
  for (int i = 0; i < keyboardLen; i++) {
    LcdChar(keyboard[i], kpos == i);
  }
  
  requireRedraw = false;
}

