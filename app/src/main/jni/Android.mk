LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := myNdkDemo
LOCAL_C_INCLUDES  := memblock.h Packet.h
LOCAL_SRC_FILES := memblock.cpp Packet.cpp myNdkDemo.cpp

include $(BUILD_SHARED_LIBRARY)