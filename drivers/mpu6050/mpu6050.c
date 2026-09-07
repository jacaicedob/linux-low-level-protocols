#include "mpu6050.h"

#include <stdint.h>
#include <unistd.h>

int mpu6050_open(struct mpu6050 *mpu){
  mpu->addr = MPU6050_ADDR;
  if (open_i2c_dev(&(mpu->dev)) < 0){
    printf("Cannot open MPU-6050 device");
    return 1;
  }

  printf("Successfully opened I2C device\n");

  if (ioctl(mpu->dev.fd, I2C_SLAVE, mpu->addr) < 0){
    printf("!!! Failed to set I2C device at address as slave\n");
    return 1;
  }
  printf("Successfully set the device at addres 0x%x to slave\n", mpu->addr);

  return 0;
}

  
int mpu6050_reset(struct mpu6050 *mpu){
  char buff[1];

  // Reset device
  buff[0] = 0x80; // DEVICE_RESET
  if(i2c_write(&(mpu->dev), MPU6050_PWR_MGMT_1, buff, 2) != 2)
  {
    printf("!!! Failed to reset device\n");
    return 1;
  }

  usleep(100000); // 100ms

  return 0;
}

int mpu6050_wakeup(struct mpu6050 *mpu){
  char buff[1];

  // Reset device
  buff[0] = 0x0; // Sets SLEEP to 0
  if(i2c_write(&(mpu->dev), MPU6050_PWR_MGMT_1, buff, 2) != 2)
  {
    printf("!!! Failed to wakeup device\n");
    return 1;
  }

  usleep(100000); // 100ms
  if(i2c_write(&(mpu->dev), MPU6050_PWR_MGMT_2, buff, 2) != 2)
  {
    printf("!!! Failed to wakeup device\n");
    return 1;
  }

  usleep(100000); // 100ms
  return 0;
}

int mpu6050_read_pwr_mgmt_config(struct mpu6050 *mpu, unsigned char *config){
  char buff[2];

  if(i2c_read(&(mpu->dev), MPU6050_PWR_MGMT_1, buff, 1) != 1)
  {
    printf("!!! Failed to read power management configuration\n");
    return 1;
  }

  if(i2c_read(&(mpu->dev), MPU6050_PWR_MGMT_2, buff+1, 1) != 1)
  {
    printf("!!! Failed to read power management configuration\n");
    return 1;
  }

  *config = buff[0];
  printf("PWR_MGMT_1: 0x%x\nPWR_MGMT_2: 0x%x\n", buff[0], buff[1]);

  return 0;
}

int mpu6050_read_accel_config(struct mpu6050 *mpu, unsigned char *config){
  char buff[1];

  if(i2c_read(&(mpu->dev), MPU6050_ACCEL_CONFIG, buff, 1) != 1)
  {
    printf("!!! Failed to read accelerometer configuration\n");
    return 1;
  }

  *config = buff[0];

  return 0;
}

int mpu6050_read_gyro_config(struct mpu6050 *mpu, unsigned char *config){
  char buff[1];

  if(i2c_read(&(mpu->dev), MPU6050_GYRO_CONFIG, buff, 1) != 1)
  {
    printf("!!! Failed to read gyroscope configuration\n");
    return 1;
  }

  *config = buff[0];

  return 0;
}

int mpu6050_read_id(struct mpu6050 *mpu, unsigned char *id){
  char buff[1];

  // Read Device ID (Address 0x75, WHO_AM_I register)
  if(i2c_read(&(mpu->dev), MPU6050_WHO_AM_I, buff, 1) != 1)
  {
    printf("!!! Failed to read device id\n");
    return 1;
  }

  *id = buff[0];
  printf("Device ID: 0x%x\n", *id);

  return 0;
}

int mpu6050_read_temp(struct mpu6050 *mpu, double *temp){
  char buff[2];

  if(i2c_read(&(mpu->dev), MPU6050_TEMP_OUT_H, buff, 1) != 1)
  {
    printf("!!! Failed to read device temperature\n");
    return 1;
  }

  if(i2c_read(&(mpu->dev), MPU6050_TEMP_OUT_L, buff+1, 1) != 1)
  {
    printf("!!! Failed to read device temperature\n");
    return 1;
  }

  int16_t measurement = buff[0] << 8 | buff[1];
  *temp = (double)(measurement)/340.0 + 36.53;
  printf("Temperature: %f C\n", *temp);

  return 0;

}

int mpu6050_read_accel(size_t msb_addr, struct mpu6050 *mpu, double *result){
  char buff[2];
  size_t addr = msb_addr;

  if(i2c_read(&(mpu->dev), addr, buff, 1) != 1)
  {
    printf("!!! Failed to read accelerometer MSB at 0x%x\n", (unsigned int)addr);
    return 1;
  }

  ++addr;

  if(i2c_read(&(mpu->dev), addr, buff+1, 1) != 1)
  {
    printf("!!! Failed to read accelerometer LSB at 0x%x\n", (unsigned int)addr);
    return 1;
  }

  int16_t measurement = buff[0] << 8 | buff[1];
  unsigned char config;
  if(mpu6050_read_accel_config(mpu, &config)){
    printf("!!! Failed to read accelerometer config\n");
    return 1;
  }

  // Extract AFS_SEL from bits 4:3
  unsigned char afs_sel = (config & 0x18) >> 3;
  double lsb_sensitivity;
  switch (afs_sel) {
    case 0:
      lsb_sensitivity = 16384.0;
      break;
    case 1:
      lsb_sensitivity = 8192.0;
      break;
    case 2:
      lsb_sensitivity = 4096.0;
      break;
    case 3:
      lsb_sensitivity = 2048.0;
      break;
  }
  printf("Sensor value: %d\tAFS_SEL: %d\tLSB Sensitivity: %f\n", measurement, afs_sel, lsb_sensitivity);

  *result = (double)(measurement)/lsb_sensitivity;

  return 0;
}

int mpu6050_read_gyro(size_t msb_addr, struct mpu6050 *mpu, double *result){
  char buff[2];
  size_t addr = msb_addr;

  if(i2c_read(&(mpu->dev), addr, buff, 1) != 1)
  {
    printf("!!! Failed to read gyroscope MSB at 0x%x\n", (unsigned int)addr);
    return 1;
  }

  ++addr;

  if(i2c_read(&(mpu->dev), addr, buff+1, 1) != 1)
  {
    printf("!!! Failed to read gyroscope LSB at 0x%x\n", (unsigned int)addr);
    return 1;
  }

  int16_t measurement = buff[0] << 8 | buff[1];
  unsigned char config;
  if(mpu6050_read_gyro_config(mpu, &config)){
    printf("!!! Failed to read gyroscope config\n");
    return 1;
  }

  // Extract FS_SEL from bits 4:3
  unsigned char fs_sel = (config & 0x18) >> 3;
  double lsb_sensitivity;
  switch (fs_sel) {
    case 0:
      lsb_sensitivity = 131.0;
      break;
    case 1:
      lsb_sensitivity = 65.5;
      break;
    case 2:
      lsb_sensitivity = 32.8;
      break;
    case 3:
      lsb_sensitivity = 16.4;
      break;
  }
  printf("Sensor value: %d\tFS_SEL: %d\tLSB Sensitivity: %f\n", measurement, fs_sel, lsb_sensitivity);

  *result = (double)(measurement)/lsb_sensitivity;

  return 0;
}

