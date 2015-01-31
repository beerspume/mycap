#!/bin/bash
set -e
device=$1
if [ "$device"x = ""x ]
then
  device="wlan0"
fi
#echo $device
ifconfig $device down
iwconfig $device mode Monitor
ifconfig $device up
./mycap -i $device $2