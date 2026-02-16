/*  blinks a led connected to GPIO24
*/

#include <stdio.h>
#include <stdlib.h>
#include <linux/types.h>
#include <unistd.h>

// standalone, no argument passed
int main(int argc, char *argv[]) {
  
FILE *fd;

// no error handling to make it simple, we are confident in our skills :)
fd = fopen("/sys/class/gpio/export", "w");
fprintf(fd, "%s", "24");
fclose(fd);

fd = fopen("/sys/class/gpio/gpio24/direction", "w");
fprintf(fd, "%s", "out");
fclose(fd);

fd = fopen("/sys/class/gpio/gpio24/value", "w");
fprintf(fd, "%s", "1");
fclose(fd);

fd = fopen("/sys/class/gpio/gpio24/value", "w");

  while (1) {
   	//usleep(500000);	// delay in microseconds
	//sleep(1);	// delay in seconds
	fprintf(fd, "%s", "1");
	rewind(fd);	// set the pointer to the beginning of the file, therefore the next write 
			// will overwrite the previous one
	//sleep(1);
    	//usleep(500000);
	fprintf(fd, "%s", "0"); 
	rewind(fd);    
  }
fclose(fd);
return 0;
}
