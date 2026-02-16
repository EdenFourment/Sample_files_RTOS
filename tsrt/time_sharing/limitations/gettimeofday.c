
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#define NB_SAMPLES 100

int main (void)
{
	int i;
	long delta;

	struct timeval ts[NB_SAMPLES];

	for (i = 0; i < NB_SAMPLES; i++)
		gettimeofday(& (ts[i]), NULL);

	// printing out results (remark: %06ld forces display on 6 digits, zeroes are added in front of
	// signficant digits if necessary, "l" stands for long, "d" for integer as usual)
	// the current time is printed out along with its offset from the previous one  

	fprintf(stdout, "[0] current: %ld.%06ld sec, delta = 0\n", ts[0].tv_sec, ts[0].tv_usec);

	for (i = 1; i < NB_SAMPLES; i++){
	if (ts[i].tv_sec == ts[i-1].tv_sec) {
	delta = ts[i].tv_usec-ts[i-1].tv_usec;
	} 
	else {
	delta = 1000000+ts[i].tv_usec-ts[i-1].tv_usec;
	}
	fprintf(stdout, "[%d] current: %ld.%06ld sec, delta = %06ld us\n", i, ts[i].tv_sec, ts[i].tv_usec, delta);
	}
	return EXIT_SUCCESS;
}
