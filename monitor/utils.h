#ifndef _NVIDIA_GPU_MONITOR_UTILS_H
#define _NVIDIA_GPU_MONITOR_UTILS_H

#include <optional>
#include <string_view>


static const int DEFAULT_EXIT_CODE = -1;


void halt(
  std::optional<std::string_view> reason = std::nullopt,
  int exit_code = DEFAULT_EXIT_CODE
);


#endif // _NVIDIA_GPU_MONITOR_UTILS_H