/*
passive waiting using select()

brs - 09/11/2016
adapted from: Christophe Blaess (http://www.blaess.fr/christophe/2013/04/15/attentes-passives-sur-gpio)

*/

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <signal.h>

#define MAXNBMEAS 10000

// function prototype
void sighandler(int);




int main(int argc, char * argv[]){
	FILE *fd1, *fd2, *fp;
	int fd;
	fd_set fds;	// this declares a list of file descriptors called fds
	struct timeval tv[MAXNBMEAS];
	char buffer[2];
	signal(SIGINT, sighandler);
	int sample;

	unsigned long delta;

	int nbmeas, pwm_freq;

	if ((argc != 2) || (sscanf(argv[1], "%d", & pwm_freq) != 1)){
		fprintf(stderr, "usage: %s PWM_frequency_in_Hz\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	// for 10 second measurement, but 10000 samples max
	nbmeas = pwm_freq*10;
	if (nbmeas > 10000) nbmeas=10000;

	fprintf(stderr,"Number of samples = %d\n",nbmeas);

	// create GPIO4 as input port, select falling edge for interruption event

	fd1 = fopen("/sys/class/gpio/export", "w");
	fprintf(fd1, "4");
	fclose(fd1);

	fd1 = fopen("/sys/class/gpio/gpio4/direction", "w");
	fprintf(fd1, "in");
	fclose(fd1);
 
	fd1 = fopen("/sys/class/gpio/gpio4/edge", "w");
	fprintf(fd1, "rising");
	fclose(fd1);


	//Open the "value" file
	if ((fd = open("/sys/class/gpio/gpio4/value", O_RDONLY)) < 0) {
		perror("/sys/class/gpio/gpio4/value");
		exit(EXIT_FAILURE);
	}


	sample=0;

	while (sample < nbmeas) {
		// FD_ZERO is a macro that resets the list of file descriptors on which  we are waiting for particular events
		FD_ZERO(& fds);
		// adds the file descriptor previously obtained to the list
		FD_SET(fd, & fds);


		// passive waiting
		// fd+1: fd is the only (and thus the highest) file desriptor we use
		// NULL: list of file descriptors checked for read (empty here)
		// NULL: list of file descriptors checked for write (empty here)
		// &fds: list of file descriptors checked for event, contains fd 
		// NULL: means no timeout, therefore infinite waiting...
		if (select(fd+1, NULL, NULL, & fds, NULL) < 0) {
			perror("select");
			break;
		}

		// the process is stopped at this point and enters passive waiting.
		// It will resume when an interruption occurs and then....

		// reads the time at which the event occured
		gettimeofday(& tv[sample], NULL);
		// rewinds the file (necessary for multiple readings).
		lseek(fd, 0, SEEK_SET);
		//******* WARNING: we MUST read the GPIO value (2 bytes) **********
		if (read(fd, & buffer, 2) != 2) {
			perror("read");
			break;
		}
		// removes cariage return
		buffer[1] = '\0';

		sample++;
	}


		//prints out time 


		fp = fopen("/tmp/delta.txt", "w");
		for (sample=0; sample<nbmeas-1; sample++){
		delta = 1e6 * (tv[sample+1].tv_sec - tv[sample].tv_sec) +  tv[sample+1].tv_usec - tv[sample].tv_usec;
		fprintf(fp, "%ld\n", delta);
		}
		fclose(fp);

		fd2 = fopen("/tmp/raw_data.txt", "w");
		fprintf(fd2, "%ld%06ld %d\n", tv[0].tv_sec, tv[0].tv_usec, 0);
		for (sample=1; sample<nbmeas; sample++){
		delta = 1e6 * (tv[sample].tv_sec - tv[sample-1].tv_sec) +  tv[sample].tv_usec - tv[sample-1].tv_usec;
		fprintf(fd2, "%ld%06ld %ld\n", tv[sample].tv_sec, tv[sample].tv_usec, delta);
		//fprintf(stdout, "%ld\n", delta);
		}
		fclose(fd2);

		fd2 = fopen("/tmp/data.txt", "w");
		for (sample=0; sample<nbmeas-1; sample++){
		delta = 1e6 * (tv[sample+1].tv_sec - tv[sample].tv_sec) +  tv[sample+1].tv_usec - tv[sample].tv_usec;
		fprintf(fd2, "%d %ld\n",sample, delta);
		//fprintf(stdout, "%ld\n", delta);
		}
		fclose(fd2);

	close(fd);
	return EXIT_SUCCESS;
}


void sighandler(int signum){
	FILE *fd1;
	fd1 = fopen("/sys/class/gpio/unexport", "w");
	fprintf(fd1, "4");
	fclose(fd1);

}


