cmake_minimum_required(VERSION 3.5)
project(hdr_isp)

set(ROS2_ENABLE false)

find_package(CUDA REQUIRED)

message(STATUS "Cuda Verison: " ${CUDA_VERSION_STRING})

include_directories(${CUDA_LNCLUDE_DIRS})

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

if (ROS2_ENABLE)
  add_definitions(-DROS2_SUPPORT=1)
  find_package(ament_cmake REQUIRED)
  find_package(rclcpp REQUIRED)
  find_package(rclcpp_components)
  find_package(std_msgs REQUIRED)
  find_package(sensor_msgs REQUIRED)
endif()

include_directories(thirdparty/easylog)
include_directories(thirdparty/easybmp)
include_directories(thirdparty/json/nlohmann)
include_directories(libs/include)

add_subdirectory(thirdparty/easylog)
add_subdirectory(thirdparty/easybmp)
#add_subdirectory(srcs)

if (CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
  message(STATUS "system is linux")
  link_directories(libs/linux)
else(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
  message(STATUS "system is windows")
  link_directories(libs/win)
endif()

set (
    SRCS
    hdr_isp_main.cpp
    hdr_isp_parse.cpp
    hdr_isp_node.cpp
)

cuda_add_executable (hdr_isp ${SRCS})
target_link_libraries(${PROJECT_NAME}  easylog easybmp modules)

add_executable(pub_raw ./tools/cpp/pub_raw_tool.cpp)
target_link_libraries(pub_raw  easylog)

if (ROS2_ENABLE)

  ament_target_dependencies(hdr_isp rclcpp std_msgs sensor_msgs rclcpp_components)
  ament_target_dependencies(pub_raw rclcpp std_msgs sensor_msgs rclcpp_components)

  install(TARGETS
    hdr_isp
    pub_raw
    DESTINATION lib/${PROJECT_NAME})

  if(BUILD_TESTING)
    find_package(ament_lint_auto REQUIRED)
    ament_lint_auto_find_test_dependencies()
  endif()

  ament_package()
endif()
