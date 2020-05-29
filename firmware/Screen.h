#ifndef SCREEN_H
#define SCREEN_H


#include "Arduino.h"
#include <LiquidCrystal.h>
extern LiquidCrystal lcd;

class Value {
  public:
    Value(float* v, int x, int y, int digits);
    Value(float* v, int x, int y, int digits, float min, float max, void (*c)(void));
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
    
    int posX;
    int posY;
    int nbDigits;
    int DPpos; //nb of significant digits
    bool editable=false;
    int cursorPos=0;
    void (*callback)(void);
};


class Screen {
  public:
    Screen(const char* line1, const char* line2, Value *value0, Value *value1, Value *value2, Value *value3);
    void show(void);
    void refresh(void);
    void enter(void);
    void next(void);
    void prev(void);
    void back(void);
    Value *values[4];
    bool valueEditing=false;
    
    private:
    void getNextValue(int dir);
    const char* txt1;
    const char* txt2;
    int selectedValue=-1;
};


#endif
