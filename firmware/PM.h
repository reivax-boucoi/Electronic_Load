#ifndef PM_H
#define PM_H


#define LTC2992_addr 0x6F //0x66=mass response  // was 0x6F

#define REG_CTRLA 0x00
#define REG_CTRLB 0x01
#define REG_ALERT1 0x02
#define REG_FAULT1 0x03
#define REG_NADC 0x04
#define REG_P1 0x05
#define REG_MAX_P1 0x08
#define REG_MIN_P1 0x0B
#define REG_MAX_P1_THRESHOLD 0x0E
#define REG_MIN_P1_THRESHOLD 0x11
#define REG_I1 0x14
#define REG_MAX_I1 0x16
#define REG_MIN_I1 0x18
#define REG_MAX_I1_THRESHOLD 0x1A
#define REG_MIN_I1_THRESHOLD 0x1C
#define REG_S1 0x1E
#define REG_MAX_S1 0x20
#define REG_MIN_S1 0x22
#define REG_MAX_S1_THRESHOLD 0x24
#define REG_MIN_S1_THRESHOLD 0x26
#define REG_G1 0x28
#define REG_MAX_G1 0x2A
#define REG_MIN_G1 0x2C
#define REG_MAX_G1_THRESHOLD 0x2E
#define REG_MIN_G1_THRESHOLD 0x30
#define REG_ADC_STATUS 0x32
#define REG_ALERT2 0x34
#define REG_FAULT2 0x35
#define REG_P2 0x37
#define REG_MAX_P2 0x3A
#define REG_MIN_P2 0x3D
#define REG_MAX_P2_THRESHOLD 0x40
#define REG_MIN_P2_THRESHOLD 0x43
#define REG_I2 0x46
#define REG_MAX_I2 0x48
#define REG_MIN_I2 0x4A
#define REG_MAX_I2_THRESHOLD 0x4C
#define REG_MIN_I2_THRESHOLD 0x4E
#define REG_S2 0x50
#define REG_MAX_S2 0x52
#define REG_MIN_S2 0x54
#define REG_MAX_S2_THRESHOLD 0x56
#define REG_MIN_S2_THRESHOLD 0x58
#define REG_G2 0x5A
#define REG_MAX_G2 0x5C
#define REG_MIN_G2 0x5E
#define REG_MAX_G2_THRESHOLD 0x60


#include <Arduino.h>
#include <Wire.h>
#include "Utils.h"

class PM {
  public :
    void init(void);
    bool dataReady(void);
    void readInputPower(float *v, float *i, float *p);
    void readOutputPower(float *v, float *i, float *p);
    float readTemp(void);
    float readBatt(void);
  private :
};
#endif
