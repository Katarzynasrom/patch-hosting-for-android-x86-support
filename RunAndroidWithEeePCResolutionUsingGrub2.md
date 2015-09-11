# Run Android with Eee PC resolution using grub2+915resolution
# Introduction #
The Eee PC models have unusual resolutions like 800x480 or 1024x600, which are not directly supported by VESA BIOS extensions(VBE) standard. However, it is possible to modify the VESA BIOS setting at boot time, using grub2 with [915resolution patch](http://www.nathancoulson.com/proj/eee/grub-1.96-915resolution-0.5.2-3.patch). The article describes how to use it to run Android on Eee PC with the native resolution.

# Steps #

  * Use make-live script to create a live android system in a usb disk
  * Install grub2 with 915resolution patched to the usb disk. Get prebuilt grub2 deb in EeeSoft repository:
> > http://218.211.38.204/eeesoft/pool/main/g/grub2/
  * Compile a vesa driver enabled kernel (see [here](BuildAndroidForX86Platforms#How_to_rebuild_kernel.md) for details), copy the image into usb disk
```
# mount /dev/sdb2 /media/root
# cp arch/x86/boot/bzImage /media/root/boot/kernel-vesa
```
  * Modify grub.cfg, like


> For Eee PC 9" or 10" (resolution 1024x600)
```
menuentry "Android Vesa 9xx" {
	insmod 915resolution
	915resolution 45 1024 600
	linux /boot/kernel-vesa root=/dev/sdc2 rootdelay=6 rw init=/init androidboot.hardware=eee_701 vga=791
}
```

> For Eee PC 7" (resolution 800x480)
```
menuentry "Android Vesa 7xx" {
	insmod 915resolution
	915resolution 43 800 480
	linux /boot/kernel-vesa root=/dev/sdb2 rootdelay=6 rw init=/init androidboot.hardware=eee_701 vga=788
}
```
> Note the root depends how many harddisks your Eee PC has. For 701 which has only one SSD, it is sdb2. For 900/901/1000 which has two SSDs, it is sdc2.

  * Boot Eee PC from the usb disk (press ESC when power on), and choose the item corresponding to your machine.

On booting, you should see the grub2 messages like
```
Chipset: 915GM
BIOS: TYPE 1
Mode Table Offset: $C000 + $269
Mode Table Entries: 36

Patch mode 43 to resolution 800x480 complete
```
Then the 915resolution patch works.

# Tips #
The patched mode must be 16-bit, or Android will not display correctly. See the vesa mode table
[here](http://en.wikipedia.org/wiki/VESA_BIOS_Extensions).

# Links #
  * [EeePC Tweaks](http://www.nathancoulson.com/proj_eee.shtml)
  * [Jeff's Place : Projects : Bios Hack](http://www.jeffsplace.net/projects/bios_hack)