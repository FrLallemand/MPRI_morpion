#include "jeu.h"

#include <stdio.h>
#include <stdlib.h>


// Copier un état
Etat * copieEtat( Etat * src ) {
    Etat * etat = (Etat *)malloc(sizeof(Etat));

    etat->joueur = src->joueur;

    /* Copie d'un état de puissance 4 */
    int i,j;
    for (i=0; i < LIGNES; i++)
        for ( j=0; j < COLONNES; j++)
            etat->plateau[i][j] = src->plateau[i][j];



    return etat;
}

// Etat initial
Etat * etat_initial( void ) {
    Etat * etat = (Etat *)malloc(sizeof(Etat));

    // Initialisation du plateau de puissance 4
    int i,j;
    for (i=0; i < LIGNES; i++)
        for ( j=0; j < COLONNES; j++)
            etat->plateau[i][j] = VIDE;

    return etat;
}


void afficheJeu(Etat * etat) {
    /* par exemple : */
    int i,j;

    printf("   |");
    for ( j = 0; j < COLONNES; j++)
        printf(" %d |", j);
    printf("\n");

    printf("   ");
    for(j = 0; j < COLONNES; j++)
        printf("----");
    printf("-\n");

    for(i=0; i < LIGNES; i++) {
        printf(" %d |", i);
        for ( j = 0; j < COLONNES; j++){
            //printf(" %c |", etat->plateau[i][j]);
            switch(etat->plateau[i][j]){
            case VIDE:
                printf("   |");
                break;
            case PION_ORDI:
                printf(" O |");
                break;
            case PION_HUM:
                printf(" H |");
                break;
            }
        }
        printf("\n   ");
        for(j = 0; j < COLONNES; j++)
            printf("----");
        printf("-\n");
    }
}


// Nouveau coup
Coup * nouveauCoup( int i ) {
    Coup * coup = (Coup *)malloc(sizeof(Coup));

    // colonne sur lequelle le pion sera ajouté
    coup->colonne = i;

    return coup;
}

// Demander à l'humain quel coup jouer
Coup * demanderCoup () {

    // On demande dans quelle colonne le joueur souhaite mettre un pion
    int i;
    printf("\n quelle colonne ? ") ;
    scanf("%d",&i);

    return nouveauCoup(i);
}


// Retourne la position (ligne) disponible sur la colonne
// Retourne -1 si aucune position disponible
int hautColonne(Etat * etat, int i){
    /*
      TODO Peut être plus efficace de stocker la position
      et la modifier à chaque coup joué ?
    */
    int j;
    j = LIGNES-1;
    while(etat->plateau[j][i] != VIDE && j>=0){
        j--;
    }
    return j;
}


// Modifier l'état en jouant un coup
// retourne 0 si le coup n'est pas possible
int jouerCoup( Etat * etat, Coup * coup ) {

    // TODO: à compléter

    /* par exemple : */
    int position = hautColonne(etat, coup->colonne);
    if(position <= -1){
        return 0;
    }
    else {
        etat->plateau[position][coup->colonne] = etat->joueur ? PION_ORDI : PION_HUM;

        // à l'autre joueur de jouer
        etat->joueur = AUTRE_JOUEUR(etat->joueur);

        return 1;
    }
}

// Retourne une liste de coups possibles à partir d'un etat
// (tableau de pointeurs de coups se terminant par NULL)
Coup ** coups_possibles( Etat * etat ) {

    Coup ** coups = (Coup **) malloc((1+LARGEUR_MAX) * sizeof(Coup *) );

    int k = 0;

    // Au puissance 4, on a un coup par colonne
    int i;
    for(i=0; i < COLONNES; i++) {
        if ( hautColonne(etat, i) > -1) {
			coups[k] = nouveauCoup(i);
			k++;
		}
	}
    /* fin de l'exemple */

    coups[k] = NULL;

    return coups;
}


// Retourne la taille de la liste de coups possibles à partir d'un etat
int nb_coups_possibles( Etat * etat ) {
    int k = 0;

    // Au puissance 4, on a un coup par colonne
    int i;
    for(i=0; i < COLONNES; i++) {
        if ( hautColonne(etat, i) > -1) {
			k++;
		}
	}

    return k;
}

// Compare deux coups. (vérification basée sur le numéro de colonne, cf règles du puissance 4)
// Retour :
// 1 si identiques
// 0 si différents
int coups_egaux(Coup * coup1, Coup * coup2){
	if(coup1->colonne == coup2->colonne){
		return 1;
	}
	return 0;
}


// Test si l'état est un état terminal
// et retourne NON, MATCHNUL, ORDI_GAGNE ou HUMAIN_GAGNE
FinDePartie testFin( Etat * etat ) {
    // tester si un joueur a gagné
    int i,j,k,n = 0;
    for (i=0; i < LIGNES; i++) {
        for(j=0; j < COLONNES; j++) {
            if (etat->plateau[i][j] != VIDE) {
                n++;	// nb coups joués

                // Vérifie sur la colonne
                k=0;
                while ( k < 4 && i+k < LIGNES && etat->plateau[i+k][j] == etat->plateau[i][j] )
					k++;
                if ( k == 4 )
					return etat->plateau[i][j] == PION_ORDI ? ORDI_GAGNE : HUMAIN_GAGNE;

                // Vérifie sur la ligne
                k=0;
                while ( k < 4 && j+k < COLONNES && etat->plateau[i][j+k] == etat->plateau[i][j] )
                    k++;
                if ( k == 4 )
                    return etat->plateau[i][j] == PION_ORDI ? ORDI_GAGNE : HUMAIN_GAGNE;

                // diagonales
                k=0;
                while ( k < 4 && i+k < LIGNES && j+k < COLONNES && etat->plateau[i+k][j+k] == etat->plateau[i][j] )
                    k++;
                if ( k == 4 )
                    return etat->plateau[i][j] == PION_ORDI ? ORDI_GAGNE : HUMAIN_GAGNE;

                k=0;
                while ( k < 4 && i+k < LIGNES && j-k >= 0 && etat->plateau[i+k][j-k] == etat->plateau[i][j] )
                    k++;
                if ( k == 4 )
                    return etat->plateau[i][j] == PION_ORDI ? ORDI_GAGNE : HUMAIN_GAGNE;
            }
        }
    }

    // et sinon tester le match nul
    if ( n == LIGNES*COLONNES )
        return MATCHNUL;

    return NON;
}
