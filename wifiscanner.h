#ifndef __WIFISCANNER_H__
#define __WIFISCANNER_H__

#include <SPI.h>
#include "applet.h"

class WiFiScanner : public Menu {
  public:
  virtual void setupItems();
  virtual void keypress(Applet::Key key);
};

#endif
