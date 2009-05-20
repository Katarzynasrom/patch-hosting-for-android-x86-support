#!/bin/sh
#
# This script is based on ChenYang (sunsetyang@gmail.com)'s instructions
# posted in Android porting group.
# This script is mainly used to generate a boot-able image for Android on
# x86 platforms. The image can be easily converted into a image for wmware
# or virtualbox
#

EXECPATH="out/target/product/eee_701"
BINPATH="../../../../out/host/linux-x86/bin/"
CP=cp
ECHO=echo
GZIP=gzip
RM=rm
MKDIR=mkdir
SED=sed
MKBOOTFS=${BINPATH}/mkbootfs
GENEXT2FS=${BINPATH}/genext2fs
EDITDISKLBL=${BINPATH}/editdisklbl

cd $(dirname $0)
DEFDISKCONF=$PWD/disk_img.conf

DISKCONF=${1:-$DEFDISKCONF}

cd ../../../$EXECPATH
if [ $? -ne 0 -o ! -e grub -o ! -e root -o ! -e data -o ! -e system -o ! -e ${BINPATH} ]; then
    ${ECHO} "Can not find prebuilt directories"
    ${ECHO} "Please run TARGET_ARCH=x86 TARGET_PRODUCT=eee_701 DISABLE_DEXPREOPT=true make -j4 installer_img first"
    exit
fi

if [ ! -e kernel ];
then
    ${ECHO} "Can not find kernel image, please build kernel image first"
    exit
fi
if [ -e bootimg ];
then
    ${RM} -rf bootimg
fi

if [ -e rootimg ];
then
    ${RM} -rf rootimg
fi

if [ ! -e dummy ];
then
    ${MKDIR} dummy
fi

${MKDIR} bootimg
${CP} grub/grub.bin installed.img
${CP} -raf root rootimg
${SED} -e 's/ext3/ext2/'<root/init.rc>rootimg/init.rc
${MKBOOTFS} rootimg | ${GZIP} -9 > bootimg/ramdisk
${ECHO} "console=tty0 console=ttyS1,115200n8 console=tty0 androidboot.hardware=eee_701 vga=788" > bootimg/cmdline
${CP} kernel bootimg/kernel
${GENEXT2FS} -b 8192 -m 0 -d bootimg boot.img
${GENEXT2FS} -b 8192 -m 0 -N 8 -d dummy dummy.img
${GENEXT2FS} -b 81920 -d data -N 512 -m 0 data.img
${EDITDISKLBL} -l ${DISKCONF} -i installed.img  boot=boot.img cache=dummy.img system=system.img third_party=dummy.img data=data.img

${ECHO} "installed.img is ready if you don't see any error messages"
