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


	char inbuf[10];
	char outbuf[10];
 
	struct spi_ioc_transfer xfer[1]; // defines a one-dimension transfer structure

	char filename[40]="/dev/spidev0.1"; //therefore,  _CS1 pin will be used as chip select

        int fd_spi;

	__u8    mode, lsb, bits;
    	__u32 speed;

 	int status;


int main(int argc, char *argv[])
{
 

  
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
	speed = 1000000;	// SCLK frequency (Hz)

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
// these values are specific to the MCP3004: 
	outbuf[0] = ???
	outbuf[1] = ???
	outbuf[2] = ???	

	xfer[0].rx_buf = (unsigned long) inbuf;
	xfer[0].tx_buf = (unsigned long)outbuf;
	xfer[0].len = ??? /* Length of  command to write */

// sending message
 
	status = ioctl(fd_spi, SPI_IOC_MESSAGE(1), xfer);
    	if (status < 0)
        {
        perror("SPI_IOC_MESSAGE");
        exit(EXIT_FAILURE);
        }

// printing results
	fprintf(stdout, "%s: spi mode %d, %d bits %sper word, %d Hz max\n",filename, mode, bits, lsb ? "(lsb first) " : "(msb first)", speed);    
	fprintf(stdout, "env: %#04x %#04x %#04x\n", outbuf[0], outbuf[1], outbuf[2]);
    	fprintf(stdout, "ret: %#04x %#04x %#04x\n", inbuf[0], inbuf[1], inbuf[2]);

 

// terminating the job
        close(fd_spi);
        return EXIT_SUCCESS;
}// end main


