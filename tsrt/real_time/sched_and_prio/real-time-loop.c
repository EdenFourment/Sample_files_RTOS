// ---------------------------------------------------------------------
// exemple-boucle-temps-reel.c
// Fichier d'exemple du livre "Solutions Temps-Reel sous Linux"
// (C) 2012-2015 Christophe BLAESS
// http://christophe.blaess.fr
// ---------------------------------------------------------------------

#define _GNU_SOURCE  // Pour avoir sched_setaffinity
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


void child_process (int cpu);

int main(void)
{
	int nb_cpus = sysconf(_SC_NPROCESSORS_ONLN);
	int cpu;
	
	/* creates a child process on each CPU */
	for (cpu = 0; cpu < nb_cpus; cpu ++) {
		if (fork() == 0) {
			child_process(cpu);
		}
	}
	/* waiting for child processes termination*/
		fprintf(stdout, "Wait for childs completion\n");
	for (cpu = 0; cpu < nb_cpus; cpu ++) waitpid(-1, NULL, 0);
		fprintf(stdout, "All childs terminated\n");
	return EXIT_SUCCESS;
}


void child_process (int cpu)
{
	struct sched_param param;
	cpu_set_t  cpuset;

	/* set cpu affinity of this child on the cpu number passed as argument  */
	CPU_ZERO(& cpuset);
	CPU_SET(cpu, &cpuset);
	if (sched_setaffinity(0, sizeof(cpuset), & cpuset) !=0) {
		perror("sched_setaffinity");
		exit(EXIT_FAILURE);
	}
	/* set scheduler and RT priority */
	param.sched_priority = 99;
	if (sched_setscheduler(0, SCHED_FIFO, & param) != 0) {
		perror("sched_setscheduler");
		exit(EXIT_FAILURE);
	}
	/* wait 1+cpu seconds  to ensure that all the childs are on their respective CPU*/
	sleep(1+cpu);
	/* generates an SIGALARM signal after 15 seconds. Because there is no handler defined for this signal,
	 the process will be killed after 15 seconds. This is  exactely what we are aiming at. */
	alarm(15);
	/* active loop to consume CPU time */
	fprintf(stdout, "New child on CPU #%d\n", cpu);
	while (1)
		;
}

