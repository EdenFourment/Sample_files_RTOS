// ---------------------------------------------------------------------
// exemple-taux-cpu.c
// Fichier d'exemple du livre "Solutions Temps-Reel sous Linux"
// (C) 2012-2015 - Christophe BLAESS
// http://christophe.blaess.fr
// ---------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>


int main(int argc, char * argv[])
{
	time_t debut;
	long int nb_boucles;
	long int nb_boucles_max;

	debut = time(NULL);

	nb_boucles = 0;
	while (time(NULL) < (debut + 10)) {
		nb_boucles ++;
	}
	fprintf(stdout, "[%d] number of loops : %ld ", getpid(), nb_boucles);
	if ((argc == 2) && (sscanf(argv[1], "%ld", & nb_boucles_max) == 1)) {
		fprintf(stdout, "(used %.0f percent of CPU)", 100.0 * (float) nb_boucles / nb_boucles_max);
	}
	fprintf(stdout, "\n");
	return EXIT_SUCCESS;
}

