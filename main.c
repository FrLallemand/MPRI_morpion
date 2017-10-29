#include "jeu.h"
#include "mcts.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <float.h>
#include <stdbool.h>

// Fonction de test, retourne 1 si le test d'une fonction échoue
// N'est pas une fonction de test unitaire au sens strict du termes
// puisque certains tests consiste en un affichage dont il faut
// vérifier la consistance
int test(){
    // Test de l'initialisation
    printf("On initialise le plateau de jeu :\n");
    Etat * etat = etat_initial();
    afficheJeu(etat);

    // Test d'un coup humain
    printf("On joue un coup en tant qu'humain dans la troisième colonne:\n");
    Coup * coup = nouveauCoup(3);
    jouerCoup(etat, coup);
    afficheJeu(etat);

    // Test d'un coup ordi
    printf("On joue un coup en tant qu'ordi dans la troisième colonne:\n");
	coup = nouveauCoup(2);
	jouerCoup(etat, coup);
	coup = nouveauCoup(2);
	jouerCoup(etat, coup);
	coup = nouveauCoup(1);
	jouerCoup(etat, coup);
	coup = nouveauCoup(0);
	jouerCoup(etat, coup);
	coup = nouveauCoup(1);
	jouerCoup(etat, coup);
	coup = nouveauCoup(1);
	jouerCoup(etat, coup);
	coup = nouveauCoup(0);
	jouerCoup(etat, coup);
	coup = nouveauCoup(0);
	jouerCoup(etat, coup);
	coup = nouveauCoup(6);
	jouerCoup(etat, coup);
	coup = nouveauCoup(0);
	jouerCoup(etat, coup);

	afficheJeu(etat);

	FinDePartie fin = testFin( etat );
	if ( fin == ORDI_GAGNE )
		printf( "** L'ordinateur a gagné **\n");
	else if ( fin == MATCHNUL )
		printf(" Match nul !  \n");
	else if ( fin == HUMAIN_GAGNE )
		printf( "** BRAVO, l'ordinateur a perdu  **\n");
	else
		printf(" Partie non finie !  \n");

    return 0;
}

int main(int argc, char *argv[]) {
    if(argc == 1){
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

            }
            else {
                // tour de l'Ordinateur

                ordijoue_mcts( etat, TEMPS, MAX);

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
        return 0;
    }
    else if (strcmp("test", argv[1]) == 0){
        return test();
    }
}
