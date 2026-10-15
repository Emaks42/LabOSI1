cmake_minimum_required(VERSION 3.10)
project(lab1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g3")
endif()
if(CMAKE_BUILD_TYPE MATCHES Release)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os -g0")
endif()


if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  message("Build for Linux")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -fsanitize=address -g -pthread")
  set(SOURCE_LIB os_interaction/POSIX)
endif ()


if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
  message("Build for Windows")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall /WX /fsanitize=address /Zi")
  set(SOURCE_LIB os_interaction/Windows)
endif ()

add_executable(${CMAKE_PROJECT_NAME}_exe main.cpp)

include_directories(os_interaction/basic)
include_directories(${SOURCE_LIB}/os_api.h)

add_library(${CMAKE_PROJECT_NAME}_lib ${SOURCE_LIB}/os.cpp)
target_link_libraries(${CMAKE_PROJECT_NAME}_exe PRIVATE ${CMAKE_PROJECT_NAME}_lib)