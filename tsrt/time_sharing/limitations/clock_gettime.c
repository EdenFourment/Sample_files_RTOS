
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define NB_SAMPLES 100

int main (void)
{
	int i;
	long delta;

	struct timespec ts[NB_SAMPLES];

	for (i = 0; i < NB_SAMPLES; i++)
		clock_gettime(CLOCK_REALTIME, & (ts[i]));

	// printing out results (remark: %09ld forces display on 9 digits, zeroes are added in front of
	// signficant digits if necessary, "l" stands for long, "d" for integer as usual)
	// the current time is printed out along with its offset from the previous one  

	fprintf(stdout, "[0] current: %ld.%09ld sec, delta = 0\n", ts[0].tv_sec, ts[0].tv_nsec);

	for (i = 1; i < NB_SAMPLES; i++){
	if (ts[i].tv_sec == ts[i-1].tv_sec) {
	delta = ts[i].tv_nsec-ts[i-1].tv_nsec;
	} 
	else {
	delta = 1000000000+ts[i].tv_nsec-ts[i-1].tv_nsec;
	}
	fprintf(stdout, "[%d] current: %ld.%09ld sec, delta = %ld ns\n", i, ts[i].tv_sec, ts[i].tv_nsec, delta);
	}
	return EXIT_SUCCESS;
}
