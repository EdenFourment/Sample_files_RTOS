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


// function prototype
void sighandler(int);




int main(int argc, char * argv[]){
	FILE *fd1;
	int fd;
	fd_set fds;	// this declares a list of file descriptors called fds
	struct timeval tv;
	char buffer[2];
	signal(SIGINT, sighandler);	// to catch signal issued from "ctrl-C"
	signal(SIGTERM, sighandler);	// to catch signal issued from "kill" shell command

	// create GPIO4 as input port, select falling edge for interruption event

	fd1 = fopen("/sys/class/gpio/export", "w");
	fprintf(fd1, "4");
	fclose(fd1);

	fd1 = fopen("/sys/class/gpio/gpio4/direction", "w");
	fprintf(fd1, "in");
	fclose(fd1);
 
	fd1 = fopen("/sys/class/gpio/gpio4/edge", "w");
	fprintf(fd1, "falling");
	fclose(fd1);


	//Open the "value" file
	if ((fd = open("/sys/class/gpio/gpio4/value", O_RDONLY)) < 0) {
		perror("/sys/class/gpio/gpio4/value");
		exit(EXIT_FAILURE);
	}

	while (1) {
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
		gettimeofday(& tv, NULL);
		// rewinds the file (necessary for multiple readings).
		lseek(fd, 0, SEEK_SET);
		//******* WARNING: we MUST read the GPIO value (2 bytes) **********
		if (read(fd, & buffer, 2) != 2) {
			perror("read");
			break;
		}
		// removes cariage return
		buffer[1] = '\0';
		//displays time and GPIO state
		fprintf(stdout, "[%ld.%06ld]: %s\n", tv.tv_sec, tv.tv_usec, buffer);
	}
	close(fd);
	return EXIT_SUCCESS;
}


void sighandler(int signum){	// releases the GPIO used to ensure a clean end of the process
	FILE *fd1;
	fd1 = fopen("/sys/class/gpio/unexport", "w");
	fprintf(fd1, "4");
	fclose(fd1);

}


