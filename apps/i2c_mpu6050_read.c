#include "mpu6050.h"
#include <fcntl.h>

int main()
{
  struct mpu6050 mpu;
  mpu.dev.id = 1;
  mpu.dev.flags = O_RDWR;

  unsigned char id;
  unsigned char pwr_mgmt;
  double temp;
  double accel_x;
  double accel_y;
  double accel_z;
  double gyro_x;
  double gyro_y;
  double gyro_z;

  mpu6050_open(&mpu);
  mpu6050_reset(&mpu);
  mpu6050_read_pwr_mgmt_config(&mpu, &pwr_mgmt);
  mpu6050_wakeup(&mpu);
  mpu6050_read_pwr_mgmt_config(&mpu, &pwr_mgmt);
  mpu6050_read_id(&mpu, &id);
  mpu6050_read_temp(&mpu, &temp);

  mpu6050_read_accel(MPU6050_ACCEL_XOUT_MSB, &mpu, &accel_x);
  mpu6050_read_accel(MPU6050_ACCEL_YOUT_MSB, &mpu, &accel_y);
  mpu6050_read_accel(MPU6050_ACCEL_ZOUT_MSB, &mpu, &accel_z);
  printf("Accelerometer data:\n\tx: %f\n\ty: %f\n\tz: %f\n", accel_x, accel_y, accel_z);

  mpu6050_read_gyro(MPU6050_GYRO_XOUT_MSB, &mpu, &gyro_x);
  mpu6050_read_gyro(MPU6050_GYRO_YOUT_MSB, &mpu, &gyro_y);
  mpu6050_read_gyro(MPU6050_GYRO_ZOUT_MSB, &mpu, &gyro_z);
  printf("Gyroscope data:\n\tx: %f\n\ty: %f\n\tz: %f\n", gyro_x, gyro_y, gyro_z);

  return 0;
}
