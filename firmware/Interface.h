#ifndef INTERFACE_H
#define INTERFACE_H

#include <Arduino.h>

#include "SerialCommand.h"


#define LEDR1_PIN 9

SerialCommand SCmd; 

//meter values
float i_meas = 0;
float v_meas = 0;
float p_meas = 0;

//load input values
float iset_load = .5;
float pset_load = 1;
float rset_load = 100;

enum LoadMode {CP, CC, CR, BATT};
LoadMode loadMode = CC;

//out of regulation error counter
uint8_t outOfReg=0;

//load output values
float temp = 25.0;
float v_batt = 12.0;
float vdisp_load = 0;
float idisp_load = 0;
float pdisp_load = 0;

//lost power
float p_loss = 0;

float refresh_rate = .5; //acquisition time
float fan_temp = 32.0; //fan temp
float eff = 0;

//battery values
float iset_batt = 0.5;
float vcutoff_batt = 3.0;
float capAh_batt = 0.0;
float capWh_batt = 0.0;


void interface_init(void);






#endif
