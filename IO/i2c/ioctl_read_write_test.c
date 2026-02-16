/*
testing IOCTL read and write on i2c
the target is the TCN75A temperature sensor
brs - 05/02/2020

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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




/*
 writing 3 bytes to the previously defined slave address
 the chip address is automatically sent prior outbyte[i]
 TCN75A: outbyte[0] defines which register is concerned
 TCN75A: the following outbyte[i] contain the data to write
*/
outbyte[0] = (unsigned char)(HYST_REG); // writes to HYSTERESIS register
outbyte[1] = 0x12;
outbyte[2] = 0x80;
write(fd, & outbyte, 3);


/*
 reading the two data bytes  previously written
 the chip address is automatically sent prior outbyte[i]
 TCN75A: there is no need to redefine the register address if you are working on the same register
*/
read(fd, & inbyte, 2);
fprintf(stdout, "inbyte[0]= %#04x\n", inbyte[0]);	//MSB 
fprintf(stdout, "inbyte[1]= %#04x\n", inbyte[1]);




	close(fd);
	return EXIT_SUCCESS;
}
