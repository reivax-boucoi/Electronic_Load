#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include <Wire.h>


int i2c_write(byte deviceAddr,int address, long data, int l);
long i2c_read(byte deviceAddr, int address, int l);

#endif
