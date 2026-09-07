add_library(mpu6050 
  STATIC 
    mpu6050.c
)

target_include_directories(mpu6050 
  PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(mpu6050 
  PUBLIC 
    i2c-driver
)
