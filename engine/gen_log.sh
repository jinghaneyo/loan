#!/bin/sh

count=0
for ((;;))
do
	now=$(date)
	now+=" >>  $count"
	
	echo "$now" >> "$(pwd)\log.txt"

	count=$(expr $count + 1)

	sleep 1
done
