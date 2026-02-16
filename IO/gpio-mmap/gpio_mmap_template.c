//
//  How to use GPIO from C-code on the Raspberry-Pi
//  compilation of many sources including:
// Gert van Loo & Dom
// Hussam al-Hertani (hertaville.com)
// Christophe Blaess (www.blaess.fr)
//
//  B. Stefanelli / Revised: 14 dec 2021
 
 
// Access from ARM Running Linux
 
//#define BCM2708_PERI_BASE        0x20000000	//RPi 1
#define BCM2708_PERI_BASE	 0x3F000000	//RPi 2 & 3
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */


 
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
 
#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)
 

void *gpio_map;

 
// I/O access
volatile unsigned *gpio;

 
 
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

#define INPUT 1
#define OUTPUT 0
#define PULL_UP 1
#define PULL_DOWN 0
#define FLOAT -1

//**************************************************************************************
//********************* function prototypes ********************************************
//**************************************************************************************
void map_peripherals();
int gpio_get(int);
void gpio_set(int);
void gpio_clr(int);
void gpio_pud(int, int);
void gpio_direction(int, int);


//**************************************************************************************
//******************** main ************************************************************
//**************************************************************************************


 
int main(int argc, char **argv){

// put your code here!
 
// Set up pointers for direct register access



// Set up IO ports 




while (1){

// toggle port


// toggle speed test


// in-out test: gpio24 reflects the state of gpio23



// pull-up pull-down test: gpio23 toggles between 0 and 1 using pull-up/pull-down, gpio24 reflects the state of gpio23


}// while



  return 0;
 
} // end  main
 
 
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

 
close(mem_fd); //No need to keep mem_fd open after mmap
 
 
} // end map_peripherals


//
// controls pull-up, pull-down
// g = GPIO number  pud: 0=down  1=up  -1=floating input
//
void gpio_pud(int g, int pud){

if (pud  == -1)
GPIO_PULL = 0;
else if (pud == 0)
GPIO_PULL = 1;
else if (pud == 1)
GPIO_PULL = 2;
else {
GPIO_PULL = 0;
fprintf(stdout, "pullup-pulldown argument pud must be either 0=down  1=up  -1=floating input. continue with floating input\n");
}

usleep(10);

if (g>= 32)
GPIO_PULLCLK1 = (1 << (g % 32));
else
GPIO_PULLCLK0 = (1 << g);

usleep(10);

GPIO_PULL = 0;

if (g>= 32)
GPIO_PULLCLK1 = 0;
else
GPIO_PULLCLK0 = 0;

}
// end gpio_pud

//
// gets the value at an input port
// g = GPIO number  
// returns 0 or 1 (low / high)
//

int gpio_get(int g)
{
  if (GET_GPIO(g)) // !=0 <-> bit is 1 <- port is HIGH=3.3V
    return 1;
  else // port is LOW=0V
    return 0;
}// end gpio_get

//
// sets the value at an output port
// g = GPIO number
//
//
void gpio_set(int g){

if (g>= 32)
GPIO_SET_EXT = (1 << (g % 32));	// GPIO_SET is a pointer, GPIO_SET = xx writes xx in the location pointed by GPIO_SET
else
GPIO_SET = (1 << g);

}// end gpio_set

//
// clears the value at an output port
// g = GPIO number
//
//
void gpio_clr(int g){

if (g>= 32)
GPIO_CLR_EXT = (1 << (g % 32));
else
GPIO_CLR = (1 << g);

}// end gpio_clear

//
// sets the direction of a port
// g = GPIO number dir = 1=input  0=output
//
//
void gpio_direction(int g, int dir){
if (dir == 1)
INP_GPIO(g); 
else if (dir == 0){
INP_GPIO(g); // must use INP_GPIO before we can use OUT_GPIO
OUT_GPIO(g);
}
else{
fprintf(stdout, "WARNING: pin %d direction must be either 1 (input) or 0 (output), input assumed\n", g);
INP_GPIO(g);
}
}// end gpio_direction


