#include "Screen.h"



Value::Value(float* v, uint8_t x, uint8_t y, uint8_t digits) {
  val = v;
  posX = x;
  posY = y;
  dispVal = *val;
  nbDigits = digits;
  DPpos = 0;
}


Value::Value(float* v, uint8_t x, uint8_t y, uint8_t digits, float min, float max) {
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
}

void Value::down(void) {
  int p = nbDigits - DPpos - 1 - cursorPos;
  (*val) -=  pow(10, p);
  if ((*val) < minVal)(*val) = minVal;
  dispVal = (*val);
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

Screen::Screen(Value *v){
  selectedValue = v;
  v->nextValue=v;
  v->prevValue=v;
}


void Screen::addValue(Value *v){
	Value* b=selectedValue->nextValue;
	selectedValue->nextValue=v;
	v->prevValue=selectedValue;
	v->nextValue=b;
	b->prevValue=v;
}

void Screen::show(void) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(txt1);
  lcd.setCursor(0, 1);
  lcd.print(txt2);
  
  refresh();
}

void  Screen::refresh(void) {
	Value* v=selectedValue;
	do{
		v->refresh();
		v=v->nextValue;
	}while(v!=selectedValue);
}

void Screen::next(void) {
  if (!valueEditing) {
    Serial.println(F("Screen next : Searching for next editable value"));
    getNextValue(1);
  } else {
    Serial.println(F("Editing value : up"));
    selectedValue->up();
  }
}
void Screen::prev(void) {
  if (!valueEditing) {
    Serial.println(F("Screen prev : Searching for previous editable value"));
    getNextValue(0);
  } else {
    Serial.println(F("Editing value : down"));
    selectedValue->down();
  }
}

void Screen::enter(void) {
    if(valueEditing){
        selectedValue->advanceCursor();
    }else{
        valueEditing=true;
        Serial.println(F("Screen enter"));
        getNextValue(1);
    }

}


void Screen::back(void) {
    if(valueEditing){
        valueEditing=false;
    }
}

void Screen::getNextValue(bool dir) {
    Value* v=selectedValue;
	do{
		if(dir){
			selectedValue=selectedValue->nextValue;
		}else{
			selectedValue=selectedValue->prevValue;
		}
	}while(selectedValue->editable==false && v!=selectedValue);
    if(v==selectedValue && !selectedValue->editable){
        valueEditing=false;
        Serial.println(F("No editable value found !"));
    }
}
