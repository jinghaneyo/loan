#!/bin/sh

if [ -f log.txt ];then
	#rm -f $(pwd)/log.txt
	rm -f log.txt
fi

count=0
for ((;;))
do
	now=$(date)
	now+=" >>  $count"
	
	#echo "$now" >> "$(pwd)\log.txt"
	echo "$now" >> "log.txt"

	count=$(expr $count + 1)

	sleep 1
done
