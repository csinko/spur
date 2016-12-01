# spur
Choose which operating system to boot into using a hardware switch.

## AVR code

AVR code is in the lufa-avr/Demos/Device/LowLevel/MassStorage directory. To compile:


```
make clean
make
```

## Preparing the SD Card

The SD card must be formatted with a hybrid MBR/GPT and GRUB needs to be installed for both BIOS and UEFI.

Assuming _/dev/mmc_ is the SD card:
```
# Wipe the card
$ shred -n 0 -z -v /dev/mmc

# Partition the card
$ gdisk /dev/mmc
GPT fdisk (gdisk) version 1.0.1

Caution: invalid main GPT header, but valid backup; regenerating main header
from backup!

Caution! After loading partitions, the CRC doesn't check out!
Warning! Main partition table CRC mismatch! Loaded backup partition table
instead of main partition table!

Warning! One or more CRCs don't match. You should repair the disk!

Partition table scan:
  MBR: not present
  BSD: not present
  APM: not present
  GPT: damaged

Found invalid MBR and corrupt GPT. What do you want to do? (Using the
GPT MAY permit recovery of GPT data.)
 1 - Use current GPT
 2 - Create blank GPT

Your answer: 2

Command (? for help): n
Partition number (1-128, default 1): 
First sector (34-15523806, default = 2048) or {+-}size{KMGTP}: 
Last sector (2048-15523806, default = 15523806) or {+-}size{KMGTP}: +1G
Current type is 'Linux filesystem'
Hex code or GUID (L to show codes, Enter = 8300): ef00
Changed type of partition to 'EFI System'

Command (? for help): r

Recovery/transformation command (? for help): h

WARNING! Hybrid MBRs are flaky and dangerous! If you decide not to use one,
just hit the Enter key at the below prompt and your MBR partition table will
be untouched.

Type from one to three GPT partition numbers, separated by spaces, to be
added to the hybrid MBR, in sequence: 1
Place EFI GPT (0xEE) partition first in MBR (good for GRUB)? (Y/N): y

Creating entry for GPT partition #1 (MBR partition #2)
Enter an MBR hex code (default EF): 
Set the bootable flag? (Y/N): y

Unused partition space(s) found. Use one to protect more partitions? (Y/N): n

Recovery/transformation command (? for help): w

Final checks complete. About to write GPT data. THIS WILL OVERWRITE EXISTING
PARTITIONS!!

Do you want to proceed? (Y/N): y
OK; writing new GUID partition table (GPT) to /dev/mmc.
The operation has completed successfully.

# Format the EFI system partition to FAT32
mkfs.msdos -F 32 -n SPUR /dev/mmcp1

# Mount EFI system partition
mount /dev/mmcp1 /mnt
cd /mnt

# Create directories
mkdir boot EFI

# Install GRUB for BIOS
grub-install --force --target=i386-pc --boot-directory=./boot /dev/mmc

# Install GRUB for UEFI
grub-install --target=x86_64-efi --efi-directory=./ --boot-directory=./EFI --bootloader-id=boot --removable /dev/mmc

# Copy over config file
cp ~/spur/grub/grub.cfg /mnt/boot/grub/
cp ~/spur/grub/grub.cfg /mnt/EFI/grub/
```
