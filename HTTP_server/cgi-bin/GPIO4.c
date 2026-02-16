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
 int value = 0;

// testing if the correct number of arguments is present in $QUERY_STRING (skip if not or segmentation fault occurs!)
 if (argc == 1){

	fd1 = fopen("/sys/class/gpio/gpio4/value", "r");
	if (fd1 == NULL){
	fprintf(stdout,"<li>can't access</li>");
	} else {
	fscanf(fd1,"%d",&value);
	fprintf(stdout, "<li>GPIO4 value = %d</li>", value);;
	}
}

return 0;

}// end main


