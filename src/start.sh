#!/bin/bash
set -e
device=$1
switch=$2
if [ "$device"x = ""x ]
then
  device="wlan0"
fi
#echo $device
ifconfig $device down
if [ "$switch"x = "up"x -o "$switch"x = "run"x ]
then
  iw dev $device set type monitor
fi
if [ "$switch"x = "down"x ]
then
  iw dev $device set type managed
fi
ifconfig $device up
if [ "$switch"x = "run"x ]
then
  ./mycap -i $device $3
fi
