#! /bin/sh
P=/home/rpi/time_sharing/limitations

if [ $# = 6 ]
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


ssh root@$1 $P/pwm_var $6

a=0
while [ "$a" -lt $5 ]
do
ssh root@$1 taskset $mycpu $P/disturber20&
sleep 0.2
a=`expr $a + 1`;
done





rm -f ./delta.txt
rm -f ./histo.txt
ssh root@$1 rm /tmp/delta.txt
ssh root@$1 rm /tmp/data.txt
ssh root@$1 rm /tmp/raw_data.txt

ssh root@$1 chrt $mysched $myprio taskset $mycpu $P/int_gpio4 $6


scp root@$1:/tmp/delta.txt delta.txt
scp root@$1:/tmp/data.txt data.txt

./compute-statistics < delta.txt


max=`expr 15000000 / $6`


./compute-histogram 200 0 $max < delta.txt > histo.txt

rm -f ./himage.png
rm -f ./dimage.png

#./dhisto.sh
#./ddata.sh

./display-histogram.sh histo.txt "pwm:$6Hz int_gpio4 running on CPU #$4 with scheduler: $2 , priority $myprio,  $5 disturber(s)" himage.png &

./display_data.sh data.txt "pwm:$6Hz int_gpio4 running on CPU #$4 with scheduler: $2 , priority $myprio,  $5 disturber(s)" dimage.png &
else
echo "usage: $0 <target IP add> <scheduler= other,fifo,rr> <priority= 1..99> <CPU= 0,1,2,3> <disturbers= 0..> <PWM frequency in Hz>"
fi
