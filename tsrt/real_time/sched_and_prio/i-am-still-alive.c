/****************************************************************************\
test if Rpi is alive                                           **
\****************************************************************************/

#define _GNU_SOURCE  // sched_getcpu() est une extension GNU
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main(void)
{

	FILE *fd1;

	int n=sched_getcpu();	// gets original CPU number

		if (n == -1) {
			perror("sched_getcpu");
			exit(EXIT_FAILURE);
		}
	fprintf(stdout,"I am still alive process runs on CPU#%d\n",n);

	// create GPIO21 as output port

	fd1 = fopen("/sys/class/gpio/export", "w");
	fprintf(fd1, "21");
	fclose(fd1);

	fd1 = fopen("/sys/class/gpio/gpio21/direction", "w");
	fprintf(fd1, "out");
	fclose(fd1);
 
	fd1 = fopen("/sys/class/gpio/gpio21/value", "w");
	fprintf(fd1, "0");
	fclose(fd1);


	while(1){

	fd1 = fopen("/sys/class/gpio/gpio21/value", "w");
	fprintf(fd1, "1");
	fclose(fd1);
	usleep(50000);
	fd1 = fopen("/sys/class/gpio/gpio21/value", "w");
	fprintf(fd1, "0");
	fclose(fd1);
	usleep(50000);

	} //while (1)

	return EXIT_SUCCESS;
}

