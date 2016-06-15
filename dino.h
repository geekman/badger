#ifndef __DINO_H__
#define __DINO_H__

#include "applet.h"

class Dino : public Applet {
  public:
    void setup(vector<Applet*> &stack);
    void loop();
    void keypress(Applet::Key key);

  private:
    // key UP was pressed?
    bool key_up;

};

#endif
