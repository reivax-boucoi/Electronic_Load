#include "PM.h"


void PM::init(void) {
  //i2c_write(LTC2992_addr,REG_CTRLA, 0b00001110,1); //calibrate on demand, continuous scan, sense only
    if(i2c_write(LTC2992_addr,REG_CTRLA, 0b00000110,1)!=0)Serial.println(F("Error during PM init")); //calibrate on demand, continuous scan, all voltages
  //reading rate should be less than 10Hz (16.4ms*6)
}

bool PM::dataReady(void) {
  int abc = i2c_read(LTC2992_addr,REG_ADC_STATUS, 1);
  //Serial.print(F("STATUS : 0b"));
  //Serial.print(abc, BIN);
  return (abc && 0x8F);//IADC,GPIO2,GPIO1,SENSE2,SENSE1 ready
}


void PM::readInputPower(float *v, float *i, float *p) {
  long pl = i2c_read(LTC2992_addr,REG_P1, 3);
  (*p) = pl * 0.000031875;
  long il = i2c_read(LTC2992_addr,REG_I1, 2) >> 4;
  (*i) = il * 0.000770653514; // should be 0.00125
  long vl = i2c_read(LTC2992_addr,REG_S1, 2) >> 4;
  (*v) = vl * 0.0255;
  
  if(il>4094 || vl>4094)Serial.println(F("Input measurement saturated"));
}

void PM::readOutputPower(float *v, float *i, float *p) {

  long pl = i2c_read(LTC2992_addr,REG_P2, 3);
  *p = pl * 0.000031875;
  long il = i2c_read(LTC2992_addr,REG_I2, 2) >> 4;
  *i = il * 0.001150748; // should be 0.00125;
  long vl = i2c_read(LTC2992_addr,REG_S2, 2) >> 4;
  *v = vl * 0.0255;
  
  if(il>4094 || vl>4094)Serial.println(F("Output measurement saturated"));
}

//Coefficients for a generic 10k NTC
#define T_A1 0.003354016
#define T_B1 0.000256985

float PM::readTemp(void){//TODO Alert with register ALERT1.0 when overtemp
  long v = i2c_read(LTC2992_addr,REG_G1, 2) >> 4;
  //FSR=2.048V on 12 bits (4096)
  float vsense=v*2.048/4096.0;
  
  //10k NTC (9.1K @ 22°C)
  //Resistor divider 5V-33k-Vsense-NTC-GND : vsense=NTC*5/(33k+NTC)
  //NTC(kOhm)=33*Vsense/(5-Vsense)

  //float NTC=33.0*vsense/(5-vsense);
  //float temp=1/(T_A1+T_B1*log(NTC/10.0))-273.15;
  return 1/(T_A1+T_B1*log(33.0*vsense/(5-vsense)/10.0))-273.15;
}
float PM::readBatt(void){//TODO Alert with register ALERT2.0 when under voltage?
  //long v = i2c_read(LTC2992_addr,REG_G2, 2) >> 4;
  //FSR=2.048V on 12 bits (4096)
  //Vbatt divided by 10/(10+75)
  //total : /4096*2.048*(10+75)/10=0.00425
  return (i2c_read(LTC2992_addr,REG_G2, 2)>>4)*.00425;
}
