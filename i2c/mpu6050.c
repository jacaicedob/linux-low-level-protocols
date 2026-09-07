#include "mpu6050.h"

#include <stdint.h>

int mpu6050_open(struct mpu6050 *mpu){
  mpu->addr = MPU6050_ADDR;
  if (open_i2c_dev(&(mpu->dev)) < 0){
    printf("Cannot open MPU-6050 device");
    return 1;
  }

  printf("Successfully opened I2C device\n");

  if (ioctl(mpu->dev.fd, I2C_SLAVE, mpu->addr) < 0){
    printf("Failed to set I2C device at address as slave\n");
    return 1;
  }
  printf("Successfully set the device at addres 0x%x to slave\n", mpu->addr);

  return 0;
}

  
int mpu6050_reset(struct mpu6050 *mpu){
  char buff[1];

  // Reset device
  buff[0] = 0x80; // DEVICE_RESET
  if(i2c_write(&(mpu->dev), 0x68, buff, 2) != 2)
  {
    printf("Failed to reset device\n");
    return 1;
  }

  return 0;
}

int mpu6050_read_id(struct mpu6050 *mpu, unsigned char *id){
  char buff[1];

  // Read Device ID (Address 0x75, WHO_AM_I register)
  if(i2c_read(&(mpu->dev), 0x75, buff, 1) != 1)
  {
    printf("Failed to read device id\n");
    return 1;
  }

  *id = buff[0];
  printf("Device ID: 0x%x\n", *id);

  return 0;
}

int mpu6050_read_temp(struct mpu6050 *mpu, double *temp){
  char buff[2];

  if(i2c_read(&(mpu->dev), 0x41, buff, 1) != 1)
  {
    printf("Failed to read device temperature\n");
    return 1;
  }

  if(i2c_read(&(mpu->dev), 0x42, buff+1, 1) != 1)
  {
    printf("Failed to read device temperature\n");
    return 1;
  }

  int16_t measurement = buff[0] << 8 | buff[1];
  *temp = (double)(measurement)/340.0 + 36.53;
  printf("Temperature: %f C\n", *temp);

  return 0;

}
