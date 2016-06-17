
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <Bounce2.h> // Bounce2 debouncing lib

#include <list>
#include <vector>
#include <string>

#include "lcd.h"
#include "applet.h"
#include "wifiscanner.h"
#include "dino.h"

using std::vector;
using std::list;
using std::string;
using std::pair;

extern "C" {
#include "user_interface.h"
}

Bounce bouncers[5];
static const int buttons[] = {0, 2, 4, 5, 15};
long lastButtonPress = 0;

vector<Applet*> applets;


void buttonPressed() {
  lastButtonPress = millis();
}

int readButtons() {
  int b = -1;
  for (int i = 0; i < 5; i++) {
    int changed = bouncers[i].update();
    int pressed = bouncers[i].read();
    
    // invert state for GPIO15
    if (buttons[i] == 15) pressed = !pressed;

    if (changed && pressed)
      b = i;
  }
  return b;
}

void setupButtons() {
  // GPIO15 is pulled down externally, 
  // so no pullups and you need to invert its value

  for (int i = 0; i < 5; i++) {
    int b = buttons[i];

    pinMode(b, b == 15 ? INPUT : INPUT_PULLUP);
    
    bouncers[i].attach(b);
    bouncers[i].interval(5); // 5ms

    //attachInterrupt(digitalPinToInterrupt(b), buttonPressed, b == 15 ? RISING : FALLING);
  }
}

class MainMenu : public Menu {
  public:
    void setupItems() {
      const char *menu[] = {
        "WiFi Scanner",
        "Challenges",
        "Solve",
      };

      for (int i = 0; i < 3; i++) {
        add(i, menu[i]);
      }
    }

    void select(int id) {
      switch (id) {
        case 0:
          addApplet(new WiFiScanner());
          break;

        case 1:
          addApplet(new Dino());
          break;

        case 2:
          addApplet(new Keyboard(nullptr, "Enter flag:"));
          break;
      }
    }
};

void setup() {
  // dont be an AP by default
  WiFi.mode(WIFI_STA);

  // setup serial and get rid of pre-boot garbage
  Serial.begin(115200);
  Serial.println();

  // set low power mode
  wifi_set_sleep_type(LIGHT_SLEEP_T);

  LcdInit();
  LcdClear();

  // show splash screen here

  setupButtons();

  // setup first applet
  applets.push_back(new MainMenu());
  applets.back()->setup(applets);
}


void loop() {
  if (applets.size() == 0) {
    // nothing to do!
    yield();
    return;
  }

  Applet *currApplet = applets.back();
  int currSize = applets.size();

  int b = readButtons();
  if (b != -1) {
    currApplet->keypress(static_cast<Applet::Key>(buttons[b]));
  }

  // keypress() method has removed the applet
  if (applets.size() < currSize) goto stack_updated;

  currApplet->loop();

  // loop() method removed applet
  if (applets.size() < currSize) goto stack_updated;

  return;

stack_updated:
  // free the removed applet
  delete currApplet;

  // tell previous applet that it needs to redraw on next loop
  applets.back()->invalidate();
}

