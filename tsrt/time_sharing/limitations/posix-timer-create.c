// ---------------------------------------------------------------------
// exemple-timer-create-01.c
// Fichier d'exemple du livre "Solutions Temps-Reel sous Linux"
// (C) 2012-2015 - Christophe BLAESS
// http://christophe.blaess.fr
// ---------------------------------------------------------------------

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
//#include <sys/time.h>

void handler_signal_1(int not_used)
{
	fprintf(stderr, "1 ");
}

void handler_signal_2(int not_used)
{
	fprintf(stderr, "2 ");
}


int main(int argc, char * argv[])
{
	timer_t timer_1, timer_2;
	struct sigevent event_1, event_2;
	struct itimerspec spec_1, spec_2;

	// Install the signal handlers 
	signal(SIGRTMIN+1, handler_signal_1);
	signal(SIGRTMIN+2, handler_signal_2);

/* from http://www.linuxprogrammingblog.com/all-about-linux-signals?page=9
Linux signals are software interruptions.
The POSIX specification defines so called real-time signals and Linux supports it. They are to be used by the programmer
and have no predefined meaning. Two macros are available: SIGRTMIN and SIGRTMAX that tells the range of these signals. 
You can use one using SIGRTMIN+n where n is some number. Never hard code their numbers, real time signals are used by 
threading library (both LinuxThreads and NTPL), so they adjust SIGRTMIN at run time.

Whats the difference between RT signals and standard signals? There are couple:

- More than one RT signal can be queued for the process if it has the signal blocked while someone sends it. In standard
  signals only one of a given type is queued, the rest is ignored.
- Order of delivery of RT signal is guaranteed to be the same as the sending order.
- PID and UID of sending process is written to si_pid and si_uid fields of siginfo_t. For more information see section about
  Real time signals in signal(7).

*/

	// Indicate the required  notification for timer 1
	event_1.sigev_notify = SIGEV_SIGNAL;	// send a signal
	event_1.sigev_signo  = SIGRTMIN+1;	// the signal to send is SIGRTMIN+1

	// Configure the period for timer 1
	spec_1.it_interval.tv_sec  = 1;	// 1 sec.
	spec_1.it_interval.tv_nsec = 0; // + 000000000 ns.
	spec_1.it_value = spec_1.it_interval;
	
	// create timer 1
	if (timer_create(CLOCK_REALTIME, & event_1, & timer_1) != 0) {
		perror("timer_create");
		exit(EXIT_FAILURE);
	} 

	// same as before for timer 2
	event_2.sigev_notify = SIGEV_SIGNAL;
	event_2.sigev_signo  = SIGRTMIN+2;
	spec_2.it_interval.tv_sec  = 0;	// 0 sec.
	spec_2.it_interval.tv_nsec = 250000000; // + 250000000 ns = 0,25 sec.
	spec_2.it_value = spec_2.it_interval;
	if (timer_create(CLOCK_REALTIME, & event_2, & timer_2) != 0) {
		perror("timer_create");
		exit(EXIT_FAILURE);
	} 

	// Program both timers
	if ((timer_settime(timer_1, 0, &spec_1, NULL) != 0)
	 || (timer_settime(timer_2, 0, &spec_2, NULL) != 0)) {
		perror("timer_settime");
		exit(EXIT_FAILURE);
	}
	while (1) 
		pause();
	//pause() causes the calling process (or thread) to sleep until a signal is delivered
	//that either terminates the process or causes the invocation of a signal-catching function. 

	return EXIT_SUCCESS;

}

