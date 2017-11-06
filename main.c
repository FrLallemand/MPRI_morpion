#include "jeu.h"
#include "mcts.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <float.h>
#include <stdbool.h>

int main(int argc, char *argv[]) {
	Regle r = MAX;
    if(argc == 2){
		if (!strcmp("robuste", argv[1])){
			r = ROBUSTE;
		} else if(!strcmp("max", argv[1])) {
			r = MAX;
		} else {
			printf("Règle non reconnue, veuillez choisir entre max et robuste.\n");
			return 1;
		}
	} else {
		printf("Pas de règle spécifiée, utilisation de la règle max.\n");
	}
	Coup * coup;
	FinDePartie fin;

	// initialisation
	Etat * etat = etat_initial();

	// Choisir qui commence :
	printf("Qui commence (0 : humain, 1 : ordinateur) ? ");
	scanf("%d", &(etat->joueur) );

	// boucle de jeu
	do {
		printf("\n");
		afficheJeu(etat);

		if ( etat->joueur == 0 ) {
			// tour de l'humain

			do {
				coup = demanderCoup();
			} while ( !jouerCoup(etat, coup) );

			free(coup);
		}
		else {
			// tour de l'Ordinateur

			ordijoue_mcts(etat, TEMPS, r);

		}

		fin = testFin( etat );
	}	while ( fin == NON ) ;

	printf("\n");
	afficheJeu(etat);

	if ( fin == ORDI_GAGNE )
		printf( "** L'ordinateur a gagné **\n");
	else if ( fin == MATCHNUL )
		printf(" Match nul !  \n");
	else
		printf( "** BRAVO, l'ordinateur a perdu  **\n");

	free(etat);
	return 0;

}
