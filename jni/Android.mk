LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := monitor
LOCAL_SRC_FILES := uninstall.c
LOCAL_LDLIBS    := -lm -llog -landroid
include $(BUILD_SHARED_LIBRARY) 