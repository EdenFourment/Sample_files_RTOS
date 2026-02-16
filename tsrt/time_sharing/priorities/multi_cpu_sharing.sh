#! /bin/sh

if [ $# -lt 2 ]
then
	echo "usage: $0 <n> <maxloop>"
	exit 1
fi

for i in `seq 1 $1`
do
taskset 4 ./cpu_sharing $2 &
echo process $i started...
done
