#! /bin/sh
P=/home/rpi/real_time/limitations

if [ $# = 5 ]
then


myprio=$3

if [ $2 = other ]
then
mysched=-o
myprio=0
elif [ $2 = fifo ]
then
mysched=-f
elif [ $2 = rr ]
then
mysched=-r
else
mysched=-o
myprio=0
fi



if [ $4 = 0 ]
then
mycpu=1
elif [ $4 = 1 ]
then
mycpu=2
elif [ $4 = 2 ]
then
mycpu=4
elif [ $4 = 3 ]
then
mycpu=8
else
mycpu=1
fi




a=0
while [ "$a" -lt $5 ]
do
ssh root@$1 chrt $mysched $myprio taskset $mycpu $P/disturber50&
sleep 0.2
a=`expr $a + 1`;
done





rm -f ./delta.txt
rm -f ./histo.txt
ssh root@$1 rm /root/delta.txt

ssh root@$1 chrt $mysched $myprio taskset $mycpu $P/posix-timer-create-03 1000 


scp root@$1:/root/delta.txt delta.txt

./compute-statistics < delta.txt

./compute-histogram 100 0 6000 < delta.txt > histo.txt



./display-histogram.sh histo.txt "timer 1kHz running on CPU #$4 with scheduler: $2 , priority $myprio,  $5 disturber(s) with priority $myprio"


else
echo "usage: $0 <target IP add> <scheduler= other,fifo,rr> <priority= 1..99> <CPU= 0,1,2,3> <disturbers= 0..>"
fi
