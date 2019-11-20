#include "monitor.h"


constexpr auto POLLING_PERIOD{std::chrono::milliseconds(250)};


[[noreturn]] int main() {
  NVML nvml;

  auto nvml_info = nvml.get_info();
  std::cout << "driver version:" << "\t" << nvml_info.driver_version << "\n"
            << "NVML version:"   << "\t" << nvml_info.nvml_version   << "\n"
            << "\n";

  NVMLDeviceManager device_manager{nvml};

  std::cout << "\n"
            << "devices_count:" << "\t" << device_manager.get_devices_count() << "\n"
            << "devices: "      << "\n";

  const auto devices_begin = device_manager.devices_begin();
  const auto devices_end = device_manager.devices_end();

  for (auto device = devices_begin; device != devices_end; ++device) {
    const auto& info = (*device).get_info();
    std::cout << "- index:"              << "\t\t"   << info.index << "\n"
              << "  name:"               << "\t\t\t" << info.name  << "\n"
              << "  fan_speed:"          << "\t\t"   << info.metrics.fan_speed          << "%"  << "\n"
              << "  temperature:"        << "\t\t"   << info.metrics.temperature        << "C"  << "\n"
              << "  power_usage:"        << "\t\t"   << info.metrics.power_usage        << "mW" << "\n"
              << "  gpu_utilization:"    << "\t"     << info.metrics.gpu_utilization    << "%"  << "\n"
              << "  memory_utilization:" << "\t"     << info.metrics.memory_utilization << "%"  << "\n";
  }

  std::cout << "\n\n"
            << "Monitoring GPUs with polling period of " << POLLING_PERIOD.count() << "ms"
            << "\n\n\n"
            << "timestamp_ms,index,fan_speed,temperature,power_usage,gpu_utilization,memory_utilization"
            << "\n";

  std::chrono::milliseconds timestamp;

  while (true) {
    timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::high_resolution_clock::now().time_since_epoch()
      );

    for (auto device = devices_begin; device != devices_end; ++device) {
      (*device).refresh_metrics_or_halt();
      const auto& info = (*device).get_info();

      std::cout
        << timestamp.count()               << ","
        << info.index                      << ","
        << info.metrics.fan_speed          << ","
        << info.metrics.temperature        << ","
        << info.metrics.power_usage        << ","
        << info.metrics.gpu_utilization    << ","
        << info.metrics.memory_utilization << std::endl;
    }

    std::this_thread::sleep_for(POLLING_PERIOD);
  }
}
