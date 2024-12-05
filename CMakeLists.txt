cmake_minimum_required(VERSION 3.16)

project(alpheratz LANGUAGES CXX VERSION 1.0)
option(CMAKE_DEBUG "CMAKE DEBUG" OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(STFLY_CPP_BASE ${PROJECT_SOURCE_DIR}/cpp/alpheratz)

file(GLOB_RECURSE CORE_SOURCES "${STFLY_CPP_BASE}/*.cpp")
file(GLOB_RECURSE TEST_SOURCES "${PROJECT_SOURCE_DIR}/tests/*.cpp")

if(APP_INSTALL_DIR)
  message(STATUS "APP_INSTALL_DIR: ${APP_INSTALL_DIR}")
  include_directories(${APP_INSTALL_DIR}/include)
  link_directories(${APP_INSTALL_DIR}/lib)
endif()
set(CMAKE_PREFIX_PATH ${APP_INSTALL_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
find_package(GTest CONFIG REQUIRED)
find_package(absl CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)

if(CMAKE_DEBUG)
  get_cmake_property(_variableNames VARIABLES)
  foreach(_variableName ${_variableNames})
    if(NOT _variableName MATCHES "^_" AND NOT _variableName MATCHES "^CMAKE")
      message("${_variableName}=${${_variableName}}")
    endif()
  endforeach()
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g") 
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG -std=c++17 -D__const__=__unused__ -pipe -W -Wall -Wno-unused-parameter -fPIE -fno-omit-frame-pointer -Wno-deprecated-declarations")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc -DNDEBUG -std=c++17 -D__const__=__unused__ -pipe -W -Wall -Wno-unused-parameter -fPIC -fno-omit-frame-pointer")
endif()

# public include
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
# for internal include
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} ${CORE_SOURCES})
add_library(${PROJECT_NAME}_shared SHARED ${CORE_SOURCES})
set(${PROJECT_NAME}_LIBS
  PRIVATE
  absl::strings
  -Wl,-Bstatic
  glog
  -l:libunwind.a
  gflags pthread
  lzma
  -Wl,-Bdynamic
)
target_link_libraries(${PROJECT_NAME} ${${PROJECT_NAME}_LIBS})
target_link_libraries(${PROJECT_NAME}_shared PRIVATE ${${PROJECT_NAME}_LIBS})


foreach(CLI_SOURCE ${CLI_SOURCES})
  get_filename_component(CLI_NAME ${CLI_SOURCE} NAME_WE)
  add_executable(${CLI_NAME} ${CLI_SOURCE})
  target_link_libraries(${CLI_NAME} ${PROJECT_NAME})
endforeach()

foreach(TEST_SOURCE ${TEST_SOURCES})
  get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
  add_executable(${TEST_NAME} ${TEST_SOURCE})
  target_link_libraries(${TEST_NAME} ${PROJECT_NAME} gtest_main gtest)
endforeach()
target_link_libraries(tests_yaml yaml-cpp pthread absl::status)
target_link_libraries(tests_zstd zstd absl::time)
