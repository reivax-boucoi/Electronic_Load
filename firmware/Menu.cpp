#include "Menu.h"


MenuItem::MenuItem(const char* n, int x, int y, Screen *s) {
  name = n;
  screens[0] = s;
  sysLocX = x;
  sysLocY = y;
}

MenuItem::MenuItem(const char* n, int x, int y, Screen *s0, Screen *s1, Screen *s2, Screen *s3) {
  name = n;

  screens[0] = s0;
  screens[1] = s1;
  screens[2] = s2;
  screens[3] = s3;

  sysLocX = x;
  sysLocY = y;
}

void MenuItem::nextScreen(void) {
  if (!entered) {
    selectedScreen++;
    if (selectedScreen > 3)selectedScreen = 0;
    while (screens[selectedScreen] == NULL) {
      selectedScreen++;
      if (selectedScreen > 3)selectedScreen = 0;
    }
    screens[selectedScreen]->show();
  } else {
    screens[selectedScreen]->next();
  }
}

void MenuItem::prevScreen(void) {
  if (!entered) {
    selectedScreen--;
    if (selectedScreen < 0)selectedScreen = 3;
    while (screens[selectedScreen] == NULL) {
      selectedScreen--;
      if (selectedScreen < 0)selectedScreen = 3;
    }
    screens[selectedScreen]->show();
  } else {
    screens[selectedScreen]->prev();
  }
}

 void MenuItem::back(void){
  //Serial.println("MenuItem back");
  screens[selectedScreen]->back();
 }

Menu::Menu(MenuItem* menuItem0, MenuItem* menuItem1, MenuItem* menuItem2, MenuItem* menuItem3) {
  menus[0] = menuItem0;
  menus[1] = menuItem1;
  menus[2] = menuItem2;
  menus[3] = menuItem3;

}
void Menu::show(void) {
  if (!entered) {
    lcd.clear();
    for (int i = 0; i < 4; i++) {
      if (menus[i] != NULL) {
        lcd.setCursor(menus[i]->sysLocX, menus[i]->sysLocY);
        if (i == selectedMenuItem) {
          lcd.print(">");
        } else {
          lcd.print(" ");
        }
        lcd.print(menus[i]->name);
      }
    }
  } else {
    menus[selectedMenuItem]->screens[menus[selectedMenuItem]->selectedScreen]->show();
  }
}


void Menu::refresh(void){
  if(entered)menus[selectedMenuItem]->screens[menus[selectedMenuItem]->selectedScreen]->refresh();
}

void Menu::next(void) {
  if (!entered) { //select next menu
    selectedMenuItem++;
    if (selectedMenuItem > 3)selectedMenuItem = 0;
    while (menus[selectedMenuItem] == NULL) {
      selectedMenuItem++;
      if (selectedMenuItem > 3)selectedMenuItem = 0;
    }
    show();
  } else {
    menus[selectedMenuItem]->nextScreen();
  }
}

void Menu::prev(void) {
  if (!entered) {
    selectedMenuItem--;
    if (selectedMenuItem < 0)selectedMenuItem = 3;
    while (menus[selectedMenuItem] == NULL) {
      selectedMenuItem--;
      if (selectedMenuItem < 0)selectedMenuItem = 3;
    }
    show();
  } else {
    menus[selectedMenuItem]->prevScreen();
  }
}

void Menu::enter(void) {
  if (entered) {
    menus[selectedMenuItem]->entered = true;
    menus[selectedMenuItem]->screens[menus[selectedMenuItem]->selectedScreen]->enter();

  } else {
    if (menus[selectedMenuItem]->screens != NULL) {
      entered = true;
      show();
    }
  }
}



void Menu::back(void) {
  if (entered && menus[selectedMenuItem]->entered) {
    if(! menus[selectedMenuItem]->screens[menus[selectedMenuItem]->selectedScreen]->valueEditing)menus[selectedMenuItem]->entered = false;
      menus[selectedMenuItem]->back();
      show();
  } else {
    entered = false;
    show();
  }
}
