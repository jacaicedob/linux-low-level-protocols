#include "mpu6050.h"
#include <fcntl.h>

int main()
{
  struct mpu6050 mpu;
  mpu.dev.id = 1;
  mpu.dev.flags = O_RDWR;

  unsigned char id;
  double temp;

  mpu6050_open(&mpu);
  mpu6050_reset(&mpu);
  mpu6050_read_id(&mpu, &id);
  mpu6050_read_temp(&mpu, &temp);

  return 0;
}
