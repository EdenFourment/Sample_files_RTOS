/*
testing IOCTL read and write on i2c
brs - 10/11/2016

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include <fcntl.h>
//#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>


#define I2C_BUS		"/dev/i2c-1"
#define CHIP_ADDRESS	0x48
#define TEMP_REG	0x00
#define CONF_REG	0x01
#define HYST_REG	0x02
#define SET_REG		0x03


int main(int argc, char * argv[])
{
int fd;

unsigned char outbyte[3];
unsigned char inbyte[3];

float degres;


// Accessing the bus
fd = open(I2C_BUS, O_RDWR);
if (fd < 0) {
	perror(I2C_BUS);
	exit(EXIT_FAILURE);
	}

// setting  slave address
if (ioctl(fd, I2C_SLAVE,CHIP_ADDRESS) < 0) {
	perror("slave unreachable");
	exit(EXIT_FAILURE);
	}


// writing CONFIG register
outbyte[0] = (unsigned char)(CONF_REG);
outbyte[1] = 0x20; // 0 01 00 0 0 0 : default values kept excepted for resolution (10 bits)
write(fd, & outbyte, 2);


// reading TEMP register
// 1/ writing the register address to the previously defined slave address
outbyte[0] = (unsigned char)(TEMP_REG);
write(fd, & outbyte, 1);


while(1){
// 2/ reading two bytes from the previously defined register
read(fd, & inbyte, 2);
fprintf(stdout, "inbyte[0]= %#04x  %d\n", inbyte[0], inbyte[0]);	//MSB 
fprintf(stdout, "inbyte[1]= %#04x  %d\n", inbyte[1], inbyte[1]);

degres = (float)inbyte[0];
if (degres >= 128) degres -= 256;


fprintf(stdout, "temp = %2.3f Celsius\n", degres+(inbyte[1]/16)*0.0625);
sleep(1);

} // end while(1)


	close(fd);
	return EXIT_SUCCESS;
}
