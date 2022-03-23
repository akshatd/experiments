#!/bin/bash

while true; do
	vcgencmd measure_clock arm
	vcgencmd measure_temp
	sleep 1
done&

stress -c 4 -t 600s

