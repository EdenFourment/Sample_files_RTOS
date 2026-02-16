#! /bin/sh

# $1 is the first argument passed to the program, here the port number

# test if directory gpio$1 exists, if not create it
# if yes: skip to the next command
# if not: execute command after ||
[ -d /sys/class/gpio/gpio$1 ] || echo $1 > /sys/class/gpio/export

# test if file gpio$1/direction exists and is writable,
# if yes: execute commands after && within { }
# if not: skip to the next command
[ -w /sys/class/gpio/gpio$1/direction ] && {
echo out > /sys/class/gpio/gpio$1/direction
echo 1 > /sys/class/gpio/gpio$1/value
echo "GPIO$1 = 1"
}
