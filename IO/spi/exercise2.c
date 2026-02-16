/* 
Code adapted from Gustavo zamboni (linux-sunxi.org/SPIdev)
B.Stefanelli - nov 2016
 */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <linux/types.h>
#include <linux/spi/spidev.h>

#include <sys/ioctl.h>

#include <string.h>
#include <signal.h>
#include <stdint.h>


	char inbuf[10];
	char outbuf[10];
 
	struct spi_ioc_transfer xfer[1]; // defines a one-dimension transfer structure

	char filename[40]="/dev/spidev0.0"; //therefore,  _CS0 pin will be used as chip select

        int fd_spi;

	__u8    mode, lsb, bits;
    	__u32 speed;

 	int status;
 	
 	float in_value;
 	uint16_t value;


int main(int argc, char *argv[]){
 
 	// command string test
	if (argc!=2){
	fprintf(stdout,"Usage: argv[0] <output value in volt>\n");
	return EXIT_SUCCESS;
	}
 	
 	fprintf(stdout,"Connect _LDAC to GND and _CSDAC to _CS0\n");
 	
 	//input value limitation
	sscanf(argv[1],"%e", & in_value);
	if(in_value <0) in_value=0;
 	if(in_value > 3.3) in_value=3.3; 
 	
 	
  
        fd_spi = open(filename, O_RDWR);
        if (fd_spi < 0) {
                perror(filename);
                exit(EXIT_FAILURE);
        }

/*
commands are sent to ioctl() through integer values. for readibility, SPI_IOC_WR_MODE, SPI_IOC_RD_MODE, SPI_IOC_RD_LSB_FIRST and so on are aliases to the corresponding number
*/

// setting mode : 0, 1, 2 or 3 (depends on the desired phase and polarity of the clock)
	mode = 0;
	ioctl(fd_spi, SPI_IOC_WR_MODE, &mode);

// reading some parameters
	if (ioctl(fd_spi, SPI_IOC_RD_MODE, &mode) < 0)
	{
         perror("SPI rd_mode");
         return -1;
         }


         if (ioctl(fd_spi, SPI_IOC_RD_LSB_FIRST, &lsb) < 0) 
         {
         perror("SPI rd_lsb_fist");
         return -1;
         }
 

// setting maximum transfer speed, the effective speed will probably be different
	speed = 15000000;	

// setting bits per word
	bits = 8;
 
 	
 
	xfer[0].cs_change = 0; /* Keep CS activated if = 1 */
	xfer[0].delay_usecs = 0, //delay in us
	xfer[0].speed_hz = speed, //speed
	xfer[0].bits_per_word = bits, // bits per word 8
 
// clearing buffers
	memset(inbuf, 0, sizeof inbuf);
	memset(outbuf, 0, sizeof outbuf);

// filling buffers
// these values are specific to the MCP4912: 
	outbuf[0] = 0x70; // chA, buffered, gain=1, active mode, data=0
	
	
	value = (uint16_t)(in_value*1023/3.3) ;
	outbuf[0] |= (char) (value >>6);
	
	outbuf[1] |= (char) (value <<2); 
	

	xfer[0].rx_buf = (unsigned long) inbuf;
	xfer[0].tx_buf = (unsigned long)outbuf;
	xfer[0].len = 3; /* Length of  command to write = 3 bytes*/

// sending message
 
	status = ioctl(fd_spi, SPI_IOC_MESSAGE(1), xfer);
    	if (status < 0)
        {
        perror("SPI_IOC_MESSAGE");
        exit(EXIT_FAILURE);
        }

// printing results
	
fprintf(stdout, "value= %eV -> %d / 0X%X, outbuf[0]= 0X%X, outbuf[1]= 0X%X\n", in_value, value, value, outbuf[0], outbuf[1]);
 

// terminating the job
        close(fd_spi);
        return EXIT_SUCCESS;
}// end main


