#!/system/bin/sh

# no sleep!
echo EeeNoSleep > /sys/android_power/acquire_partial_wake_lock

netcfg eth0 dhcp
setprop net.dns1 4.2.2.2

## For wifi, we'll need this:

# insmod ath_hal.ko
# insmod wlan.ko
# insmod wlan_scan_sta.ko
# insmod ath_rate_sample.ko
# insmod ath_pci.ko
