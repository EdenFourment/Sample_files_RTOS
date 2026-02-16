/****************************************************************************\
** Exemple de la formation "Temps-reel Linux et Xenomai                     **
**                                                                          **
** Christophe Blaess 2012                                                   **
** http://christophe.blaess.fr                                              **
\****************************************************************************/

#define _GNU_SOURCE  // sched_getcpu() est une extension GNU
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main(void)
{
	int n;
	int precedent = -1;
	time_t heure;
	struct tm * tm_heure;

	n=sched_getcpu();	// gets original CPU number
		if (n == -1) {
			perror("sched_getcpu");
			exit(EXIT_FAILURE);
		}
	fprintf(stdout,"CPU is: %d\n",n);
	
	while (1) {
		n=sched_getcpu();
		if (n == -1) {
			perror("sched_getcpu");
			exit(EXIT_FAILURE);
		}
		if (precedent == -1)
			precedent = n;
		if (n != precedent) {	// prints message if new CPU is different from previous one
			heure = time(NULL);
			tm_heure = localtime(& heure);
			fprintf(stdout, "%02d:%02d:%02d CPU migration %d -> %d\n",
			         tm_heure->tm_hour, tm_heure->tm_min, tm_heure->tm_sec,
			         precedent, n);
			precedent = n;
		}
	}
	return EXIT_SUCCESS;
}

