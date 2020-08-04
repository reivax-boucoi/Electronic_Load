#include <avr/pgmspace.h>
#include <LiquidCrystal.h>
#include <SimpleRotary.h>
#include <Wire.h>
//#include "Interface.h"
#include "Screen.h"
#include "Menu.h"
#include "Load.h"
#include "PM.h"
#include "SerialCommand.h"




#define VERSION_NUMBER "1.3"

#define BTN_BACK_PIN A0
#define BTN_LOAD_PIN A1
#define ENC_A_PIN 12
#define ENC_B_PIN 13
#define ENC_SW_PIN 11
#define LEDR1_PIN 9
#define LEDB1_PIN 10

#define ACQ_TIME 150.0    //time in ms
#define ACQ_HOUR (ACQ_TIME/(3600.0*1000.0))

#define VOLTAGE_MAX 50.0
#define RSET_MAX 1000.0
#define RSET_MIN 0.2
#define CURRENT_MAX 5.1
#define POWER_MAX   500.0

#define OUTOFREGNB  10
#define OUTOFREGPERCENT 0.05

bool BTN_LOAD_state = true;
bool BTN_BACK_state = true;

void BTN_LOAD_check(void);
void BTN_BACK_check(void);
float calc_iload(void);
void build_menu(void);
void interface_init(void);
void interface_poll(void);

//Rotary encoder : 13 12, btn 11
SimpleRotary rotary(ENC_A_PIN, ENC_B_PIN, ENC_SW_PIN);


const int LCD_RS = 8, LCD_E = 7, LCD_D4 = 6, LCD_D5 = 5, LCD_D6 = 4, LCD_D7 = 3;
LiquidCrystal lcd = LiquidCrystal(8, 7, 6, 5, 4, 3);


PM pm;
Load load;
SerialCommand SCmd;

//meter values
float i_meas = 0;
float v_meas = 0;
float p_meas = 0;

//load input values
float iset_load = .5;
float pset_load = 1;
float rset_load = 100;

enum LoadMode {CP = 1, CC = 0, CR = 2, BATT = 3};
const char*loadName[4] = {"CC", "CP", "CR", "BATT"};
LoadMode loadMode = CC;

//out of regulation error counter
uint8_t outOfReg = 0;

//load output values
float temp = 25.0;
float v_batt = 12.0;
float vdisp_load = 0;
float idisp_load = 0;
float pdisp_load = 0;

//lost power
float p_loss = 0;

float refresh_rate = .5; //acquisition time
float fan_temp = 35.0; //fan temp
float eff = 0;

//battery values
float iset_batt = 0.5;
float vcutoff_batt = 3.0;
float capAh_batt = 0.0;
float capWh_batt = 0.0;

unsigned long time1 = 0;
unsigned long time2 = 0;



Value i_measV(&i_meas, 10, 0, 4);
Value v_measV(&v_meas, 2, 0, 4);
Value p_measV(&p_meas, 2, 1, 4);

Value i_setV(&iset_load, 5, 0, 4, 0.0, CURRENT_MAX);
Value p_setV(&pset_load, 5, 0, 4, 0.0, POWER_MAX);
Value r_setV(&rset_load, 5, 0, 4, RSET_MIN, RSET_MAX);

Value tempV(&temp, 14, 0, 0);
Value tempV1(&temp, 14, 0, 0);
Value tempV2(&temp, 14, 0, 0);
Value v_battV(&v_batt, 5, 0, 3);
Value v_loadV(&vdisp_load, 2, 1, 4);
Value v_loadV1(&vdisp_load, 2, 1, 4);
Value v_loadV2(&vdisp_load, 2, 1, 4);

Value p_loadV(&pdisp_load, 10, 1, 4);
Value i_loadV(&idisp_load, 10, 1, 4);
Value i_loadV1(&idisp_load, 10, 1, 4);

//CE screen
Value pr_loadV(&p_loss, 3, 0, 3);
Value effV(&eff, 12, 0, 0);
Value v_load_eV(&vdisp_load, 3, 1, 3);
Value v_meas_eV(&v_meas, 12, 1, 3);

Value refresh_rateV(&refresh_rate, 5, 1, 2, 0.05, 10);
Value setting2V(&fan_temp, 14, 1, 2, 20, 50);


Value vcutoff_battV(&vcutoff_batt, 11, 0, 3, 0, VOLTAGE_MAX);
Value iset_battV(&iset_batt, 2, 0, 3, 0.0, CURRENT_MAX);
Value capAh_battV(&capAh_batt, 8, 1, 4);
Value capWh_battV(&capWh_batt, 0, 1, 4);


const String myStr = String("CR R      ") + String((char)244) + String(" T   ");		//myStr.c_str()

Screen meterScreen   (&v_measV);
Screen CCloadScreen (&i_setV);
Screen CPloadScreen (&p_setV);
Screen CRloadScreen (&r_setV);
Screen CEloadScreen (&pr_loadV);
Screen settingsScreen (&v_battV);
Screen battScreen (&iset_battV);


MenuItem meterItem(0, 1, &meterScreen);
MenuItem loadItem(0, 0, &CCloadScreen);
MenuItem battItem(7, 0, &battScreen);
MenuItem settingsItem(7, 1, &settingsScreen);

Menu mainMenu(&loadItem);

uint16_t interface_nbSteps = 0;
float interface_stepsize = 0.0;
Value *inteface_varPt = &i_setV;

void setup() {
    Serial.begin(115200);
    Wire.begin();
    pm.init();
    load.init();
    interface_init();

    pinMode(ENC_A_PIN, INPUT);
    pinMode(ENC_B_PIN, INPUT);
    pinMode(ENC_SW_PIN, INPUT);
    pinMode(BTN_BACK_PIN, INPUT);
    pinMode(BTN_LOAD_PIN, INPUT);
    pinMode(LEDB1_PIN, OUTPUT);
    digitalWrite(LEDB1_PIN, 0);

    lcd.begin(16, 2);
    lcd.print(F("Electronic Load"));
    lcd.setCursor(0, 1);
    lcd.print(F("0-100V 0-5A v"));
    lcd.print(VERSION_NUMBER);
    build_menu();
    Serial.println(F("Electronic Load Started !"));
    delay(500);

    mainMenu.show();

    time1 = millis();
    time2 = millis();
}


void loop() {

    BTN_LOAD_check();
    BTN_BACK_check();
    byte i;
    i = rotary.rotate();
    if (i == 1) {
        mainMenu.next();
    } else if (i == 2) {
        mainMenu.prev();
    }
    if (rotary.push()) {
        mainMenu.enter();
    }

    if (time1 + ACQ_TIME < millis()) {  //occurs @ 10Hz

        //digitalWrite(LEDB1_PIN, HIGH);
        if (pm.dataReady()) {
            pm.readInputPower(&v_meas, &i_meas, &p_meas);
            pm.readOutputPower(&vdisp_load, &idisp_load, &pdisp_load);

            load.regulate(calc_iload(), temp);

        } /*else {
            Serial.println(F("PMACQ not ready !"));
        }*/
        if (loadMode == BATT && load.onState) { //accumulate in BATT mode
            if (vdisp_load < vcutoff_batt) { //cutoff
                load.off();
                loadMode = CC;
                Serial.print(F("Load cutoff ! Accumulated values : "));
                Serial.print(capAh_batt);
                Serial.print(F("mAh, Wh :"));
                Serial.println(capWh_batt, 3);
            } else {
                capAh_batt += idisp_load * 1000.0 * (ACQ_HOUR);
                capWh_batt += pdisp_load * (ACQ_HOUR);
            }
        }

        //digitalWrite(LEDB1_PIN, LOW);
        time1 = millis();
    } else if (time2 + refresh_rate * 1000 < millis()) { //occurs @ refreshRate(0.5s default)
        digitalWrite(LEDB1_PIN, HIGH);

        while (!pm.dataReady());

        //battery supply check
        v_batt = pm.readBatt();
        if (false && v_batt < 7.0) {
            Serial.print(F("Supply Low : "));
            Serial.print(v_batt);
            Serial.println(F("V !"));
        }
        //temperature check
        temp = pm.readTemp();
        //efficiency calculation
        if (p_meas > 0.0)eff = 100.0 * (pdisp_load / p_meas);
        eff = constrain(eff, 0, 99);
        //ploss calculation
        p_loss = p_meas - pdisp_load;
        p_loss = max(0, p_loss);


        mainMenu.refresh();

        if (interface_nbSteps && load.onState) {//wait for the load to be on to take samples
            interface_nbSteps--;
            if (interface_nbSteps == 0) {
                load.off();
            } else {
                interface_printLoad(false);
                if ((*inteface_varPt).change(interface_stepsize)) {
                    interface_nbSteps = 0;
                    load.off();
                    Serial.println(F("Sweep exceeded parameter limits"));
                }
            }
        }

        digitalWrite(LEDB1_PIN, LOW);
        time2 = millis();
    }
    interface_poll();
}

void getLoadModeFromMenu(void) {
    if (mainMenu.entered) {    //update load mode based on current menu. If not a CC/CP/CR/BATT menu keep the last mode
        MenuItem* m = mainMenu.selectedMenuItem;
        if (m->selectedScreen == &CCloadScreen) {
            loadMode = CC;
        } else if (m->selectedScreen == &CPloadScreen) {
            loadMode = CP;
        } else if (m->selectedScreen == &CRloadScreen) {
            loadMode = CR;
        } else if (m == &battItem) {
            loadMode = BATT;
            capAh_batt = 0;
            capWh_batt = 0;
        }
    }
}

void BTN_LOAD_check(void) {
    if (digitalRead(BTN_LOAD_PIN) != BTN_LOAD_state) {
        BTN_LOAD_state = !BTN_LOAD_state;
        if (!BTN_LOAD_state) {
            if (load.onState) {
                load.off();
                //Serial.println(F("Load turned off"));
            } else {
                getLoadModeFromMenu();
                load.on(calc_iload());
                /*
                    Serial.print(F("Load turned on in "));
                    Serial.print(loadName[loadMode]);
                    Serial.print(F(", current set to "));
                    Serial.println(calc_iload());
                */
            }
        }
    }
}
void BTN_BACK_check(void) {
    if (digitalRead(BTN_BACK_PIN) != BTN_BACK_state) {
        BTN_BACK_state = !BTN_BACK_state;
        if (!BTN_BACK_state) {
            mainMenu.back();
        }
    }
}
float calc_iload(void) {
    float res = 0;
    switch (loadMode) {
        case CC:
            res = iset_load;
            break;
        case CP:
            if (vdisp_load > 0.0)res = constrain(pset_load / vdisp_load, 0, CURRENT_MAX);
            break;
        case CR:
            res = constrain(vdisp_load / rset_load, 0, CURRENT_MAX);
            break;
        case BATT:
            res = iset_batt;
            break;
    }

    /*  if (load.onState && ((res * (1.0 + OUTOFREGPERCENT)) < idisp_load || (res * (1.0 - OUTOFREGPERCENT)) > idisp_load) ) {
        //  Serial.println(F("Regulation error detected"));
        if (++outOfReg > OUTOFREGNB) {
            outOfReg = 0;
            load.fault = true;
            Serial.println(F("Regulation can't keep up"));
            interface_nbSteps = 0;
        }
        } else {
        outOfReg = 0;
        }*/
    return res;
}
void build_menu(void) {
    meterScreen.txt1 = F("V       I      A");
    meterScreen.txt2 = F("P      W        ");
    CCloadScreen.txt1 = F("CC I      A T   ");
    CCloadScreen.txt2 = F("V       P      W");
    CPloadScreen.txt1 = F("CP P      W T   ");
    CPloadScreen.txt2 = F("V       I      A");
    CRloadScreen.txt1 = F("CR R      O T   ");
    CRloadScreen.txt2 = F("V       I      A");
    CEloadScreen.txt1 = F("Pr     W Ef    %");
    CEloadScreen.txt2 = F("Vo       Vi     ");
    settingsScreen.txt1 = F("Batt     V      ");
    settingsScreen.txt2 = F("Tacq    s Fan   ");
    battScreen.txt1 = F("I     A Vc     V");
    battScreen.txt2 = F("     Wh      mAh");

    meterItem.name = F("Meter");
    loadItem.name = F("Load");
    battItem.name = F("Battery");
    settingsItem.name = F("Settings");

    meterScreen.addValue(&i_measV);
    meterScreen.addValue(&p_measV);
    CCloadScreen.addValue(&tempV);
    CCloadScreen.addValue(&v_loadV);
    CCloadScreen.addValue(&p_loadV);
    CPloadScreen.addValue(&tempV1);
    CPloadScreen.addValue(&v_loadV1);
    CPloadScreen.addValue(&i_loadV);
    CRloadScreen.addValue(&tempV2);
    CRloadScreen.addValue(&v_loadV2);
    CRloadScreen.addValue(&i_loadV1);
    CEloadScreen.addValue(&effV);
    CEloadScreen.addValue(&v_load_eV);
    CEloadScreen.addValue(&v_meas_eV);
    settingsScreen.addValue(&refresh_rateV);
    settingsScreen.addValue(&setting2V);
    battScreen.addValue(&vcutoff_battV);
    battScreen.addValue(&capWh_battV);
    battScreen.addValue(&capAh_battV);

    loadItem.addScreen(&CEloadScreen);
    loadItem.addScreen(&CRloadScreen);
    loadItem.addScreen(&CPloadScreen);

    mainMenu.addMenuItem(&settingsItem);
    mainMenu.addMenuItem(&meterItem);
    mainMenu.addMenuItem(&battItem);
}

void interface_printLoad(bool header) {
    if (header) {
        Serial.println(F("Mode\tI(mA)\tV\tP(mW)\tIin(mA)\tVin\tPin(mW)\tEff(%)"));
    } else {
        Serial.print(loadName[loadMode]);
        Serial.write('\t');
        Serial.print((int)(idisp_load * 1000));
        Serial.write('\t');
        Serial.print(vdisp_load, 2);
        Serial.write('\t');
        Serial.print((int)(pdisp_load * 1000));
        Serial.write('\t');
        Serial.print((int)(i_meas * 1000));
        Serial.write('\t');
        Serial.print(v_meas, 2);
        Serial.write('\t');
        Serial.print((int)(p_meas * 1000));
        Serial.write('\t');
        Serial.print(eff, 1);
        Serial.println(F("\t"));
    }
}

    void interface_load() {
    char *arg;
    arg = SCmd.next();
    if (arg != NULL) {
        if (arg == "1") {
                load.on(calc_iload());
        } else if (arg == "0") {
        } else {
            interface_unrecognized();
        }
    }
}

void interface_mode() {
    char *arg;
    arg = SCmd.next();
    if (arg != NULL) {
        /*  if (arg == "1") {
            Serial.println(F("turning load on"));
            } else if (arg == "0") {
            Serial.println(F("turning load off"));
            } else {
            interface_unrecognized();
            }*/
    }
}
void interface_get() {
    char *arg = SCmd.next();
    if (arg != NULL) {
        interface_nbSteps = atol(arg);
        interface_stepsize = 0.0;
    }
    interface_printLoad(true);
}

void interface_sweep() {
    float a, b;
    char *arg = SCmd.next();
    if (arg != NULL) {
        a = (float)atol(arg) / 1000.0;
        arg = SCmd.next();
        if (arg != NULL) {
            b = (float)atol(arg) / 1000.0;
            arg = SCmd.next();
            if (arg != NULL) {

                interface_nbSteps = atol(arg);
                interface_stepsize = (float)(b - a) / ((float)interface_nbSteps);
                interface_nbSteps++;
                getLoadModeFromMenu();
                switch (loadMode) {
                    case CC:
                        inteface_varPt = &i_setV;
                        i_setV.change(a - iset_load);
                        break;
                    case CP:
                        inteface_varPt = &p_setV;
                        p_setV.change(a - pset_load);
                        break;
                    case CR:
                        inteface_varPt = &r_setV;
                        r_setV.change(a - rset_load);
                        break;
                    case BATT:
                        Serial.println(F("Invalid Sweep mode"));
                        return;
                        break;
                }
                Serial.print(F("Starting sweep from "));
                Serial.print(a);
                Serial.print(" to ");
                Serial.print(b);
                Serial.print(" , inc ");
                Serial.println(interface_stepsize);
                interface_printLoad(true);
                load.on(calc_iload());
                return;
            }
        }
    }
    Serial.println(F("Invalid nb of args"));
}

void interface_unrecognized() {
    if (interface_nbSteps) {
        load.off();
        interface_nbSteps = 0;
        Serial.println(F("Emergency stop !"));
    } else {
        Serial.println(F("Help Page"));
        Serial.println(F("load x : Turns the load on(x=1) or off(x=0)"));
        Serial.println(F("mode x : Set the load mode : CC=0 CP=1 CR=2 BATT=3"));
        Serial.println(F("get [x]: returns load info, x times"));
        Serial.println(F("sweep b e s : sweeps mode's parameter from b to e in s steps"));
    }
    Serial.println(F("\r\n"));
}

void interface_poll(void) {
    SCmd.readSerial();
}

void interface_init(void) {
    pinMode(LEDR1_PIN, OUTPUT);
    digitalWrite(LEDR1_PIN, 0);

    //    SCmd.addCommand("load", interface_load);
    SCmd.addCommand("mode", interface_mode);
    SCmd.addCommand("get", interface_get);
    SCmd.addCommand("sweep", interface_sweep);
    SCmd.addDefaultHandler(interface_unrecognized);
}
