#!/bin/bash
DIRNAME="`dirname "$0"`"

# Sometimes this bit gets stuck?
echo 7 > /sys/class/gpio/unexport
echo 9 > /sys/class/gpio/unexport
echo 10 > /sys/class/gpio/unexport
echo 11 > /sys/class/gpio/unexport

python ./avrdude.py
