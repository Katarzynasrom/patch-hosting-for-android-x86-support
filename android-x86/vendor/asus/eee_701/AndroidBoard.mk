LOCAL_PATH := $(call my-dir)

ifeq ($(TARGET_PREBUILT_KERNEL),)
TARGET_PREBUILT_KERNEL := $(LOCAL_PATH)/kernel
endif

#ifeq ($(INSTALLED_BOOTIMAGE_TARGET),)
#INSTALLED_BOOTIMAGE_TARGET := $(PRODUCT_OUT)/boot.img
#endif

#ifeq ($(INSTALLED_RAMDISK_TARGET),)
#INSTALLED_RAMDISK_TARGET := $(PRODUCT_OUT)/ramdisk.img
#endif

###############################################################
############## Generate the kernel with command line ##########
file := $(INSTALLED_KERNEL_TARGET)
ALL_PREBUILT += $(file)
$(file): $(TARGET_PREBUILT_KERNEL) | $(ACP)
	$(transform-prebuilt-to-target)


####################################################################
####################################################################

# When adding GL support, will have to specify this?
#-include vendor/intel/g945/Android.mk

################### TEMP HACK!?!
LOCAL_PATH := vendor/asus/eee_701
include $(CLEAR_VARS)

kernel_modules := \
	atl2.ko \
	bitblit.ko \
	cfbcopyarea.ko \
	cfbfillrect.ko \
	cfbimgblt.ko \
	fbcon.ko \
	font.ko \
	softcursor.ko \
	drm.ko \
	i915.ko

installed_kernel_modules := \
	$(addprefix $(TARGET_ROOT_OUT)/lib/modules/,$(kernel_modules))
$(TARGET_ROOT_OUT)/lib/modules/%.ko: $(LOCAL_PATH)/modules/%.ko | $(ACP)
	$(transform-prebuilt-to-target)
$(INSTALLED_KERNEL_TARGET): $(installed_kernel_modules)
ALL_PREBUILT += $(installed_kernel_modules)

# include more board specific stuff here? Audio params?! <shrug>


####################################################################

# Lets install our own init.rc files :)
# We will also make the ramdisk depend on it so that it's always pulled in.

LOCAL_PATH := vendor/asus/eee_701
include $(CLEAR_VARS)

target_init_rc_file := $(TARGET_ROOT_OUT)/init.rc
$(target_init_rc_file) : $(LOCAL_PATH)/init.rc | $(ACP)
	$(transform-prebuilt-to-target)
ALL_PREBUILT += $(target_init_rc_file)

target_hw_init_rc_file := $(TARGET_ROOT_OUT)/init.eee_701.rc
$(target_hw_init_rc_file) : $(LOCAL_PATH)/init.eee_701.rc | $(ACP)
	$(transform-prebuilt-to-target)
ALL_PREBUILT += $(target_hw_init_rc_file)

$(INSTALLED_RAMDISK_TARGET): $(target_init_rc_file) $(target_hw_init_rc_file)

# and our initialization script
file := $(TARGET_OUT)/etc/init.eee_701.sh
$(file) : $(LOCAL_PATH)/init.eee_701.sh | $(ACP)
	$(transform-prebuilt-to-target)
ALL_PREBUILT += $(file)


####################################################################
### Include input devices specific files

include $(CLEAR_VARS)

file := $(TARGET_OUT_KEYLAYOUT)/AT_Translated_Set_2_keyboard.kl
ALL_PREBUILT += $(file)
$(file): $(LOCAL_PATH)/AT_Translated_Set_2_keyboard.kl | $(ACP)
	$(transform-prebuilt-to-target)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := AT_Translated_Set_2_keyboard.kcm
include $(BUILD_KEY_CHAR_MAP)
