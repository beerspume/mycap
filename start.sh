#!/bin/sh

deivce=$1
if [device==""] 
then
    device="wlan0"
fi
airmon-ng start $deivce
./mycap -i mon0
airmon-ng stop mon0
