project(str_gateway)
set(CMAKE_C_COMPILER      arm-linux-gcc   )
set(CMAKE_CXX_COMPILER  arm-linux-g++  )
set(CMAKE_LINKER                arm-linux-ld     )

# set( CMAKE_C_FLAGS  "-Wall -O2 -Wstrict-prototypes ")
set( CMAKE_C_FLAGS  "-Wall -O2  ")
# set(EXECUTABLE_OUTPUT_PATH /home/sr/share/)
set(EXECUTABLE_OUTPUT_PATH /mnt/hgfs/share-2)
add_executable(str_gateway main.c)

