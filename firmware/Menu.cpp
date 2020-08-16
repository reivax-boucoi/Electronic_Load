#include "Menu.h"


MenuItem::MenuItem(uint8_t x, uint8_t y, Screen *s) {
	selectedScreen = s;
	selectedScreen->nextScreen=s;
	selectedScreen->prevScreen=s;
	sysLocX = x;
	sysLocY = y;
}


void MenuItem::addScreen(Screen *s){
	Screen* b=selectedScreen->nextScreen;
	selectedScreen->nextScreen=s;
	s->prevScreen=selectedScreen;
	s->nextScreen=b;
	b->prevScreen=s;
}


void MenuItem::nextScreen(void) {
  if (!entered) {
    selectedScreen=selectedScreen->nextScreen;
    selectedScreen->show();
  } else {
    selectedScreen->next();
  }
}

void MenuItem::prevScreen(void) {
  if (!entered) {
    selectedScreen=selectedScreen->prevScreen;
    selectedScreen->show();
  } else {
    selectedScreen->prev();
  }
}


void MenuItem::enter(void){
    if(entered){
        selectedScreen->enter();
    }else{
        entered=true;
        Serial.println(F("MenuItem enter"));
    }
}

 void MenuItem::back(void){
    if(entered){
        if(selectedScreen->valueEditing){
            selectedScreen->back();
        }else{
            entered=false;
           lcd.noBlink();
        }
    }
 }

Menu::Menu(MenuItem* menuItem) {
  selectedMenuItem = menuItem;
  selectedMenuItem->nextMenu=selectedMenuItem;
  selectedMenuItem->prevMenu=selectedMenuItem;
}


void Menu::addMenuItem(MenuItem* menuItem){
	MenuItem* b=selectedMenuItem->nextMenu;
	selectedMenuItem->nextMenu=menuItem;
	menuItem->prevMenu=selectedMenuItem;
	menuItem->nextMenu=b;
	b->prevMenu=menuItem;
}

void Menu::show(void) {
  if (!entered) {
    lcd.clear();
	MenuItem* m=selectedMenuItem;
	do{
		lcd.setCursor(m->sysLocX, m->sysLocY);
		if (m == selectedMenuItem) {
		  lcd.print(">");
		} else {
		  lcd.print(" ");
		}
		lcd.print(m->name);
      
	m=m->nextMenu;
	}while(m!=selectedMenuItem);
	
  } else {
    selectedMenuItem->selectedScreen->show();
    if(selectedMenuItem->entered){
        selectedMenuItem->selectedScreen->selectedValue->showCursor(selectedMenuItem->selectedScreen->valueEditing);
    }
  }
}


void Menu::refresh(void){
  if(entered){
    selectedMenuItem->selectedScreen->refresh();
    if(selectedMenuItem->entered){
        selectedMenuItem->selectedScreen->selectedValue->showCursor(selectedMenuItem->selectedScreen->valueEditing);
    }
  }
}

void Menu::next(void) {
  if (!entered) { //select next menu
    selectedMenuItem=selectedMenuItem->nextMenu;
    Serial.println(F("Menu next : nextmenuitem"));
  } else {
    selectedMenuItem->nextScreen();
  }
    show();
}

void Menu::prev(void) {
 if (!entered) { //select next menu
    Serial.println(F("Menu prev : prevmenuitem"));
    selectedMenuItem=selectedMenuItem->prevMenu;
  } else {
    selectedMenuItem->prevScreen();
  }
    show();
}

void Menu::enter(void) {
    if(entered){
        selectedMenuItem->enter();
    }else{
        entered=true;
        Serial.println(F("Menu enter"));
    }
    show();
}



void Menu::back(void) {
    if(entered){
        if(selectedMenuItem->entered==false){
            entered=false;
        }else{
            selectedMenuItem->back();
        }
    }//no else, we're at root
    show();
}
