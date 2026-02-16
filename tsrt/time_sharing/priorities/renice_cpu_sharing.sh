#! /bin/sh

if [ $# -lt 2 ]
then
	echo "usage: $0 <priority> <maxloop>"
	exit 1
fi


#taskset 4 nice -n $1 ./cpu_sharing 37000000 &
taskset 4 nice -n $1 ./cpu_sharing $2 &
echo process 1 started with priority $1...
#taskset 4 nice -n -$1 ./cpu_sharing 37000000 &
taskset 4 nice -n -$1 ./cpu_sharing $2 &
echo process 2 started with priority -$1...

