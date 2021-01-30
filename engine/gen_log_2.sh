#!/bin/sh

count=0
for ((;;))
do
	file_name=$(date "+%Y-%m-%d_%H%M")
	now=$(date)
	now+=" >> $count"
	
	#echo "$now" >> "$(pwd)\log.txt"
	echo "$now" >> log_2_${file_name}.log

	count=$(expr $count + 1)

	sleep 1
done
