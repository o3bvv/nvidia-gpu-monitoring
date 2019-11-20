#ifndef _NVIDIA_GPU_MONITOR_DLIB_UNIX_H
#define _NVIDIA_GPU_MONITOR_DLIB_UNIX_H

#include <dlfcn.h>

typedef void* dlib_handle_t;
typedef void* dfunc_handle_t;

constexpr auto LIBS_PATH_ENV_VAR{"LD_LIBRARY_PATH"};


#endif // _NVIDIA_GPU_MONITOR_DLIB_UNIX_H