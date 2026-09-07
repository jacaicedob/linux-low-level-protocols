#ifndef MPU6050H
#define MPU6050H

#include "i2c-driver.h"

#include <stdlib.h>

#define MPU6050_ADDR 0x68

#define MPU6050_GYRO_CONFIG       0x1B
#define MPU6050_ACCEL_CONFIG      0x1C
#define MPU6050_ACCEL_XOUT_MSB    0x3B
#define MPU6050_ACCEL_XOUT_LSB    0x3C
#define MPU6050_ACCEL_YOUT_MSB    0x3D
#define MPU6050_ACCEL_YOUT_LSB    0x3E
#define MPU6050_ACCEL_ZOUT_MSB    0x3F
#define MPU6050_ACCEL_ZOUT_LSB    0x40
#define MPU6050_TEMP_OUT_H        0x41
#define MPU6050_TEMP_OUT_L        0x42
#define MPU6050_GYRO_XOUT_MSB     0x43
#define MPU6050_GYRO_XOUT_LSB     0x44
#define MPU6050_GYRO_YOUT_MSB     0x45
#define MPU6050_GYRO_YOUT_LSB     0x46
#define MPU6050_GYRO_ZOUT_MSB     0x47
#define MPU6050_GYRO_ZOUT_LSB     0x48
#define MPU6050_PWR_MGMT_1        0X6B
#define MPU6050_PWR_MGMT_2        0X6C
#define MPU6050_WHO_AM_I          0x75

struct mpu6050{
  struct i2c_device dev;
  int addr;
};

int mpu6050_open(struct mpu6050 *mpu);
int mpu6050_reset(struct mpu6050 *mpu);
int mpu6050_wakeup(struct mpu6050 *mpu);
int mpu6050_read_pwr_mgmt_config(struct mpu6050 *mpu, unsigned char *config);
int mpu6050_read_accel_config(struct mpu6050 *mpu, unsigned char *config);
int mpu6050_read_gyro_config(struct mpu6050 *mpu, unsigned char *config);
int mpu6050_read_id(struct mpu6050 *mpu, unsigned char *id);
int mpu6050_read_temp(struct mpu6050 *mpu, double *temp);
int mpu6050_read_accel(size_t msb_addr, struct mpu6050 *mpu, double *result);
int mpu6050_read_gyro(size_t msb_addr, struct mpu6050 *mpu, double *result);

#endif
