#ifndef I2C_DRIVERH
#define I2C_DRIVERH

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <linux/i2c-dev.h>

struct i2c_device{
  int id;
  char path[20];
  int fd;
  int flags;
  int addr;
};

/* Open /dev/i2c-id with flags and returns file descriptor. Returns -1 for error. */
int open_i2c_dev(struct i2c_device *dev);

/* Configure I2C device with address addr opened on file descriptor fd as I2C_SLAVE. Returns -1 for error. */
int config_i2c_slave(struct i2c_device *dev);

/* Write count bytes from buff to I2C register offset */
size_t i2c_write(struct i2c_device *dev, size_t offset, char* buffer, size_t count);

/* Read count bytes from I2C register offset */
size_t i2c_read(struct i2c_device *dev, size_t offset, char* buffer, size_t count);

#endif

