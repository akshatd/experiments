#!/bin/bash

date
echo "Starting emergency shutdown!!"
gpio mode 7 out
gpio write 7 0
gpio write 7 1

sudo echo 1 > /proc/sys/kernel/sysrq 
#sudo echo i > /proc/sysrq-trigger
sudo echo s > /proc/sysrq-trigger
sudo echo u > /proc/sysrq-trigger
sudo echo b > /proc/sysrq-trigger

