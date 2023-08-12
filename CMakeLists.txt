cmake_minimum_required (VERSION 3.0)

set(OpenCV_DIR "thirdparty/opencv") 

project(HDR_ISP)

find_package(OpenCV REQUIRED)

message(STATUS "opencv inc path " ${OpenCV_INCLUDE_DIRS})

include_directories(thirdparty/easylog)
include_directories(srcs/include)

include_directories(thirdparty/json/nlohmann)

add_subdirectory(thirdparty/easylog)
add_subdirectory(srcs)


#aux_source_directory(srcs/sources/modules/ ISP_MOD_SRCS_LISTS)
#aux_source_directory(srcs/sources/common/  ISP_COM_SRCS_LISTS)
#add_subdirectory(sources/)

set (
    SRCS
    main.cpp
    parse.cpp
    #${ISP_MOD_SRCS_LISTS}
    #${ISP_COM_SRCS_LISTS}
)

message(STATUS "sources lists " ${SRCS})

add_executable (${PROJECT_NAME} ${SRCS})

include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIBRARY_DIRS})
target_link_libraries(${PROJECT_NAME}  ${OpenCV_LIBS} easylog modules)








