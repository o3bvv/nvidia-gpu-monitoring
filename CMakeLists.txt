cmake_minimum_required(VERSION 3.8)

project("nvidia-gpu-monitoring")

include(CheckIncludeFiles)

check_include_files(windows.h HAVE_WINDOWS_H)
check_include_files(dlfcn.h HAVE_DLFCN_H)

add_subdirectory("monitor")
