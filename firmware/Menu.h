#ifndef MENU_H
#define MENU_H

#include "Arduino.h"
#include "Screen.h"
//#include <LiquidCrystal.h>
//extern LiquidCrystal lcd;


class MenuItem {
  public:
    MenuItem(const char* n, int x, int y, Screen *s);
    MenuItem(const char* n, int x, int y, Screen *s0, Screen *s1, Screen *s2, Screen *s3);
    const char* name;
    void nextScreen(void);
    void prevScreen(void);
    void back(void);
    int selectedScreen=0;
    bool entered = false;
    Screen *screens[4];
    int sysLocX, sysLocY;
};

class Menu {
  public:
    Menu(MenuItem* menuItem0, MenuItem* menuItem1, MenuItem* menuItem2, MenuItem* menuItem3);
    MenuItem* menus[4];
    int selectedMenuItem = 0;
    bool entered = false;
    void show(void);
    void next(void);
    void prev(void);
    void enter(void);
    void back(void);
    void refresh(void);
};

#endif
