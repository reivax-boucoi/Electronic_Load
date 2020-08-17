#ifndef MENU_H
#define MENU_H

#include "Arduino.h"
#include "Screen.h"

class MenuItem;
class Menu;

class MenuItem {
  public:
    MenuItem(uint8_t x, uint8_t y, Screen *s);
	void addScreen(Screen *s);
	
    const __FlashStringHelper* name;
    void nextScreen(void);
    void prevScreen(void);
    void back(void);
    void enter(void);
	
	MenuItem* nextMenu;
	MenuItem* prevMenu;
	
    Screen* selectedScreen;
    bool entered = false;
    uint8_t sysLocX, sysLocY;
};

class Menu {
  public:
    Menu(MenuItem* menuItem);
	void addMenuItem(MenuItem* menuItem);
	
    MenuItem* selectedMenuItem;
    bool entered = false;
    void show(void);
    void next(void);
    void prev(void);
    void enter(void);
    void back(void);
    void refresh(void);
};

#endif
