#!/bin/sh
# AVailable event variables:
#
# ACTION
# SEQNUM
# MAJOR
# MDEV
# DEVPATH
# SUBSYSTEM
# MINOR
# PHYSDEVPATH
# PHYSDEVDRIVER
# PHYSDEVBUS
# PWD
#-----------------------------------------------
CNF=/etc/mdev/module.map

# Is this a USB device?
USBD=`echo $MDEV|cut -c1-6`
if [ "$USBD" != "usbdev" ]
then
    exit 0
fi

# Grab the bus and device numbers
BD=`echo $MDEV | cut -c7-`
BUS=`echo $BD | cut -f1 -d"."`
DEVICE=`echo $BD | cut -f2 -d"."`

# Find the Product and Vendor IDs
IDS=`lsusb -s $BUS:$DEVICE | cut -f6 -d" "`
MODULE=`grep $IDS $CNF|cut -f2- -d" "`
if [ "$MODULE" != "" ]
then
[ $MODULE == "rt73" ] && insmod /lib/modules/rt2x00lib.ko && insmod /lib/modules/rt2x00usb.ko
    echo "Loading USB module: $MODULE"
    insmod /lib/modules/$MODULE.ko
fi
