
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main(void)
{
	FILE * fp;
	time_t tm;
	
	sleep(10);


	fp = fopen("/dev/ttyAMA0", "w");

	fprintf(fp, "%s", "\n");
	fprintf(fp, "%s", "**********************************************\n");
	fprintf(fp, "%s", "************ minimal init process ************\n");
	fprintf(fp, "%s", "**********************************************\n");
	fprintf(fp, "%s", "\n");
	
	
	while (1) {
		time(& tm);
		fprintf(fp, "%s\n", ctime(& tm));
		fflush(fp);
		sleep(1);	
	}
	return EXIT_FAILURE;
}

