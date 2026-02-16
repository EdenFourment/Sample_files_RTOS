/*  blinks a led connected to pin passed as argument
*/

#include <stdio.h>
#include <stdlib.h>
#include <linux/types.h>
#include <unistd.h>


int main(int argc, char *argv[]) {
  
	FILE *fd;
	char *byte;
	char path[100];

	if (argc != 2){
		fprintf(stderr, "usage: %s <port number>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	byte = argv[1];
	

// no error handling to make it simple, we are confident in our skills :)
fd = fopen("/sys/class/gpio/export", "w");
fprintf(fd, "%s", byte);
fclose(fd);

sprintf(path,"/sys/class/gpio/gpio%s/direction", byte);

//fprintf(stdout,"%s\n",path);


fd = fopen(path, "w");
fprintf(fd, "%s", "out");
fclose(fd);

sprintf(path,"/sys/class/gpio/gpio%s/value", byte);

fd = fopen(path, "w");
fprintf(fd, "%s", "1");
fclose(fd);

fd = fopen(path, "w");

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
