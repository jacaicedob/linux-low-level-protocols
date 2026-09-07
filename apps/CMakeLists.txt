add_executable(i2c_mpu6050_read 
  i2c_mpu6050_read.c
)

target_link_libraries(i2c_mpu6050_read 
  PRIVATE 
    mpu6050
)
