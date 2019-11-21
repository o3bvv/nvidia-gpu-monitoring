cmake_minimum_required(VERSION 3.8)


configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(utils STATIC "utils.cpp" "utils.h")
target_compile_features(utils PRIVATE cxx_std_17)


if(HAVE_WINDOWS_H)
  add_library(dlib STATIC "config.h" "dlib.h" "dlib_windows.cpp" "dlib_windows.h")
elseif(HAVE_DLFCN_H)
  add_library(dlib STATIC "config.h" "dlib.h" "dlib_unix.cpp" "dlib_unix.h")
endif()

target_compile_features(dlib PRIVATE cxx_std_17)
target_link_libraries(dlib utils)

if(HAVE_DLFCN_H)
  target_link_libraries(dlib -ldl)
endif()


add_library(nvml STATIC "nvml.cpp" "nvml.h" "dlib.h" "config.h")
target_compile_features(nvml PRIVATE cxx_std_17)
target_link_libraries(nvml utils dlib)


add_executable(monitor "monitor.cpp" "monitor.h")
target_compile_features(monitor PRIVATE cxx_std_17)
target_link_libraries(monitor utils nvml)
