cmake_minimum_required(VERSION 2.6)
project(csv_xml)

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


include_directories(   /home/sr/xml-analysis/xml-worklib/include/libxml2  )

add_executable(csv_xml xml_create_export_csv.c xml_csv.c xml_point_list_analysis.c main.c)

target_link_libraries(csv_xml  xml2  pthread )

