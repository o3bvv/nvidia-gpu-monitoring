#ifndef _NVIDIA_GPU_MONITOR_NVML_H
#define _NVIDIA_GPU_MONITOR_NVML_H

#include <ostream>
#include <string>
#include <string_view>
#include <vector>

#include "config.h"
#include "dlib.h"


#ifdef HAVE_WINDOWS_H
  constexpr auto NVML_LIB_NAME{"nvml.dll"};
#elif HAVE_DLFCN_H
  constexpr auto NVML_LIB_NAME{"libnvidia-ml.so"};
#endif 


constexpr unsigned int NVML_DEVICE_NAME_BUFFER_SIZE{64};
constexpr unsigned int NVML_DEVICE_SERIAL_BUFFER_SIZE{30};

constexpr unsigned int NVML_SYSTEM_DRIVER_VERSION_BUFFER_SIZE{80};
constexpr unsigned int NVML_SYSTEM_NVML_VERSION_BUFFER_SIZE{80};


enum class nvmlTemperatureSensors_t {
  NVML_TEMPERATURE_GPU = 0,
  NVML_TEMPERATURE_COUNT,
};


enum class nvmlReturn_t {
  NVML_SUCCESS                       = 0,
  NVML_ERROR_UNINITIALIZED           = 1,
  NVML_ERROR_INVALID_ARGUMENT        = 2,
  NVML_ERROR_NOT_SUPPORTED           = 3,
  NVML_ERROR_NO_PERMISSION           = 4,
  NVML_ERROR_ALREADY_INITIALIZED     = 5,
  NVML_ERROR_NOT_FOUND               = 6,
  NVML_ERROR_INSUFFICIENT_SIZE       = 7,
  NVML_ERROR_INSUFFICIENT_POWER      = 8,
  NVML_ERROR_DRIVER_NOT_LOADED       = 9,
  NVML_ERROR_TIMEOUT                 = 10,
  NVML_ERROR_IRQ_ISSUE               = 11,
  NVML_ERROR_LIBRARY_NOT_FOUND       = 12,
  NVML_ERROR_FUNCTION_NOT_FOUND      = 13,
  NVML_ERROR_CORRUPTED_INFOROM       = 14,
  NVML_ERROR_GPU_IS_LOST             = 15,
  NVML_ERROR_RESET_REQUIRED          = 16,
  NVML_ERROR_OPERATING_SYSTEM        = 17,
  NVML_ERROR_LIB_RM_VERSION_MISMATCH = 18,
  NVML_ERROR_IN_USE                  = 19,
  NVML_ERROR_MEMORY                  = 20,
  NVML_ERROR_NO_DATA                 = 21,
  NVML_ERROR_VGPU_ECC_NOT_SUPPORTED  = 22,
  NVML_ERROR_UNKNOWN                 = 999,
};


typedef struct nvmlDevice_st* nvmlDevice_t;

typedef struct nvmlUtilization_st {
  unsigned int gpu;
  unsigned int memory;
} nvmlUtilization_t;


typedef nvmlReturn_t (*nvmlInit_t)(void);
typedef nvmlReturn_t (*nvmlShutdown_t)(void);
typedef  const char* (*nvmlErrorString_t)(nvmlReturn_t result);
typedef nvmlReturn_t (*nvmlSystemGetDriverVersion_t)(char* version, unsigned int length);
typedef nvmlReturn_t (*nvmlSystemGetNVMLVersion_t)(char* version, unsigned int length);
typedef nvmlReturn_t (*nvmlDeviceGetCount_t)(unsigned int* deviceCount);
typedef nvmlReturn_t (*nvmlDeviceGetHandleByIndex_t)(unsigned int index, nvmlDevice_t* device);
typedef nvmlReturn_t (*nvmlDeviceGetName_t)(nvmlDevice_t device, char* name, unsigned int length);
typedef nvmlReturn_t (*nvmlDeviceGetSerial_t)(nvmlDevice_t device, char* serial, unsigned int length);
typedef nvmlReturn_t (*nvmlDeviceGetFanSpeed_t)(nvmlDevice_t device, unsigned int* speed);
typedef nvmlReturn_t (*nvmlDeviceGetTemperature_t)(nvmlDevice_t device, nvmlTemperatureSensors_t sensorType, unsigned int* temp);
typedef nvmlReturn_t (*nvmlDeviceGetPowerUsage_t)(nvmlDevice_t device, unsigned int* power);
typedef nvmlReturn_t (*nvmlDeviceGetUtilizationRates_t)(nvmlDevice_t device, nvmlUtilization_t* utilization);


class NVML {
  public:
    typedef struct info_st {
      std::string_view driver_version;
      std::string_view nvml_version;
    } info_t;

    NVML(std::string_view lib_name = NVML_LIB_NAME);
    ~NVML();
  
    unsigned int get_devices_count_or_halt() const;
    void get_device_handle_or_halt(const unsigned int index, nvmlDevice_t& handle) const;
    std::string get_device_name_or_halt(const unsigned int index, const nvmlDevice_t& handle) const;
    unsigned int get_device_fan_speed_or_halt(const unsigned int index, const nvmlDevice_t& handle) const;
    unsigned int get_device_temperature_or_halt(const unsigned int index, const nvmlDevice_t& handle) const;
    unsigned int get_device_power_usage_or_halt(const unsigned int index, const nvmlDevice_t& handle) const;
    void get_device_utilization_rates_or_halt(const unsigned int index, const nvmlDevice_t& handle, nvmlUtilization_t& utilization) const;
    info_t get_info() const;

  private:    
    void load_lib_or_halt(std::string_view lib_name);
    void maybe_unload_lib();

    void init_nvml_or_halt();
    void maybe_shutdown_nvml_or_halt();
    void gather_info_or_halt();
    std::string get_driver_version_or_halt() const;
    std::string get_nvml_version_or_halt() const;
    void bind_functions_or_halt();

    std::string driver_version;
    std::string nvml_version;

    dlib_handle_t lib{NULL};

    nvmlInit_t nvmlInit{NULL};
    nvmlShutdown_t nvmlShutdown{NULL};
    nvmlErrorString_t nvmlErrorString{NULL};
    nvmlSystemGetDriverVersion_t nvmlSystemGetDriverVersion{NULL};
    nvmlSystemGetNVMLVersion_t nvmlSystemGetNVMLVersion{NULL};
    nvmlDeviceGetCount_t nvmlDeviceGetCount{NULL};
    nvmlDeviceGetHandleByIndex_t nvmlDeviceGetHandleByIndex{NULL};
    nvmlDeviceGetName_t nvmlDeviceGetName{NULL};
    nvmlDeviceGetFanSpeed_t nvmlDeviceGetFanSpeed{NULL};
    nvmlDeviceGetTemperature_t nvmlDeviceGetTemperature{NULL};
    nvmlDeviceGetPowerUsage_t nvmlDeviceGetPowerUsage{NULL};
    nvmlDeviceGetUtilizationRates_t nvmlDeviceGetUtilizationRates{NULL};
};


class NVMLDevice {
  public:
    typedef struct metrics_st {
      const unsigned int fan_speed;
      const unsigned int temperature;
      const unsigned int power_usage;
      const unsigned int gpu_utilization;
      const unsigned int memory_utilization;
    } metrics_t;

    typedef struct info_st {
      std::string_view name;
      const unsigned int index;
      const metrics_st metrics;      

    } info_t;

    NVMLDevice(
      const unsigned int index,
      const nvmlDevice_t handle,
      const NVML& api
    );
    ~NVMLDevice();

    void refresh_metrics_or_halt();
    info_t get_info() const;

  private:    
    const unsigned int index;
    const nvmlDevice_t handle;
    const NVML& api;

    std::string name;

    unsigned int fan_speed{0};   // in %
    unsigned int temperature{0}; // in deg. C
    unsigned int power_usage{0}; // in milliwatts

    nvmlUtilization_t utilization{0, 0};
};


class NVMLDeviceManager {
  public:
    NVMLDeviceManager(const NVML& api);
    ~NVMLDeviceManager();

    size_t get_devices_count() const;
    const std::vector<NVMLDevice>::iterator devices_begin();
    const std::vector<NVMLDevice>::iterator devices_end();

  private:    
    void detect_devices_or_halt();

    const NVML& api;
    std::vector<NVMLDevice> devices;
};


#endif // _NVIDIA_GPU_MONITOR_NVML_H