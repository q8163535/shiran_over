cmake_minimum_required(VERSION 2.8.7)
project(PCap)

set(CMAKE_SYSTEM_NAME "linux")
# set(CMAKE_C_COMPILER arm-linux-gcc )
# set(CMAKE_CXX_COMPILER arm-linux-g++)
# set(CMAKE_LINKER arm-linux-ld)

add_subdirectory( MainFormEntrance )
add_subdirectory( GlobalDataDefine )
add_subdirectory( PublicInterface )
add_subdirectory( CoreFuction )
add_subdirectory( BuildSharedLibrary )

#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
# set(EXECUTABLE_OUTPUT_PATH /mnt/hgfs/share-2)


aux_source_directory(PublicInterface            SRC_LIST1 )
aux_source_directory(CoreFuction                 SRC_LIST2 )
aux_source_directory(GlobalDataDefine       SRC_LIST3 )
aux_source_directory(MainFormEntrance     SRC_LIST4 )
aux_source_directory(BuildSharedLibrary     SRC_LIST5 )


include_directories(GlobalDataDefine/include  PublicInterface/include CoreFuction/include MainFormEntrance/include   BuildSharedLibrary/include)

link_directories(LinkLibraryforProject )



add_executable(PCap ${SRC_LIST1} ${SRC_LIST2}  ${SRC_LIST3} ${SRC_LIST4} ${SRC_LIST5} )



target_link_libraries(PCap  pthread  sqlite3 )
