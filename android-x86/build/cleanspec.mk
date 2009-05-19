# Copyright (C) 2007 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# Just bump this if you want to force a clean build.
# **********************************************************************
# WHEN DOING SO, DELETE ANY "add-clean-step" ENTRIES THAT HAVE PILED UP.
# **********************************************************************
#
INTERNAL_CLEAN_BUILD_VERSION := 2
#
# ***********************************************************************
# Do not touch INTERNAL_CLEAN_BUILD_VERSION if you've added a clean step!
# ***********************************************************************

# If you don't need to do a full clean build but would like to touch
# a file or delete some intermediate files, add a clean step to the end
# of the list.  These steps will only be run once, if they haven't been
# run before.
#
# E.g.:
#     $(call add-clean-step, touch -c external/sqlite/sqlite3.h)
#     $(call add-clean-step, rm -rf $(PRODUCT_OUT)/obj/external/zlib/)
#
# Always use "touch -c" and "rm -f" or "rm -rf" to gracefully deal with
# files that are missing or have been moved.
#
# Use $(PRODUCT_OUT) to get to the "out/target/product/blah/" directory.
# Use $(OUT_DIR) to refer to the "out" directory.
#
# If you need to re-do something that's already mentioned, just copy
# the command and add it to the bottom of the list.  E.g., if a change
# that you made last week required touching a file and a change you
# made today requires touching the same file, just copy the old
# touch step and add it to the end of the list.
#
# ************************************************
# NEWER CLEAN STEPS MUST BE AT THE END OF THE LIST
# ************************************************

# For example:
#$(call add-clean-step, rm -rf $(OUT_DIR)/target/common/obj/APPS/AndroidTests_intermediates)
#$(call add-clean-step, rm -rf $(OUT_DIR)/target/common/obj/JAVA_LIBRARIES/core_intermediates)
#$(call add-clean-step, find $(OUT_DIR) -type f -name "IGTalkSession*" -print0 | xargs -0 rm -f)
#$(call add-clean-step, rm -rf $(PRODUCT_OUT)/data/*)

$(call add-clean-step, rm -f $(PRODUCT_OUT)/system/etc/NOTICE.html)
# Remove generated java files after CL 126153
$(call add-clean-step, find $(OUT_DIR) -type f -name "*.java" -print0 | xargs -0 rm -f)
$(call add-clean-step, rm -rf $(OUT_DIR)/target/common/obj/JAVA_LIBRARIES/framework_intermediates)
$(call add-clean-step, rm -rf $(OUT_DIR)/target/product/sapphire/obj/SHARED_LIBRARIES/libhardware_legacy_intermediates/led)
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/system/bin/mountd)
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/system/etc/mountd.conf)
$(call add-clean-step, rm -rf $(OUT_DIR)/target/common/obj/APPS/Browser_intermediates)
$(call add-clean-step, rm -f vendor/google/apps/Talk/res/drawable/%*)
$(call add-clean-step, rm -rf $(OUT_DIR)/product/*/obj/SHARED_LIBRARIES/libandroid_runtime_intermediates/android_os_NetStat.o)
$(call add-clean-step, rm -rf $(OUT_DIR)/target/common/obj/JAVA_LIBRARIES/framework_intermediates)
$(call add-clean-step, rm -rf $(OUT_DIR)/target/product/*/obj/SHARED_LIBRARIES/libjni_andpyime_intermediates)
$(call add-clean-step, rm -rf $(OUT_DIR)/target/product/*/obj/SHARED_LIBRARIES/share)
$(call add-clean-step, rm -rf $(OUT_DIR)/target/product/*/obj/SHARED_LIBRARIES/libwebcore_intermediates)
$(call add-clean-step, rm -rf $(OUT_DIR)/target/common/obj/JAVA_LIBRARIES/framework_intermediates)
$(call add-clean-step, rm -rf $(OUT_DIR)/target/common/obj/APPS/PinyinIME_intermediates)
$(call add-clean-step, rm -rf $(OUT_DIR)/target/common/obj/JAVA_LIBRARIES/com.android.inputmethod.pinyin.lib_intermediates)
$(call add-clean-step, rm -rf $(OUT_DIR)/target/common/obj/APPS/PinyinIMEGoogleService_intermediates)
$(call add-clean-step, rm -rf $(OUT_DIR)/target/common/obj/JAVA_LIBRARIES/com.android.inputmethod.pinyin.lib_intermediates)
$(call add-clean-step, rm -rf $(OUT_DIR)/target/common/obj/APPS/PinyinIMEGoogleService_intermediates)

# ************************************************
# NEWER CLEAN STEPS MUST BE AT THE END OF THE LIST
# ************************************************
