#The instructions to build Android for X86.

# Introduction #

This page has (hopefully) the latest information about how to build Android for x86 platforms like EeePC. I try to provide instructions to build images to run on the real hardware as well as virtual machines. Due to the resource limitation (I'm doing this in my private time), currently I'm going to only use EeePC 1000HE and VirtualBox as reference running environments. I only have Ubuntu 8.10 for now, I will use it as my building platform.

_As you can see that I'm not a English speaker, so please bare with my broken English. For questions, please send e-mail to android porting group for quick help._

# Getting Android source code for x86 #
> Android currently supports x86 platforms though EeePC 701 porting done by Dima Zavin <dima@android.com>. In order to build Android image for EeePC 701, you need to :
  1. follow the page http://source.android.com/download to configure your build environment. But do not check out the source code. (Do not run repo init)
  1. $ mkdir <sandbox or whatever name you like for your sandbox>
  1. $ cd sandbox
  1. $ Run repo init -u git://android.git.kernel.org/platform/manifest.git -b cupcake and answer the questions
  1. If the last step runs successfully, you should see something like repo initialized in sandbox"
  1. $ repo sync
It depends on your network, this step could take very long time (on my ADSL connection, it can take up to 3 hours).
  1. $ cd .repo
  1. create  local\_manifest.xml file with following contents
> > <?xml version="1.0" encoding="UTF-8"?>
> > 

&lt;manifest&gt;


> > > 

&lt;project name="platform/vendor/asus/eee\_701" path="vendor/asus/eee\_701"/&gt;



> > 

&lt;/manifest&gt;


  1. $cd ..
  1. $repo sync
  1. patch all the patches in the downloads section of this site.
Here is a smart tip for apply patches from kewarken@gmail.com

```
   On Wed, 2009-04-22 at 07:04 -0700, kewarken wrote:
> If you want to apply them all at once, since the patches follow a
> pattern, you can do something like this from your cupcake source dir
> with the patches in the directory above it:
> 
> for patch in `pwd`/../*patch ; do
>     project=`awk '/^project /{print $2}' $patch`
>     (cd $project && patch -p1 < $patch)
> done
> 
> cheers,
> 
> Kris
> 

```

# How to build an image for VirtualBox210 #
  1. You need to build the kernel image before you do following steps. Please see the "How to rebuild kernel" section for detail
  1. $cd sandbox
  1. $TARGET\_ARCH=x86 TARGET\_PRODUCT=eee\_701 DISABLE\_DEXPREOPT=true make -j4 droid
  1. $vendor/asus/eee\_701/make\_boot\_img.sh [configuration file](disk.md)

> by default vendor/asus/eee\_701/disk\_img.conf will be used as default disk configuration file. You can create your own.
  1. cd out/target/product/eee\_701
  1. $VBoxManage convertfromraw -static -format vdi installed.img installed.vdi
> now you can boot installed.vdi from VirtualBox

# Install and config VirtualBox210 #
  1. download VirtualBox package from http://download.virtualbox.org/virtualbox/2.1.0/
  1. install the package on your machine
  1. run the VirtualBox after you finished install
  1. Click on New -> Next
> > - Name = it as "AsusAndrioid"
> > - OStype=Linux
> > - Version=2.6
> > - RAM=256MB
> > - Hard Disks=  Slot Checked Boot Harddisk with IDE Primary Master

# Booting Android kernel in VirtualBox #
  1. Start VirtualBox and push F12 key fast as soon as you see "Press F12 to select boot device" message  in virtualbox window.
  1. Select correct boot device (should be IDE Primary Master)
  1. Boot to "std\_boot"

# How to rebuild kernel #
  1. download kernel.config to vendor/asus/eee\_701/
  1. $cd sandbox
  1. $cp vender/asus/eee\_701/kernel.config kernel/.config
  1. $cd kernel
  1. $make menuconfig  select the Virtualization option and de-select it then exit, choose "Yes" when you are asked whether to save a new configuration. (Select/de-select the Virtualization option is just used to trigger some menuconfig operations.)
  1. $make bzImage
  1. you can find your kernel image from arch/x86/boot directory
  1. $cp bzImage vendor/asus/eee\_701/ and rename it to kernel
  1. rebuild your Android image.

# How to build and install image for EeePC701 #
Building an installer image for a real EeePC is not more complicate than building an image for VB. The biggest issue is the framebuffer. The original porting done by Dima uses a modesetting inteldrmfb driver. Dima gave a link to download the source code for the driver. But the link does not work anymore. So people can not build their own kernel anymore.
The modesetting inteldrmfb is released with 2.6.29 kernel, but at the time this note is written, the Android does not support 2.6.29 yet. Before Android moves to 2.6.29, we  have two choices (Both tested by Luca and me):
  1. use VESA with very low resolution (640x480)
  1. use intelfb with little bit higher resolution (800x600)

I choose to use intelfb. In order to use either VESA or intelfb, we need to put vga=788 (800x600) or vga=785 (640x480) in the kernel command line.

_(Luca)_ I am running on EeePc 701 with following cmdline parameters appended to default ones:
```
video=intelfb:640x480-16,accel vga=785
```

_(Yi)_ I'am running on EeePC 1000HD with following cmdline parameters appended to default ones:
```
video=intelfb:800x600-32,vga=788
```

Until now, _Intel 915_ need userspace stuff to improve performances and resolution.

To do it, you need to do following changes:
```
project bootable/diskinstaller/
diff --git a/config.mk b/config.mk
index 2cb034d..3ed1759 100644
--- a/config.mk
+++ b/config.mk
@@ -119,6 +119,7 @@ internal_installerimage_args := \
 internal_installerimage_files := \
        $(filter-out --%,$(internal_installerimage_args))
 
+BOARD_KERNEL_CMDLINE := $(patsubst vga=788, ,$(BOARD_KERNEL_CMDLINE))
 BOARD_KERNEL_CMDLINE := $(strip $(BOARD_KERNEL_CMDLINE))
 ifdef BOARD_KERNEL_CMDLINE
   internal_installerimage_args += --cmdline "$(BOARD_KERNEL_CMDLINE)"

In vendor/asus/eee_701 directory do following changes

diff --git a/BoardConfig.mk b/BoardConfig.mk
index 54b0d4f..598a528 100644
--- a/BoardConfig.mk
+++ b/BoardConfig.mk
@@ -25,7 +25,7 @@ TARGET_USE_DISKINSTALLER := true
 
 TARGET_DISK_LAYOUT_CONFIG := vendor/asus/eee_701/disk_layout.conf
 
-BOARD_KERNEL_CMDLINE := console=tty0 console=ttyS1,115200n8 console=tty0 androidboot.hardware=eee_701
+BOARD_KERNEL_CMDLINE := console=tty0 console=ttyS1,115200n8 console=tty0 androidboot.hardware=eee_701 vga=788
 
diff --git a/init.rc b/init.rc
index 00aa93c..3eeb758 100644
--- a/init.rc
+++ b/init.rc
@@ -91,8 +91,10 @@ loglevel 3
     insmod /lib/modules/cfbfillrect.ko
     insmod /lib/modules/cfbcopyarea.ko
     insmod /lib/modules/drm.ko
-    insmod /lib/modules/i915.ko modeset=1
+    insmod /lib/modules/i915.ko 
     insmod /lib/modules/fbcon.ko
+    insmod /lib/modules/i2c-algo-bit.ko
+    insmod /lib/modules/intelfb.ko
```
in AndroidBoard.mk find kernel\_modules := \ and add i2c-algo-bit.ko and intelfb.ko


Now you can start to build installer.img:

  1. get the source code as mentioned in the "Getting Android source code for x86" section
  1. compile kernel by disable VESA framebuffer and enable follow features as module
> > atl2.ko     cfbcopyarea.ko  cfbimgblt.ko  font.ko
> > i915.ko     bitblit.ko  cfbfillrect.ko  drm.ko	  fbcon.ko     i2c-algo-bit.ko
> > intelfb.ko  softcursor.ko
  1. copy these modules to vendor/asus/eee\_701/modules directory
  1. build installer\_img with following command
> > TARGET\_ARCH=x86 TARGET\_PRODUCT=eee\_701 DISABLE\_DEXPREOPT=true make -j4 installer\_img
  1. Plug usb driver to your pc
  1. cd out/target/product/eee\_701
  1. dd if=./installer.img of=<your usb device node>
  1. use the usb driver to boot your eeePC and install Android on the target