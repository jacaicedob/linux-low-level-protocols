add_library(i2c-driver 
  STATIC 
	  i2c-driver.c
)

target_include_directories(i2c-driver 
  PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)
