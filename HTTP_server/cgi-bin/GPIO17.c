/*
testing CGI scripts
brs - /11/2016

*/



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>





// prototypes

int main(int argc, char * argv[])
{


FILE *fd1; // for GPIO


// creating GPIO17 as output port
	fd1 = fopen("/sys/class/gpio/export", "w");
	fprintf(fd1, "17");
	fclose(fd1);

	fd1 = fopen("/sys/class/gpio/gpio17/direction", "w");
	fprintf(fd1, "out");
	fclose(fd1);
// testing if the correct number of arguments is present in $QUERY_STRING (skip if not or segmentation fault occurs!)
 if (argc == 2){

	if (strcmp(argv[1],"1") == 0){
	fd1 = fopen("/sys/class/gpio/gpio17/value", "w");
	fprintf(fd1, "1");
	fclose(fd1);
	} else {
	fd1 = fopen("/sys/class/gpio/gpio17/value", "w");
	fprintf(fd1, "0");
	fclose(fd1);
	}
}

return EXIT_SUCCESS;

}// end main


