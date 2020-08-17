#ifndef SCREEN_H
#define SCREEN_H


#include "Arduino.h"
#include <LiquidCrystal.h>
extern LiquidCrystal lcd;

class Screen;
class Value;

class Value {
  public:
    Value(float* v, uint8_t x, uint8_t y, uint8_t digits);
    Value(float* v, uint8_t x, uint8_t y, uint8_t digits, float min, float max);
	
	Value *nextValue;
	Value *prevValue;
	
    void up(void);
    void down(void);
    void advanceCursor(void);
    void show(void);
    void refresh(void);
    void showCursor(bool editing);
    float* val;
    float dispVal;
    float minVal;
    float maxVal;
    
    uint8_t posX;
    uint8_t posY;
    uint8_t nbDigits;
    uint8_t DPpos; //nb of significant digits
    bool editable=false;
    uint8_t cursorPos=0;
};


class Screen {
  public:
    Screen(Value *v);
	void addValue(Value *v);
    void show(void);
    void refresh(void);
    void enter(void);
    void next(void);
    void prev(void);
    void back(void);
	Screen* nextScreen;
	Screen* prevScreen;
	
    bool valueEditing=false;
    const __FlashStringHelper* txt1;
    const __FlashStringHelper* txt2;
    
    bool entered = false;
    void getNextValue(bool dir);
    Value *selectedValue;
};


#endif
