// ---------------------------------------------------------------------
// exemple-perturbateur.c
// Fichier d'exemple du livre "Solutions Temps-Reel sous Linux"
// (C) 2012-2015 - Christophe BLAESS
// http://christophe.blaess.fr
// ---------------------------------------------------------------------

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>


int main(void)
{
	struct timeval heure;
	struct timeval debut_boucle;
	struct timeval debut_programme;
	long long int  difference;

	gettimeofday(& debut_programme, NULL);

	while (1) {
		gettimeofday(& debut_boucle, NULL);
		do {
			gettimeofday(& heure, NULL);
			difference  = heure.tv_sec - debut_boucle.tv_sec;
			difference *= 1000000;
			difference += heure.tv_usec - debut_boucle.tv_usec;
		} while (difference < 100);

		usleep(10);

		difference  = heure.tv_sec - debut_programme.tv_sec;
		difference *= 1000000;
		difference += heure.tv_usec - debut_programme.tv_usec;
	
		if ((heure.tv_sec - debut_programme.tv_sec) > 20)
			break;
	}
	return EXIT_SUCCESS;
}
