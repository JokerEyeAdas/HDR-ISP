cmake_minimum_required (VERSION 3.0)

project(HDR_ISP)

#if (${CMAKE_SYSTEM_NAME} STREQUAL  "Windows")
    #message(STATUS "set opencv dir to thirdparty")
    #set(OpenCV_DIR "thirdparty/opencv") 
#endif()
#find_package(OpenCV REQUIRED)
#message(STATUS "opencv inc path " ${OpenCV_INCLUDE_DIRS})

include_directories(thirdparty/easylog)
include_directories(thirdparty/easybmp)
include_directories(thirdparty/json/nlohmann)
include_directories(srcs/include)

add_subdirectory(thirdparty/easylog)
add_subdirectory(thirdparty/easybmp)
add_subdirectory(srcs)


#aux_source_directory(srcs/sources/modules/ ISP_MOD_SRCS_LISTS)
#aux_source_directory(srcs/sources/common/  ISP_COM_SRCS_LISTS)
#add_subdirectory(sources/)

set (
    SRCS
    main.cpp
    parse.cpp
)

message(STATUS "sources lists " ${SRCS})

add_executable (${PROJECT_NAME} ${SRCS})

#remove opencv depend
#include_directories(${OpenCV_INCLUDE_DIRS})
#link_directories(${OpenCV_LIBRARY_DIRS})
#${OpenCV_LIBS}
target_link_libraries(${PROJECT_NAME}  easylog easybmp modules)








