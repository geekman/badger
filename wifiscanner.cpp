#include "wifiscanner.h"

#include <ESP8266WiFi.h>
#include "lcd.h"

void WiFiScanner::setupItems() {
  // setup as a wireless client
  WiFi.mode(WIFI_STA);
  delay(1);

  LcdClear();
  LcdRow(2);
  LcdStr(" scanning... ");

  int numAps = WiFi.scanNetworks();
  for (int i = 0; i < numAps; i++) {
    //Serial.print(i); Serial.print(' '); Serial.println(WiFi.SSID(i).c_str());
    add(i, WiFi.SSID(i).c_str());
  }

  // give feedback when no APs found
  if (numAps == 0) {
      add(0, "- no APs -");
  }

  // power off WiFi
  WiFi.mode(WIFI_OFF);
  delay(1);
}

void WiFiScanner::keypress(Applet::Key key) {
  switch (key) {
    case KeyLeft:
      removeMe();
      break;

    default:
      Menu::keypress(key);
      break;
  }
}

