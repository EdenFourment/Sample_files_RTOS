/*  toggles a GPIO @ max speed
*/

#include <stdio.h>
#include <stdlib.h>
#include <linux/types.h>
#include <unistd.h>

// standalone, no argument passed
int main(int argc, char *argv[]) {
  
	FILE *fd;
	char *byte1;
	char path[100];
	int nbsec;

	if (argc != 3){
		fprintf(stderr, "usage: %s <port number> <seconds>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	byte1 = argv[1];

	

// no error handling to make it simple, we are confident in our skills :)
fd = fopen("/sys/class/gpio/export", "w");
fprintf(fd, "%s", byte1);
fclose(fd);

sprintf(path,"/sys/class/gpio/gpio%s/direction", byte1);

//fprintf(stdout,"%s\n",path);


fd = fopen(path, "w");
fprintf(fd, "%s", "out");
fclose(fd);

sprintf(path,"/sys/class/gpio/gpio%s/value", byte1);

fd = fopen(path, "w");
fprintf(fd, "%s", "1");
fclose(fd);

fd = fopen(path, "w");

sscanf(argv[2], "%d", &nbsec);
if (nbsec<0) nbsec = -nbsec;

alarm((unsigned int)nbsec);

  while (1) {

	fprintf(fd, "%s", "1");
	rewind(fd);	// set the pointer to the beginning of the file, therefore the next write 
			// will overwrite the previous one
	
	fprintf(fd, "%s", "0"); 
	rewind(fd);  
 
  }
fclose(fd);


return 0;
}
