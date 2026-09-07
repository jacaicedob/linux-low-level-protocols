#ifndef MPU6050H
#define MPU6050H

#include "i2c-driver.h"

#define MPU6050_ADDR 0x68

struct mpu6050{
  struct i2c_device dev;
  int addr;
};

int mpu6050_open(struct mpu6050 *mpu);
int mpu6050_reset(struct mpu6050 *mpu);
int mpu6050_read_id(struct mpu6050 *mpu, unsigned char *id);
int mpu6050_read_temp(struct mpu6050 *mpu, double *temp);

#endif
