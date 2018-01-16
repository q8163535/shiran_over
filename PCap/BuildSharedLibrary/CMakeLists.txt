
add_subdirectory(MD5)

aux_source_directory(MD5 DIR_SRCS)

include_directories(MD5/include )

#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/BuildSharedLibrary/MD5)
#echo "${PROJECT_BINARY_DIR}/library"
#set(EXECUTABLE_OUTPUT_PATH /mnt/hgfs/share)

add_library(MD5 SHARED ${DIR_SRCS})
