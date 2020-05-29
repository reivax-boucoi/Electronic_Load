#include "Screen.h"



Value::Value(float* v, int x, int y, int digits) {
  val = v;
  posX = x;
  posY = y;
  dispVal = *val;
  nbDigits = digits;
  DPpos = 0;
}


Value::Value(float* v, int x, int y, int digits, float min, float max, void (*c)(void)) {
  val = v;
  posX = x;
  posY = y;
  nbDigits = digits;
  DPpos = 0;
  minVal = min;
  maxVal = max;
  editable = true;
  if ((*val) < minVal)dispVal = minVal;
  if ((*val) > maxVal)dispVal = maxVal;
  dispVal = *val;
  callback = c;
}

void Value::show(void) {
  if (dispVal < 10.0) {
    DPpos = nbDigits - 1;
  } else if (dispVal < 100.0) {
    DPpos = nbDigits - 2;
  } else {
    DPpos = 1;
  }
  if(nbDigits==0)DPpos=0;   //special case : if nbDigits is set to 0, display only a 2 digit number with no comma every time
  lcd.setCursor(posX, posY);
  lcd.print(dispVal, DPpos);
}


void Value::refresh(void) {
  if ((*val) != dispVal) {
    dispVal = (*val);
  }
  show();
}

void Value::up(void) {
  int p = nbDigits - DPpos - 1 - cursorPos;
  (*val) +=  pow(10, p);
  if ((*val) > maxVal)(*val) = maxVal;
  dispVal = (*val);
  callback();
}

void Value::down(void) {
  int p = nbDigits - DPpos - 1 - cursorPos;
  (*val) -=  pow(10, p);
  if ((*val) < minVal)(*val) = minVal;
  dispVal = (*val);
  callback();
}

void Value::advanceCursor(void) {
  cursorPos++;
  if (cursorPos >= nbDigits)cursorPos = 0;
}

void Value::showCursor(bool editing) {
  if (!editing) {
    lcd.setCursor(posX + cursorPos - 1, posY);
    lcd.noCursor();
    lcd.blink();
  } else {
    if ((nbDigits - DPpos) <= cursorPos) {
      lcd.setCursor(posX + cursorPos + 1, posY);
    } else {
      lcd.setCursor(posX + cursorPos, posY);
    }
    lcd.cursor();
    lcd.noBlink();
  }
}

Screen::Screen(const char* line1, const char* line2, Value * value0, Value * value1, Value * value2, Value * value3) {
  values[3] = value3;
  values[2] = value2;
  values[1] = value1;
  values[0] = value0;
  txt1 = line1;
  txt2 = line2;
}


void Screen::show(void) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(txt1);
  lcd.setCursor(0, 1);
  lcd.print(txt2);
  for (int i = 0; i < 4; i++) {
    if (values[i] != NULL)values[i]->show();
  }
  if (selectedValue != -1) {
    values[selectedValue]->showCursor(valueEditing);
  } else {
    lcd.noBlink();
    lcd.noCursor();
  }
}

void  Screen::refresh(void) {
  for (int i = 0; i < 4; i++) {
    if (values[i] != NULL)values[i]->refresh();
  }
  if (selectedValue != -1) {
    values[selectedValue]->showCursor(valueEditing);
  } else {
    lcd.noBlink();
    lcd.noCursor();
  }
}

void Screen::next(void) {
  if (!valueEditing) {
    //Serial.println("Searching for next editable value");
    getNextValue(1);
  } else {
    //Serial.println("Editing value : up");
    values[selectedValue]->up();
  }
  show();
}
void Screen::prev(void) {
  if (!valueEditing) {
    //Serial.println("Searching for previous editable value");
    getNextValue(-1);
  } else {
    //Serial.println("Editing value : down");
    values[selectedValue]->down();
  }
  show();
}

void Screen::enter(void) {
  if (selectedValue == -1) {//just entered the screen, select a value
    //Serial.println("Entering screen, searching for new value");
    getNextValue(1);
  } else if (!valueEditing) {//go to
    //Serial.println("Switching to value editing");
    valueEditing = true;
  } else if (valueEditing) {
    //Serial.println("Editing value : advancing cursor");
    values[selectedValue]->advanceCursor();
  }
  show();
}


void Screen::back(void) {
  if (valueEditing) {
    //Serial.println("Screen back editing");
    valueEditing = false;
    values[selectedValue]->cursorPos = 0;
    show();
  } else {
    //Serial.println("Screen back selecting");
    selectedValue = -1;
  }
}

void Screen::getNextValue(int dir) {
  int oldValue = selectedValue;
  int i = 1;
  int trials = 0;
  //Serial.print("Get next value starting at ");
  //Serial.println(selectedValue);
  while (oldValue == selectedValue && trials < 4) {
    int index = selectedValue + i;
    if (index >= 4)index -= 4;
    if (index < 0)index += 4;
    //Serial.print("Trying ");
    //Serial.println(index);
    if (values[index] != NULL) {
      if (values[index]->editable)selectedValue = index;
    }
    i += dir;
    trials++;
  }
  //Serial.print("Get next value exiting with ");
  //Serial.println(selectedValue);
}
