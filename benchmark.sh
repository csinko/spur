#!/bin/bash
set -e

blocksize=512

if [ "$EUID" -ne 0 ]
  then echo "This command must be run as root."
  exit 1
fi

if [ "${1: -1}" == "/" ]; then
	mntpoint=$1;
else
	mntpoint=$1/;
fi

if [ -z $1 ] || [ ! -d $mntpoint ]; then
	echo "Usage: benchmark.sh MOUNT_POINT"
	echo "Benchmarks read speed for a drive mounted at MOUNT_POINT."
	exit 1
fi

echo "Read tests:"
sh -c "sync && echo 3 > /proc/sys/vm/drop_caches"
dd if=${mntpoint}EFI/BOOT/BOOTX64.EFI of=/dev/null bs=${blocksize} 2>&1 | awk '/copied/ {print $10 " " $11}'
sh -c "sync && echo 3 > /proc/sys/vm/drop_caches"
dd if=${mntpoint}EFI/grub/fonts/unicode.pf2 of=/dev/null bs=${blocksize} 2>&1 | awk '/copied/ {print $10 " " $11}'
sh -c "sync && echo 3 > /proc/sys/vm/drop_caches"
dd if=${mntpoint}EFI/grub/locale/de@hebrew.mo of=/dev/null bs=${blocksize} 2>&1 | awk '/copied/ {print $10 " " $11}'
sh -c "sync && echo 3 > /proc/sys/vm/drop_caches"
dd if=${mntpoint}EFI/grub/x86_64-efi/normal.mod of=/dev/null bs=${blocksize} 2>&1 | awk '/copied/ {print $10 " " $11}'
