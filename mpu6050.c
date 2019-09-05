/*
 * This file contains some mpu6050 operation.
 * By vrqq (vrqq3118@163.com)
 */
#include "mpu6050.h"

int Get16Bit(uchar);
void MPU6050Init()
{
	int aa = 0;

	I2CInit(SlaveAddr);
	I2C_Write(PWR_MGMT_1,0x00); //resume from sleep.
	I2C_Write(SMPLRT_DIV, 0x07);
	I2C_Write(CONFIG, 0x06);
	I2C_Write(GYRO_CONFIG, 0x18);
	I2C_Write(ACCEL_CONFIG, 0x01);

}
int Get16Bit (uchar address)
{
	uchar ho,lo;
	ho = I2C_Read(address);
	lo = I2C_Read(address+1);
	return (ho<<8)+lo;
}

// X/Y/Z-Axis Acceleration
int GetAccelX ()
{
	return Get16Bit(ACCEL_XOUT_H);
}
int GetAccelY ()
{
	return Get16Bit(ACCEL_YOUT_H);
}
int GetAccelZ ()
{
	return Get16Bit(ACCEL_ZOUT_H);
}

// X/Y/Z-Axis Angular velocity
int GetAnguX ()
{
	return Get16Bit(GYRO_XOUT_H);
}
int GetAnguY ()
{
	return Get16Bit(GYRO_YOUT_H);
}
int GetAnguZ ()
{
	return Get16Bit(GYRO_ZOUT_H);
}
