// ---------------------------------------------------------------------
// exemple-timer-create-02.c
// Fichier d'exemple du livre "Solutions Temps-Reel sous Linux"
// (C) 2012-2015 - Christophe BLAESS
// http://christophe.blaess.fr
// ---------------------------------------------------------------------

#define _GNU_SOURCE  
#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>



static timer_t timer = (timer_t) 0;
static long int * measurement = NULL;
static int meas_number = 0;
static int total_meas_number = 0;

void handler_signal(int not_used)
{
	struct timespec hour;
	static struct timespec previous = { 0, 0} ;

	clock_gettime (CLOCK_REALTIME, & hour);
	if (previous.tv_sec > 0) {
		measurement[meas_number]  = hour.tv_sec  - previous.tv_sec;
		measurement[meas_number] *= 1000000000;
		measurement[meas_number] += hour.tv_nsec - previous.tv_nsec;
		meas_number ++;
	}
	previous = hour;
	if (meas_number == total_meas_number)
		timer_delete(timer);
}


int main(int argc, char * argv[])
{
	long int period;
	struct sigevent event;
	struct itimerspec spec;
	struct sched_param param;
	FILE *fp;


	
	if ((argc != 2) || (sscanf(argv[1], "%ld", & period) != 1)){
		fprintf(stderr, "usage: %s period_in_microsec\n", argv[0]);
		exit(EXIT_FAILURE);
	}


	sched_getparam(0, &param);
	fprintf(stderr, "Posix-timer-create-03: PID= %d,CPU= %d, scheduler= %d priority= %d\n",
			                getpid(), sched_getcpu(), sched_getscheduler(0), param.sched_priority);

	// Allouer le tableau de measurements
	total_meas_number = 5000000 / period; // total of 5 sec.
	if ((measurement = calloc(total_meas_number, sizeof(struct timespec))) == NULL) {
		perror("calloc");
		exit(EXIT_FAILURE);
	}

	// Configurer le timer
	signal(SIGRTMIN, handler_signal);
	event.sigev_notify = SIGEV_SIGNAL;
	event.sigev_signo  = SIGRTMIN;
	period = period * 1000;  // en nanosec
	spec.it_interval.tv_sec  = period / 1000000000;
	spec.it_interval.tv_nsec = period % 1000000000;
	spec.it_value = spec.it_interval;
	// Allouer le timer
	if (timer_create(CLOCK_REALTIME, & event, & timer) != 0) {
		perror("timer_create");
		exit(EXIT_FAILURE);
	} 
	// Programmer le timer
	if (timer_settime(timer, 0, &spec, NULL) != 0) {
		perror("timer_settime");
		exit(EXIT_FAILURE);
	}
	// Attendre le dernier signal
	while (meas_number < total_meas_number)
		pause();
	// stocker les measurements en micro-secondes

	fp = fopen("/tmp/delta.txt", "w");
	for (meas_number = 0; meas_number < total_meas_number; meas_number ++)
		fprintf(fp, "%ld\n", measurement[meas_number]/1000);
	fclose(fp);
	return EXIT_SUCCESS;
}

