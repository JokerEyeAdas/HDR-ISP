cmake_minimum_required (VERSION 3.0)

project(modules)

include_directories(../thirdparty/easylog)
include_directories(../thirdparty/json/nlohmann)
include_directories(../include)

#file(GLOB_RECURSE COMMON_FILES ./common/*.cpp ./common/*.h*)
#file(GLOB_RECURSE MODULES_FILES ./modules/*.cpp ./moudles/*.h*)
aux_source_directory(sources/modules/ ISP_MOD_SRCS_LISTS)
aux_source_directory(sources/common/  ISP_COM_SRCS_LISTS)



add_library(${PROJECT_NAME} ${ISP_MOD_SRCS_LISTS} ${ISP_COM_SRCS_LISTS})

target_link_libraries(${PROJECT_NAME} easylog)