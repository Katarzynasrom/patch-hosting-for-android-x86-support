#How to make S3 suspend and resume to work on X86
# Introduction #
Android added so called wakelock and earlysleep on top of normal Linux power management. You could get some idea about wakelock and earlysleep from https://lists.linux-foundation.org/pipermail/linux-pm/2009-January/019444.html

# How to make suspend and resume to work on x86 #
  * on the 2.6.29 android kernel apply the sleep.patch. If you are using 2.6.27 kernel or earlier, then you need to manually change the kernel/drivers/acpi/sleep/main.c to apply the same change.
  * rebuild kernel
  * add acpi\_sleep=s3\_bios,s3\_mode to your kernel command line
  * boot your kernel

# NOTE #
Android allows user program to decide which key can be used to wakeup the system. When user push a key that is configured to wake up the system, Android WM will push "on" to /sys/power/state to keep the system to stay awake. Otherwise, the system will get back to sleep right away. And I think the key is configured in the key layout file (the **.kl).**

This patch will break this feature. But I think it is ok. With a normal netbook, press any key can wake up the machine from S3 mode. And the current patch also makes the life easier when we debug the root cause of why Android application can not get the key event and push done "ON" to earlysuspend code.