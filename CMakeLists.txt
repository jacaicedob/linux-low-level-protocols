cmake_minimum_required(VERSION 3.16)
project(embedded-protocols C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra)

add_subdirectory(protocols/i2c)
add_subdirectory(drivers/mpu6050)
add_subdirectory(apps)
