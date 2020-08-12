#include "Menu.h"


MenuItem::MenuItem(uint8_t x, uint8_t y, Screen *s) {
    selectedScreen = s;
    selectedScreen->nextScreen = s;
    selectedScreen->prevScreen = s;
    sysLocX = x;
    sysLocY = y;
}


void MenuItem::addScreen(Screen *s) {
    Screen* b = selectedScreen->nextScreen;
    selectedScreen->nextScreen = s;
    s->prevScreen = selectedScreen;
    s->nextScreen = b;
    b->prevScreen = s;
}


void MenuItem::nextScreen(void) {
    if (!entered) {
        selectedScreen = selectedScreen->nextScreen;
        selectedScreen->show();
    } else {
        selectedScreen->next();
    }
}

void MenuItem::prevScreen(void) {
    if (!entered) {
        selectedScreen = selectedScreen->prevScreen;
        selectedScreen->show();
    } else {
        selectedScreen->prev();
    }
}


void MenuItem::enter(void) {
    entered = true;
    //Serial.println(F("MenuItem enter"));
    selectedScreen->enter();

}

void MenuItem::back(void) {
    selectedScreen->back();
}

Menu::Menu(MenuItem* menuItem) {
    selectedMenuItem = menuItem;
    selectedMenuItem->nextMenu = selectedMenuItem;
    selectedMenuItem->prevMenu = selectedMenuItem;
}


void Menu::addMenuItem(MenuItem* menuItem) {
    MenuItem* b = selectedMenuItem->nextMenu;
    selectedMenuItem->nextMenu = menuItem;
    menuItem->prevMenu = selectedMenuItem;
    menuItem->nextMenu = b;
    b->prevMenu = menuItem;
}

void Menu::show(void) {
    if (!entered) {
        lcd.clear();
        MenuItem* m = selectedMenuItem;
        do {
            lcd.setCursor(m->sysLocX, m->sysLocY);
            if (m == selectedMenuItem) {
                lcd.print(">");
            } else {
                lcd.print(" ");
            }
            lcd.print(m->name);

            m = m->nextMenu;
        } while (m != selectedMenuItem);

    } else {
        selectedMenuItem->selectedScreen->show();
    }
}


void Menu::refresh(void) {
    if (entered) {
        selectedMenuItem->selectedScreen->refresh();
    }
}

void Menu::next(void) {
    if (!entered) { //select next menu
        selectedMenuItem = selectedMenuItem->nextMenu;
        //Serial.println(F("Menu next : nextmenuitem"));
        show();
    } else {
        selectedMenuItem->nextScreen();
    }
}

void Menu::prev(void) {
    if (!entered) { //select next menu
        //Serial.println(F("Menu prev : prevmenuitem"));
        selectedMenuItem = selectedMenuItem->prevMenu;
        show();
    } else {
        selectedMenuItem->prevScreen();
    }
}

void Menu::enter(void) {
    if (entered) {
        selectedMenuItem->enter();
    } else {
        entered = true;
        show();
        //Serial.println(F("Menu enter"));
    }
}



void Menu::back(void) {
    if (entered && selectedMenuItem->entered) {
        if (! selectedMenuItem->selectedScreen->valueEditing)selectedMenuItem->entered = false;
        selectedMenuItem->back();
        show();
    } else {
        entered = false;
        show();
    }
}
