#ifndef _NVIDIA_GPU_MONITOR_DLIB_WINDOWS_H
#define _NVIDIA_GPU_MONITOR_DLIB_WINDOWS_H

#include <windows.h>

typedef HMODULE dlib_handle_t;
typedef FARPROC dfunc_handle_t;

constexpr auto LIBS_PATH_ENV_VAR{"PATH"};

#endif // _NVIDIA_GPU_MONITOR_DLIB_WINDOWS_H