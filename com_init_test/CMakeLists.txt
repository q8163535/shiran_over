cmake_minimum_required(VERSION 2.6)
project(testcom)

set( CMAKE_SYSTEM_NAME "linux")

# set( CMAKE_FIND_ROOTPATH  "/home/sr/tools/Cross-4.8.4/usr")
set(CMAKE_C_COMPILER      arm-linux-gcc   )
set(CMAKE_CXX_COMPILER  arm-linux-g++  )
set(CMAKE_LINKER                arm-linux-ld     )

set(EXECUTABLE_OUTPUT_PATH /mnt/hgfs/share-2)

# set(EXECUTABLE_OUTPUT_PATH /mnt/hgfs/share-2)
# add_executable(systemtest main.c net.c)

add_executable(com_test com_test.c )
