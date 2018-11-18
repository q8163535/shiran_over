cmake_minimum_required(VERSION 2.6)
project(iec61850_v2)

set( CMAKE_SYSTEM_NAME "linux")
# set( CMAKE_FIND_ROOTPATH  "/home/sr/tools/Cross-4.8.4/usr")
# set(CMAKE_C_COMPILER     /home/sr/tools/Cross-4.8.4/usr/bin/arm-linux-gcc )
# set(CMAKE_CXX_COMPILER  /home/sr/tools/Cross-4.8.4/usr/bin/arm-linux-g++)
# set(CMAKE_LINKER  /home/sr/tools/Cross-4.8.4/usr/bin/arm-linux-ld )
# set( CMAKE_FIND_ROOTPATH  "/home/sr/tools/Cross-4.8.4/usr")

set(CMAKE_C_COMPILER      arm-linux-gcc   )
set(CMAKE_CXX_COMPILER  arm-linux-g++  )
set(CMAKE_LINKER                arm-linux-ld     )

# set( CMAKE_C_FLAGS  "-Wall -O2 -Wstrict-prototypes ")
set( CMAKE_C_FLAGS  "-Wall -O2  ")
# set(EXECUTABLE_OUTPUT_PATH /home/sr/share/)
set(EXECUTABLE_OUTPUT_PATH /mnt/hgfs/share-2)
add_subdirectory( source )

# add_subdirectory( source//src   )
# add_subdirectory( source/modbus_m/inc/ )
include_directories( source      source/iec61850_t/inc  /home/sr/xml-analysis/xml-worklib/include/libxml2  )

aux_source_directory( source/   SRC_LIST1)
aux_source_directory(  source/iec61850_t/src/   SRC_LIST2)

add_executable(  iec61850        ${SRC_LIST1}   ${SRC_LIST2} )

#  link_directories( home/sr/tools/Cross-4.8.4/usr/lib  /home/sr/xml-analysis/xml-worklib/lib   )
target_link_libraries(iec61850  xml2  pthread )


install (TARGETS iec61850 DESTINATION /home/sr/install/bin)
# install (FILES source/modbus_s/inc/ DESTINATION /home/sr/install/include)
install (DIRECTORY source/iec61850_t/inc/ DESTINATION /home/sr/install/include)

# 添加安装目标
# install (TARGETS Tutorial DESTINATION bin)
# install (FILES "${PROJECT_BINARY_DIR}/TutorialConfig.h"       
#          DESTINATION include)

