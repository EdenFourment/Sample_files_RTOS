//
//  How to use GPIO and PWM from C-code on the Raspberry-Pi
// compilation of many sources including:
// Gert van Loo & Dom
// Hussam al-Hertani (hertaville.com)
// Christophe Blaess (www.blaess.fr)
// Frank Buss, 2012
//
//  B. Stefanelli / Revised: 14 dec 2021
 
 
// Access from ARM Running Linux
 
//#define BCM2708_PERI_BASE        0x20000000	//RPi 1
#define BCM2708_PERI_BASE	 0x3F000000	//RPi 2 & 3
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */
#define PWM_BASE		(BCM2708_PERI_BASE + 0x20C000) /* PWM controller */
#define CLOCK_BASE		(BCM2708_PERI_BASE + 0x101000) /* clock controller */

 
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
 
#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)
 

void *gpio_map;
void *pwm_map;
void *clk_map;
 
// I/O access
volatile unsigned *gpio;
volatile unsigned *pwm;
volatile unsigned *clk;
 
 
// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x) or SET_GPIO_ALT(x,y)
#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))
 
#define GPIO_SET *(gpio+7)  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR *(gpio+10) // clears bits which are 1 ignores bits which are 0

#define GPIO_SET_EXT *(gpio+8)  // for GPIO# >= 32 RPi B+, A+, 2, 3
#define GPIO_CLR_EXT *(gpio+11) // for GPIO# >= 32 RPi B+, A+, 2, 3
 
#define GET_GPIO(g) (*(gpio+13)&(1<<g)) // 0 if LOW, (1<<g) if HIGH
#define GET_GPIO_EXT(g) (*(gpio+14)&(1<<g)) // 0 if LOW, (1<<g) if HIGH
 
#define GPIO_PULL *(gpio+37) // Pull up/pull down
#define GPIO_PULLCLK0 *(gpio+38) // Pull up/pull down clock
#define GPIO_PULLCLK1 *(gpio+39) // Pull up/pull down clock for GPIO# >= 32 RPi B+, A+, 2, 3

#define PWM_CTL *(pwm)  // PWM CTL register
#define PWM_RNG1 *(pwm+4)  // PWM RNG1 register
#define PWM_DAT1 *(pwm+5)  // PWM DAT1 register
#define PWM_RNG2 *(pwm+8)  // PWM RNG2 register
#define PWM_DAT2 *(pwm+9)  // PWM DAT2 register

#define CM_PWMCTL *(clk+40)	// Clock manager, PWM clock control
#define CM_PWMDIV *(clk+41)	// Clock manager, PWM clock divisors

#define PWM_ALGO 0
#define PWM_MS 1
#define PWM_SERIAL 2

//**************************************************************************************
//********************* function prototypes ********************************************
//**************************************************************************************
void map_peripherals();
float pwm_clock(float);
void pwm_create(int);
void pwm_range(int, unsigned int);
void pwm_data(int, unsigned int);
void pwm_enable(int);
void pwm_disable(int);
void pwm_mode(int, int);

//**************************************************************************************
//******************** main ************************************************************
//**************************************************************************************


 
int main(int argc, char **argv)
{

 
// Set up pointers for direct register access
map_peripherals();


// PWM channels 
pwm_disable(0);
pwm_disable(1);

pwm_create(18);
pwm_create(19);



// set pwm clock to 1 MHz (source = 19.2MHz)

float pwmclk= 1000000.0;
pwm_clock(pwmclk);



// set pwm mode, range and data parameters
/*
pwm_mode(0, PWM_MS);
pwm_mode(1, PWM_MS);

pwm_range(0, 20000);
pwm_data(0,1000);

pwm_range(1, 100);
pwm_data(1,10);
*/

/*
pwm_mode(0, PWM_ALGO);
pwm_mode(1, PWM_ALGO);

pwm_range(0, 100);
pwm_data(0,15);

pwm_range(1, 100);
pwm_data(1,1);
*/


pwm_mode(0, PWM_MS); // to see the difference between the two modes for the same setings
pwm_mode(1, PWM_ALGO);


pwm_range(0, 20000);
pwm_data(0,4000); // 4ms


pwm_range(1, 20000);
pwm_data(1,4000); // 4ms



/*
pwm_mode(0, PWM_SERIAL);
pwm_mode(1, PWM_SERIAL);

pwm_range(0, 32);
pwm_data(0,0xFF000000);

pwm_range(1, 32);
pwm_data(1,0xFFF00000);

*/


pwm_enable(0);
usleep(1);	// necessary to make it work...
pwm_enable(1);


  return 0;
 
} // main
 
 
//**************************************************************************************
//******************** subroutines *****************************************************
//**************************************************************************************


//
// Set up a memory regions to access peripherals
//
void map_peripherals()
{
int  mem_fd;

   /* open /dev/mem */
   if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
      perror("opening /dev/mem");
      exit(-1);
   }
 
   /* mmap GPIO */
   gpio_map = mmap(
      NULL,             //Any adddress in our space will do
      BLOCK_SIZE,       //Map length
      PROT_READ|PROT_WRITE,// Enable reading & writting to mapped memory
      MAP_SHARED,       //Shared with other processes
      mem_fd,           //File to map
      GPIO_BASE         //Offset to GPIO peripheral
   );
 
   
 
   if (gpio_map == MAP_FAILED) {
      perror("mmap(/dev/mem)");//errno also set!
      exit(-1);
   }
 
   // Always use volatile pointer!
   gpio = (volatile unsigned *)gpio_map;

   /* mmap PWM */
   pwm_map = mmap(
      NULL,             //Any adddress in our space will do
      BLOCK_SIZE,       //Map length
      PROT_READ|PROT_WRITE,// Enable reading & writting to mapped memory
      MAP_SHARED,       //Shared with other processes
      mem_fd,           //File to map
      PWM_BASE         //Offset to PWM peripheral
   );
 
   
 
   if (pwm_map == MAP_FAILED) {
      perror("mmap(/dev/mem)");//errno also set!
      exit(-1);
   }
 
   // Always use volatile pointer!
   pwm = (volatile unsigned *)pwm_map;

   /* mmap CLK */
   clk_map = mmap(
      NULL,             //Any adddress in our space will do
      BLOCK_SIZE,       //Map length
      PROT_READ|PROT_WRITE,// Enable reading & writting to mapped memory
      MAP_SHARED,       //Shared with other processes
      mem_fd,           //File to map
      CLOCK_BASE         //Offset to clock controller
   );
 
   
 
   if (clk_map == MAP_FAILED) {
      perror("mmap(/dev/mem)");//errno also set!
      exit(-1);
   }
 
   // Always use volatile pointer!
   clk = (volatile unsigned *)clk_map;


close(mem_fd); //No need to keep mem_fd open after mmap
 
 
} // setup_io


//
// sets the value of the PWM clock
// clkval = required clock frequency
// returns the actually computed frequency value which may be slightly different from the requested one
//
float pwm_clock(float clkval){

float effective_pwmclk;

	// stop clock and waiting for busy flag doesn't work, so kill clock
	CM_PWMCTL = 0x5A000000 | (1 << 5);
	usleep(10); 


	// set frequency (DIVI only)
	int idiv = (int) (19200000.0 / clkval);
	if (idiv < 1 || idiv > 0x1000) {
		printf("WARNING: clock division DIVI out of range: %x continue with DIVI=10\n", idiv);
		idiv = 10;
	}

	CM_PWMDIV  = 0x5A000000 | (idiv<<12);
	
	// source=19.2MHz osc, enable clock
	CM_PWMCTL = 0x5A000011;
	
	effective_pwmclk= 19200000.0/idiv;

	fprintf(stdout, "INFO: PWM clock requested: %9.1f HZ, actual value: %9.1f Hz\n", clkval,effective_pwmclk);
	return effective_pwmclk;
}

//
// sets the given port as PWM
// g = 12|13|18|19
// PWM0: 12 or 18, PWM1: 13 or 19
//
void pwm_create(int pinNumber){

switch(pinNumber)
{
case 12:
	{
	INP_GPIO(12); 
	INP_GPIO(18); 
//	SET_GPIO_ALT(12,0);
	*(gpio+1) |= 0x00000100;
	}
break;

case 13:
	{
	INP_GPIO(19); 
	INP_GPIO(13); 
//	SET_GPIO_ALT(13,0);
	*(gpio+1) |= 0x00000800;
	}
break;

case 18:
	{
	INP_GPIO(18); 
	INP_GPIO(12); 
//	SET_GPIO_ALT(18,5);
	*(gpio+1) |= 0x02000000;
	}
break;

case 19:
	{
	INP_GPIO(13); 
	INP_GPIO(19); 
//	SET_GPIO_ALT(19,5);
	*(gpio+1) |= 0x10000000;
	}
break;

default:
	{
	fprintf(stdout, "WARNING: Pin %d can't be configured as PWM, ignored\n", pinNumber);
	}

}
PWM_CTL = 0;	// resets everything in PWM control register (reset state)
usleep(10);  
 
}

//
// sets the range of the given PWM
// pwmNumber = 0|1  range <= 2^32 -1
// no test on range value!!
//
void pwm_range(int pwmNumber, unsigned int range){
switch(pwmNumber)
{
case 0:
	{
	PWM_RNG1 = range; //PWM0, channel 1
	}
break;

case 1:
	{
	PWM_RNG2 = range; //PWM1, channel 2
	}
break;

default:
	{
	fprintf(stdout, "WARNING: PWM %d doesn't exist, RANGE ignored\n", pwmNumber);
	}

}

}

//
// sets the range of the given PWM
// pwmNumber = 0|1  data <= 2^32 -1
// no test on data value!!
//
void pwm_data(int pwmNumber, unsigned int data){
switch(pwmNumber)
{
case 0:
	{
	PWM_DAT1 = data; //PWM0, channel 1
	}
break;

case 1:
	{
	PWM_DAT2 = data; //PWM1, channel 2
	}
break;

default:
	{
	fprintf(stdout, "WARNING: PWM %d doesn't exist, DATA ignored\n", pwmNumber);
	}

}

}

//
// enables the given PWM
// pwmNumber = 0|1 
//
void pwm_enable(int pwmNumber){

switch(pwmNumber)
{
case 0:
	{
	PWM_CTL |= 0x00000001; //PWM0, channel 1 (bits 31:16 must be 0)
	}
break;

case 1:
	{
	PWM_CTL |= 0x00000100; //PWM1, channel 2
	}
break;

default:
	{
	fprintf(stdout, "WARNING: PWM %d doesn't exist, ENABLE ignored\n", pwmNumber);
	}

}
}


//
// disables the given PWM
// pwmNumber = 0|1 
//
void pwm_disable(int pwmNumber){

switch(pwmNumber)
{
case 0:
	{
	PWM_CTL &= 0x0000FFFE; //PWM0, channel 1 (bits 31:16 must be 0)
	}
break;

case 1:
	{
	PWM_CTL &= 0x0000FEFF; //PWM1, channel 2
	}
break;

default:
	{
	fprintf(stdout, "WARNING: PWM %d doesn't exist, DISABLE ignored\n", pwmNumber);
	}

}
}

//
// sets the mode of the given PWM
// pwmNumber = 0|1 mode = 0|1|2 for pwm_algo, pwm_ms, serial
//
void pwm_mode(int pwmNumber, int mode){

switch(pwmNumber)	//(bits 31:16 must be 0)
{
case 0:
	{

	PWM_CTL &= 0x0000FF00;// PWM0 disabled
	usleep(10); 

	if (mode == 0)
	PWM_CTL &= 0x0000FF00; //PWM0, channel 1, reset bits 7 & 1 

	else if (mode == 1){
	PWM_CTL &= 0x0000FF00; //PWM0, channel 1, set bit 7, reset bit 1
	PWM_CTL |= 0x00000080;
	}
	else if (mode == 2){
	PWM_CTL &= 0x0000FF00; //PWM0, channel 1, reset bit 7, set bit 1
	PWM_CTL |= 0x00000002;
	}
	else{
	fprintf(stdout, "WARNING: PWM0 mode %d doesn't exist, defaults to pwm_ms\n", mode);
	PWM_CTL &= 0x0000FF00; //PWM0, channel 1, set bit 7, reset bit 1
	PWM_CTL |= 0x00000080;
	}
	}
break;

case 1:
	{

	PWM_CTL &= 0x000000FF; // PWM1 disabled
	usleep(10); 

	if (mode == 0)
	PWM_CTL &= 0x000000FF; //PWM1, channel 2, reset bits 15 & 9

	else if (mode == 1){
	PWM_CTL &= 0x000000FF; //PWM1, channel 2, set bit 15, reset bit 9
	PWM_CTL |= 0x00008000;
	}
	else if (mode == 2){
	PWM_CTL &= 0x000000FF; //PWM1, channel 2, reset bit 15, set bit 9
	PWM_CTL |= 0x00000200;
	}
	else{
	fprintf(stdout, "WARNING: PWM1 mode %d doesn't exist, defaults to pwm_ms\n", mode);
	PWM_CTL &= 0x000000FF; //PWM1, channel 2, set bit 15, reset bit 9
	PWM_CTL |= 0x00008000;
	}
break;
	}

default:
	{
	fprintf(stdout, "WARNING: PWM %d doesn't exist, MODE ignored\n", pwmNumber);
	}

}

}
