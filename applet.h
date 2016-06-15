#ifndef __APPLET_H__
#define __APPLET_H__

#include <string>
#include <vector>
#include <list>
#include <utility>
#include "lcd.h"

using std::list;
using std::vector;
using std::pair;
using std::string;


// forward declaration
class Applet;

// an app-let is the small program running on the device
// keypress() will be called first, followed by loop()
class Applet {
  public:
    enum Key { KeyUp = 15, KeyDown = 5, KeyLeft = 4, KeyRight = 2, KeySelect = 0 };

    // default setup. remember to call this from subclasses!
    virtual void setup(vector<Applet*> &stack) {
      appletStack = &stack;

      // draw for the first time
      invalidate();
    };

    // removes current applet
    inline void removeMe() {
      appletStack->pop_back();
    }

    // adds a new applet
    inline void addApplet(Applet *app) {
      appletStack->push_back(app);
      app->setup(*appletStack);
    }

    virtual void loop() { };
    virtual void keypress(Key key) { };
    virtual inline void invalidate() {
      requireRedraw = true;
    }

  protected:
    bool requireRedraw;
    vector<Applet*> *appletStack;
};

// facilitates making menus
// just add all the items using add()
// a selection cursor will be displayed and scrolling handled automatically
// if you subclass, override setupItems() to put ur items
// override select(int) to handle selection
class Menu : public Applet {
  public:
    Menu() : cursorPos(0), m_items() {}

    void add(int id, const char *item);

    // redraws the menu
    void redraw();

    virtual void setup(vector<Applet*> &stack) {
      Applet::setup(stack);
      setupItems();
    }

    // called during setup to init menu items
    virtual inline void setupItems() {}

    // called upon user selection
    // override this if you want to do something useful
    virtual inline void select(int id) {}

    virtual void loop() {
      if (requireRedraw) redraw();
    }

    virtual void keypress(Key key);

  private:
    vector<pair<int, const char *>> m_items;
    int cursorPos;
    static const int maxItems = 6, maxChars = 84 / 6;
};

class Keyboard : public Applet {
  public:
    Keyboard(char *output, const char *prompt) :
      ipos(0), kpos(0), uppercase(false), prompt(prompt), output(output) {}

    virtual void loop() {
      if (requireRedraw) redraw();
    };

    virtual void keypress(Key key);

    // called after user selects ENTER
    // default implementation is just to exit
    virtual inline void completed() {
      removeMe();
    }

    void redraw();

    static const int maxLine = 84 / 6;

    // on-screen keyboard
    static const int keyboardLen = 3 * maxLine;
    static constexpr const char *keyboard = "abcdefghijklm_"
                                            "nopqrstuvwxyz\x81"
                                            "0123456789@ \x7f\x80";

  protected:
    // current positions for keyboard and input buffer
    int kpos, ipos;
    bool uppercase;
    char input[32 + 1];

    const char *prompt;   // user prompt
    char *output;         // output buffer
};

#endif
