#! /bin/sh
P=/home/rpi/real_time/sched_and_prio

if [ $# = 5 ]
then

prio1=$4
prio2=$5

if [ $2 = other ]
then
mysched=-o
prio1=0
prio2=0
elif [ $2 = fifo ]
then
mysched=-f
elif [ $2 = rr ]
then
mysched=-r
else
mysched=-o
prio1=0
prio2=0
fi



if [ $3 = 0 ]
then
mycpu=1
elif [ $3 = 1 ]
then
mycpu=2
elif [ $3 = 2 ]
then
mycpu=4
elif [ $3 = 3 ]
then
mycpu=8
else
mycpu=1
fi



ssh root@$1 chrt $mysched $prio1 taskset $mycpu $P/fastblink_nbsec 24 10&
sleep 2
ssh root@$1 chrt $mysched $prio2 taskset $mycpu $P/fastblink_nbsec 25 5&




else
echo "usage: $0 <target IP add> <scheduler= other,fifo,rr> <CPU= 0,1,2,3> <priority GPIO24= 1..99> <priority GPIO25= 1..99>"
fi
