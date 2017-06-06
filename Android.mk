LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
		src/android_main.cpp \
		src/thd_engine.cpp \
		src/thd_cdev.cpp \
		src/thd_cdev_therm_sys_fs.cpp \
		src/thd_engine_default.cpp \
		src/thd_sys_fs.cpp \
		src/thd_trip_point.cpp \
		src/thd_zone.cpp \
		src/thd_zone_dynamic.cpp \
		src/thd_preference.cpp \
		src/thd_parse.cpp \
		src/thd_sensor.cpp \
		src/thd_sensor_virtual.cpp \
		src/thd_kobj_uevent.cpp \
		src/thd_cdev_gen_sysfs.cpp \
		src/thd_pid.cpp \
		src/thd_zone_generic.cpp \
		src/thd_cdev_cpufreq.cpp \
		src/thd_cdev_rapl.cpp \
		src/thd_cdev_intel_pstate_driver.cpp \
		src/thd_rapl_power_meter.cpp \
		src/thd_cdev_rapl_dram.cpp \
		src/thd_int3400.cpp

ifneq ($(THERMALD_GENERATE_CONFIG), false)
LOCAL_SRC_FILES += src/thd_trt_art_reader.cpp
LOCAL_CFLAGS += -DGENERATE_CONFIG
endif

ifneq ($(THERMALD_DETECT_THERMAL_ZONES), false)
LOCAL_SRC_FILES += \
		src/thd_zone_cpu.cpp \
		src/thd_zone_therm_sys_fs.cpp \
		src/thd_cdev_order_parser.cpp \
		src/thd_cdev_kbl_amdgpu.cpp \
		src/thd_sensor_kbl_amdgpu_power.cpp \
		src/thd_sensor_kbl_amdgpu_thermal.cpp \
		src/thd_zone_kbl_g_mcp.cpp \
		src/thd_sensor_kbl_g_mcp.cpp \
		src/thd_zone_kbl_amdgpu.cpp
LOCAL_CFLAGS += -DDETECT_THERMAL_ZONES
endif

ifneq ($(THERMALD_BACKLIGHT_COOLING_DEVICE), false)
LOCAL_SRC_FILES += src/thd_cdev_backlight.cpp
LOCAL_CFLAGS += -DBACKLIGHT_COOLING_DEVICE
endif

LOCAL_C_INCLUDES += external/libxml2/include

LOCAL_PROPRIETARY_MODULE := true
LOCAL_CFLAGS += \
		-DTDRUNDIR='"/data/misc/thermald"'\
		-DTDCONFDIR='"/vendor/etc/thermald"'\
		-Wno-unused-parameter\
		-Wall\
		-Werror

LOCAL_STATIC_LIBRARIES := libxml2
LOCAL_SHARED_LIBRARIES := liblog libcutils libdl libc++ libicuuc libutils
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE := thermald
include $(BUILD_EXECUTABLE)
