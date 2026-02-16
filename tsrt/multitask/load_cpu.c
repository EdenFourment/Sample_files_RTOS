/****************************************************************************\
** Exemple de la formation "Temps-reel Linux et Xenomai                     **
**                                                                          **
** Christophe Blaess 2012                                                   **
** http://christophe.blaess.fr                                              **
** this program skips among all the available CPUs to print out a message   **
** 
\****************************************************************************/

#define _GNU_SOURCE  // sched_getcpu() is a GNU extension
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

static int fin_traitement = 0;

void handler_alarm(int unused)
{
	fin_traitement = 1;
}

int main(void)
{
	int compteur;
	int cpu;
	cpu_set_t cpu_set;
	
		cpu = 0;
	while(1){
	 
			CPU_ZERO(& cpu_set);
			CPU_SET(cpu, & cpu_set);
			sched_setaffinity(0, sizeof(cpu_set), & cpu_set);
			fprintf(stderr, "My PID is [%d], I run on CPU number %d\n",
			                 getpid(), sched_getcpu());
			


			signal(SIGALRM, handler_alarm);
			alarm(2);
		compteur=0;
		fin_traitement=0;
		while (! fin_traitement) {
		compteur ++;
		}//end while
	cpu++;
	if (cpu>3) cpu=0;

	}// end while
	return EXIT_SUCCESS;
}

