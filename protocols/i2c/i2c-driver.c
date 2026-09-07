#include "i2c-driver.h"

int open_i2c_dev(struct i2c_device *dev){
  sprintf(dev->path, "/dev/i2c-%d", dev->id);
  dev->fd = open(dev->path, dev->flags);
  if (dev->fd < 0){
    printf("Cannot open %s\n", dev->path);
    return -1;
  }

  return 0;
}

int config_i2c_slave(struct i2c_device *dev){
  if (dev->fd <= 0){
    printf("Device not opened.");
    return 1;
  }
        
  if (ioctl(dev->fd, I2C_SLAVE, dev->addr) < 0){
    printf("Failed to set the I2C address with ioctl\n");
    return 1;
  }
  printf("Successfully set the device addres to 0x%x\n", dev->addr);
  return 0;
}

size_t i2c_write(struct i2c_device *dev, size_t offset, char* buffer, size_t count){
  char data[count+1];

  // Set I2C register offset as first byte
  data[0] = offset;
  
  if (count > 0){
    // Copy buffer to data
    for (size_t i=0; i < count; ++i){
      data[i+1] = buffer[i];
    }
  }

  if (write(dev->fd, data, count+1) != (ssize_t)(count+1)){
    printf("Error writing to device\n");
    return -1;
  }
  return count;
}
  
size_t i2c_read(struct i2c_device *dev, size_t offset, char* buffer, size_t count){
  char data[1];

  // Set I2C register offset as first byte
  data[0] = offset;

  if(write(dev->fd, data, 1) != 1){
    printf("Error writing register offset to device\n");
    return -1;
  }

  if(read(dev->fd, buffer, count) != (ssize_t)count){
    printf("Error reading from device\n");
    return 1;
  }

  return count;
}
