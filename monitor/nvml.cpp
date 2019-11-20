#include <string>

#include "nvml.h"
#include "utils.h"


NVML::NVML(std::string_view lib_name) {
  load_lib_or_halt(lib_name);
  bind_functions_or_halt();
  init_nvml_or_halt();
  gather_info_or_halt();
}


void NVML::load_lib_or_halt(std::string_view lib_name) {
  lib = load_dlib_or_halt(lib_name);
}


void NVML::bind_functions_or_halt() {
  nvmlShutdown                  = reinterpret_cast<nvmlShutdown_t                 >(load_dfunc_or_halt(lib, "nvmlShutdown"));
  nvmlInit                      = reinterpret_cast<nvmlInit_t                     >(load_dfunc_or_halt(lib, "nvmlInit"));
  nvmlErrorString               = reinterpret_cast<nvmlErrorString_t              >(load_dfunc_or_halt(lib, "nvmlErrorString"));
  nvmlSystemGetDriverVersion    = reinterpret_cast<nvmlSystemGetDriverVersion_t   >(load_dfunc_or_halt(lib, "nvmlSystemGetDriverVersion"));
  nvmlSystemGetNVMLVersion      = reinterpret_cast<nvmlSystemGetNVMLVersion_t     >(load_dfunc_or_halt(lib, "nvmlSystemGetNVMLVersion"));
  nvmlDeviceGetCount            = reinterpret_cast<nvmlDeviceGetCount_t           >(load_dfunc_or_halt(lib, "nvmlDeviceGetCount"));
  nvmlDeviceGetHandleByIndex    = reinterpret_cast<nvmlDeviceGetHandleByIndex_t   >(load_dfunc_or_halt(lib, "nvmlDeviceGetHandleByIndex"));
  nvmlDeviceGetName             = reinterpret_cast<nvmlDeviceGetName_t            >(load_dfunc_or_halt(lib, "nvmlDeviceGetName"));
  nvmlDeviceGetFanSpeed         = reinterpret_cast<nvmlDeviceGetFanSpeed_t        >(load_dfunc_or_halt(lib, "nvmlDeviceGetFanSpeed"));
  nvmlDeviceGetTemperature      = reinterpret_cast<nvmlDeviceGetTemperature_t     >(load_dfunc_or_halt(lib, "nvmlDeviceGetTemperature"));
  nvmlDeviceGetPowerUsage       = reinterpret_cast<nvmlDeviceGetPowerUsage_t      >(load_dfunc_or_halt(lib, "nvmlDeviceGetPowerUsage"));
  nvmlDeviceGetUtilizationRates = reinterpret_cast<nvmlDeviceGetUtilizationRates_t>(load_dfunc_or_halt(lib, "nvmlDeviceGetUtilizationRates"));
}


void NVML::init_nvml_or_halt() {
  if (
    auto nv_status = nvmlInit();
    nv_status != nvmlReturn_t::NVML_SUCCESS
   ) {
    halt("failed to init NVML: " + std::string(nvmlErrorString(nv_status)));
  }
}


void NVML::gather_info_or_halt() {
  driver_version = get_driver_version_or_halt();
  nvml_version   = get_nvml_version_or_halt();
}


std::string NVML::get_driver_version_or_halt() const {
  char value[NVML_SYSTEM_DRIVER_VERSION_BUFFER_SIZE];

  if (
    auto nv_status = nvmlSystemGetDriverVersion(value, NVML_SYSTEM_DRIVER_VERSION_BUFFER_SIZE);
    nv_status != nvmlReturn_t::NVML_SUCCESS
    ) {
    halt("failed to get driver version: " + std::string(nvmlErrorString(nv_status)));
  }

  return std::string(value);
}


std::string NVML::get_nvml_version_or_halt() const {

  char value[NVML_SYSTEM_NVML_VERSION_BUFFER_SIZE];
  if (
    auto nv_status = nvmlSystemGetNVMLVersion(value, NVML_SYSTEM_NVML_VERSION_BUFFER_SIZE);
    nv_status != nvmlReturn_t::NVML_SUCCESS
    ) {
    halt("failed to get NVML version: " + std::string(nvmlErrorString(nv_status)));
  }

  return std::string(value);
}


NVML::~NVML() {
  maybe_shutdown_nvml_or_halt();
  maybe_unload_lib();
}


void NVML::maybe_shutdown_nvml_or_halt() {
  if (nvmlShutdown == NULL) {
    return;
  }

  if (
    auto nv_status = nvmlShutdown();
    nv_status != nvmlReturn_t::NVML_SUCCESS
    ) {
    halt("failed to shut down NVML: " + std::string(nvmlErrorString(nv_status)));
  }
}


void NVML::maybe_unload_lib() {
  if (lib != NULL) {
    unload_dlib(lib);
  }
}


unsigned int NVML::get_devices_count_or_halt() const {
  unsigned int device_count{0};

  if (
    auto nv_status = nvmlDeviceGetCount(&device_count);
    nv_status != nvmlReturn_t::NVML_SUCCESS
  ) {
    halt("failed to get devices count: " + std::string(nvmlErrorString(nv_status)));
  }

  return device_count;
}


void NVML::get_device_handle_or_halt(const unsigned int index, nvmlDevice_t& handle) const {
  if (
    auto nv_status = nvmlDeviceGetHandleByIndex(index, &handle);
    nv_status != nvmlReturn_t::NVML_SUCCESS
  ) {
    halt(
      "failed to get handle for device #" + std::to_string(index) +
      ": " + std::string(nvmlErrorString(nv_status))
    );
  }
}


std::string NVML::get_device_name_or_halt(const unsigned int index, const nvmlDevice_t& handle) const {
  char value[NVML_DEVICE_NAME_BUFFER_SIZE];

  if (
    auto nv_status = nvmlDeviceGetName(handle, value, NVML_DEVICE_NAME_BUFFER_SIZE);
    nv_status != nvmlReturn_t::NVML_SUCCESS
  ) {
    halt(
      "failed to get name for device #" + std::to_string(index) +
      ": " + std::string(nvmlErrorString(nv_status))
    );
  }

  return std::string(value);
}


unsigned int NVML::get_device_fan_speed_or_halt(const unsigned int index, const nvmlDevice_t& handle) const {
  unsigned int value;

  if (
    auto nv_status = nvmlDeviceGetFanSpeed(handle, &value);
    nv_status != nvmlReturn_t::NVML_SUCCESS
    ) {
    halt(
      "failed to get fan speed for device #" + std::to_string(index) +
      ": " + std::string(nvmlErrorString(nv_status))
    );
  }

  return value;
}


unsigned int NVML::get_device_temperature_or_halt(const unsigned int index, const nvmlDevice_t& handle) const {
  unsigned int value;

  if (
    auto nv_status = nvmlDeviceGetTemperature(handle, nvmlTemperatureSensors_t::NVML_TEMPERATURE_GPU, &value);
    nv_status != nvmlReturn_t::NVML_SUCCESS
    ) {
    halt(
      "failed to get temperature for device #" + std::to_string(index) +
      ": " + std::string(nvmlErrorString(nv_status))
    );
  }

  return value;
}


unsigned int NVML::get_device_power_usage_or_halt(const unsigned int index, const nvmlDevice_t& handle) const {
  unsigned int value;

  if (
    auto nv_status = nvmlDeviceGetPowerUsage(handle, &value);
    nv_status != nvmlReturn_t::NVML_SUCCESS
    ) {
    halt(
      "failed to get power usage for device #" + std::to_string(index) +
      ": " + std::string(nvmlErrorString(nv_status))
    );
  }

  return value;
}


void NVML::get_device_utilization_rates_or_halt(const unsigned int index, const nvmlDevice_t& handle, nvmlUtilization_t& utilization) const {
  if (
    auto nv_status = nvmlDeviceGetUtilizationRates(handle, &utilization);
    nv_status != nvmlReturn_t::NVML_SUCCESS
    ) {
    halt(
      "failed to get utilization rates for device #" + std::to_string(index) +
      ": " + std::string(nvmlErrorString(nv_status))
    );
  }
}


NVML::info_t NVML::get_info() const {
  return NVML::info_t{
    driver_version,
    nvml_version
  };
}


NVMLDevice::NVMLDevice(
  const unsigned int index,
  const nvmlDevice_t handle,
  const NVML& api
): index{index},
   handle{handle},
   api{api}
{
  name = api.get_device_name_or_halt(index, handle);
  refresh_metrics_or_halt();
}


void NVMLDevice::refresh_metrics_or_halt() {
  fan_speed = api.get_device_fan_speed_or_halt(index, handle);
  temperature = api.get_device_temperature_or_halt(index, handle);
  power_usage = api.get_device_power_usage_or_halt(index, handle);
  api.get_device_utilization_rates_or_halt(index, handle, utilization);
}


NVMLDevice::info_t NVMLDevice::get_info() const {
  return NVMLDevice::info_t{
    name,
    index,
    NVMLDevice::metrics_t{
      fan_speed,
      temperature,
      power_usage,
      utilization.gpu,
      utilization.memory,
    }
  };
}


NVMLDevice::~NVMLDevice() {
}


NVMLDeviceManager::NVMLDeviceManager(const NVML& api): api(api) {
  detect_devices_or_halt();
}


NVMLDeviceManager::~NVMLDeviceManager() {
  devices.clear();
}


void NVMLDeviceManager::detect_devices_or_halt() {  
  const unsigned int device_count = api.get_devices_count_or_halt();

  if (device_count == 0) {
    halt("found no devices");
  }
  
  for (unsigned int device_index{0}; device_index < device_count; ++device_index) {
    nvmlDevice_t handle;
    api.get_device_handle_or_halt(device_index, handle);

    NVMLDevice device{device_index, handle, api};
    devices.push_back(device);
  }
}


size_t NVMLDeviceManager::get_devices_count() const {
  return devices.size();
}


const std::vector<NVMLDevice>::iterator NVMLDeviceManager::devices_begin() {
  return devices.begin();
}


const std::vector<NVMLDevice>::iterator NVMLDeviceManager::devices_end() {
  return devices.end();
}