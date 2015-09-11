# Introduction #

This page will explain how to make ALSA work on EeePC 701 (and I think also other similar products).

## Downloading ALSA projects ##

Create the `local_manifest.xml` file which tells repo about the eee\_701 branch:

```
$ cd .repo
$ touch local_manifest.xml
```

Open `local_manifest.xml` in your favorite text editor. Paste the following text into `local_manifest.xml`.

```
<?xml version="1.0" encoding="UTF-8"?>
<manifest>
 <project path="external/alsa-lib" name="platform/external/alsa-lib"/>
 <project path="external/alsa-utils" name="platform/external/alsa-utils"/>
 <project path="hardware/alsa_sound" name="platform/hardware/alsa_sound"/>
</manifest>
```

## Notice ##

Both `alsa_amixer` and `alsa_aplay` will not work. Do not rely on them.
On troubleshooting, check if the audio card is recognized by the ALSA Layer by using procfs or sysfs.
Also `alsa_aplay -l` should help.

## Kernel ##

ALSA (as built in) and Intel HD (as module).

## vendor/asus/eee\_701/BoardConfig.mk ##
```
    #...
    HAVE_HTC_AUDIO_DRIVER := false
    BOARD_USES_ALSA_AUDIO := true
    BUILD_WITH_ALSA_UTILS := true
    #...
```

## vendor/asus/eee\_701/AndroidBoard.mk ##

Add the module to the kernel modules section.

```
kernel_modules := \
    #...
    snd-hda-intel.ko
```

Copy alsa config into `system/etc/`.

PRODUCT\_COPY\_FILES += \
> $(LOCAL\_PATH)/asound.conf:system/etc/asound.conf

## Asound.conf ##

Place asound.conf file into vendor directory (you'll find it in Downloads section).

## vendor/asus/eee\_701/init.rc ##

Under `kernel/Documentation/sound/alsa/ALSA-Configuration.txt` there are other sound card models can be specified as module arg.

```
on init
    #...
    insmod /lib/modules/snd-hda-intel.ko model=eeepc-p701
    #...
```

## vendor/asus/eee\_701/init.eee\_701.rc ##

```
    setprop alsa.mixer.playback.master Master
    setprop alsa.mixer.capture.master Capture
    setprop alsa.mixer.playback.earpiece Master
    setprop alsa.mixer.capture.earpiece Capture
    setprop alsa.mixer.playback.headset Master
    setprop alsa.mixer.playback.speaker Master
```

## system/core/init/devices.c ##

Add the following line to `static struct perms_ devperms[]`.

```
{ "/dev/snd/",          0664,   AID_SYSTEM,     AID_AUDIO,      1 }
```

# Google Groups Discussion #

You can notify missing information or ask for help at [GoogleDiscussion](http://groups.google.it/group/android-porting/browse_thread/thread/5ac2606399e134d6?hl=it&pli=1)