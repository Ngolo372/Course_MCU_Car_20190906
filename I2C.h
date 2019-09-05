/*
 * I2C.h
 * I2C Operation for MSP430G2553.
 * By vrqq (vrqq3118@163.com)
 */
#ifndef I2C_H_
#define I2C_H_

#include <msp430g2553.h>

typedef unsigned char uchar;
void I2CInit(uchar);
void I2C_Write(uchar,uchar);
uchar I2C_Read(uchar);


#endif /* I2C_H_ */
