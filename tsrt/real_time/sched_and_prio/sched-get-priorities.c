// ---------------------------------------------------------------------
// sched-get-priorities.c
// Fichier d'exemple du livre "Solutions Temps-Reel sous Linux"
// (C) 2012-2015 Christophe BLAESS
// http://christophe.blaess.fr
// ---------------------------------------------------------------------

#include <sched.h>
#include <stdio.h>
#include <stdlib.h>


int main(void)
{
// prints out the limit values of real-time priorities on the target system
	printf("SCHED_FIFO :  [%d - %d]\n",
	       sched_get_priority_min(SCHED_FIFO),
	       sched_get_priority_max(SCHED_FIFO));
	printf("SCHED_RR :    [%d - %d]\n",
	       sched_get_priority_min(SCHED_RR),
	       sched_get_priority_max(SCHED_RR));
	printf("SCHED_OTHER : [%d - %d]\n",
	       sched_get_priority_min(SCHED_OTHER),
	       sched_get_priority_max(SCHED_OTHER));
	return EXIT_SUCCESS;
}
