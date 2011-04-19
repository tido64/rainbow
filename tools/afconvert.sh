#!/bin/sh
if [ -z "$1" ]; then
	echo "Syntax: $0 [-c] input_file"
	exit
fi
if [ "$1" == "-c" ]; then
	if [ -z "$2" ]; then
		echo "Syntax: $0 [-c] input_file"
		exit
	fi
	/usr/bin/afconvert -f caff -d ima4 -c 1 $2
else
	/usr/bin/afconvert -f caff -d LEI16@44100 -c 1 $1
fi
