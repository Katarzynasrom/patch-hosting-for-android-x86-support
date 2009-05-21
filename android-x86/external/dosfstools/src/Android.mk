ifneq ($(TARGET_SIMULATOR), true)

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
		boot.c check.c common.c fat.c file.c io.c lfn.c dosfsck.c

LOCAL_C_INCLUDES := external/dosfstools/src

LOCAL_CFLAGS := -O2 -g -W -Wall -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64

LOCAL_MODULE := dosfsck
LOCAL_MODULE_TAGS :=
LOCAL_SYSTEM_SHARED_LIBRARIES := libc

include $(BUILD_EXECUTABLE)

endif
