
cmake_minimum_required(VERSION 2.6)
project(example)
set( CMAKE_SYSTEM_NAME "linux")





set(CMAKE_C_COMPILER      arm-linux-gcc   )
set(CMAKE_CXX_COMPILER  arm-linux-g++  )
set(CMAKE_LINKER                arm-linux-ld     )

# set( CMAKE_C_FLAGS  "-Wall -O2 -Wstrict-prototypes ")
set( CMAKE_C_FLAGS  "-Wall -O2  ")
set(EXECUTABLE_OUTPUT_PATH /home/sr/share/)
# set(EXECUTABLE_OUTPUT_PATH /home/sr/share/source/TransmitProtocol/IEC104 )

# #链接库目录
#  link_directories(  /home/sr/xml-analysis/gcc-xml/pc-xml/lib   )
#  
#  
# include_directories(   /home/sr/xml-analysis/gcc-xml/pc-xml/include/libxml2  )

# /home/sr/xml-analysis/gcc-xml/pc-xml/
#包含头文件目录target_link_libraries( IEC104Slave  xml2 pthread  )
include_directories( /home/sr/xml-analysis/xml-worklib/include/libxml2  )


#链接库目录
 link_directories(  /home/sr/xml-analysis/xml-worklib/lib   )

add_executable(example main.c)

target_link_libraries(example  xml2  pthread )

