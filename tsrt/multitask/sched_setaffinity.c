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

int main(void)
{
	int cpu;
	cpu_set_t cpu_set;
	while (1) {
		for (cpu = 0; cpu < sysconf(_SC_NPROCESSORS_ONLN); cpu ++) {
			CPU_ZERO(& cpu_set);
			CPU_SET(cpu, & cpu_set);
			sched_setaffinity(0, sizeof(cpu_set), & cpu_set);
			fprintf(stderr, "My PID is [%d], I run on CPU number %d\n",
			                 getpid(), sched_getcpu());
			sleep(1);
		}
	}
	return EXIT_SUCCESS;
}

