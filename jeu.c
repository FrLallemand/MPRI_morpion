/*
  Canvas pour algorithmes de jeux à 2 joueurs

  joueur 0 : humain
  joueur 1 : ordinateur

*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <float.h>

// Paramètres du jeu
#define LARGEUR_MAX 9 		// nb max de fils pour un noeud (= nb max de coups possibles)

#define TEMPS 5		// temps de calcul pour un coup avec MCTS (en secondes)

//Dimensions standards d'un jeu de puissance 4
#define LIGNES 6
#define COLONNES 7

//Récompenses en fonction du vainqueur (voir sujet)
#define RECOMPENSE_ORDI_VAINQUEUR 1
#define RECOMPENSE_HUMAIN_VAINQUEUR 0
#define RECOMPENSE_MATCH_NUL 0.5
//Approximation de la valeur de √2
#define C 1,4142

// macros
#define AUTRE_JOUEUR(i) (1-(i))
#define min(a, b)       ((a) < (b) ? (a) : (b))
#define max(a, b)       ((a) < (b) ? (b) : (a))

// Critères de fin de partie
typedef enum {NON, MATCHNUL, ORDI_GAGNE, HUMAIN_GAGNE } FinDePartie;

// Valeurs possibles d'une case
typedef enum {PION_HUM, PION_ORDI, VIDE} Pion;

// Definition du type Etat (état/position du jeu)
typedef struct EtatSt {

    int joueur; // à qui de jouer ?

    // Un jeu de puissance 4 est composé de 7 colonnes de 6 cases de hauteurs
    Pion plateau[LIGNES][COLONNES];

} Etat;

// Definition du type Coup
typedef struct {
    // Un coup au puissance 4 n'est défini que par une colonne dans laquelle on veut glisser un pion
    int colonne;

} Coup;

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

    etat->joueur = 0;

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
// TODO: adapter la liste de paramètres au jeu
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


// Definition du type Noeud
typedef struct NoeudSt {

    int joueur; // joueur qui a joué pour arriver ici
    Coup * coup;   // coup joué par ce joueur pour arriver ici

    Etat * etat; // etat du jeu

    struct NoeudSt * parent;
    struct NoeudSt * enfants[LARGEUR_MAX]; // liste d'enfants : chaque enfant correspond à un coup possible
    int nb_enfants;	// nb d'enfants présents dans la liste

    // POUR MCTS:
    int nb_victoires;
    int nb_simus;
    int somme_recompense;
} Noeud;


// Créer un nouveau noeud en jouant un coup à partir d'un parent
// utiliser nouveauNoeud(NULL, NULL) pour créer la racine
Noeud * nouveauNoeud (Noeud * parent, Coup * coup ) {
    Noeud * noeud = (Noeud *)malloc(sizeof(Noeud));

    if ( parent != NULL && coup != NULL ) {
        noeud->etat = copieEtat ( parent->etat );
        jouerCoup ( noeud->etat, coup );
        noeud->coup = coup;
        noeud->joueur = AUTRE_JOUEUR(parent->joueur);
    }
    else {
        noeud->etat = NULL;
        noeud->coup = NULL;
        noeud->joueur = 0;
    }
    noeud->parent = parent;
    noeud->nb_enfants = 0;

    // POUR MCTS:
    noeud->nb_victoires = 0;
    noeud->nb_simus = 0;
    noeud->somme_recompense = 0;

    return noeud;
}

// Ajouter un enfant à un parent en jouant un coup
// retourne le pointeur sur l'enfant ajouté
Noeud * ajouterEnfant(Noeud * parent, Coup * coup) {
    Noeud * enfant = nouveauNoeud (parent, coup ) ;
    parent->enfants[parent->nb_enfants] = enfant;
    parent->nb_enfants++;
    return enfant;
}

void freeNoeud ( Noeud * noeud) {
    if ( noeud->etat != NULL)
        free (noeud->etat);

    while ( noeud->nb_enfants > 0 ) {
        freeNoeud(noeud->enfants[noeud->nb_enfants-1]);
        noeud->nb_enfants --;
    }
    if ( noeud->coup != NULL)
        free(noeud->coup);

    free(noeud);
}


// Test si l'état est un état terminal
// et retourne NON, MATCHNUL, ORDI_GAGNE ou HUMAIN_GAGNE
FinDePartie testFin( Etat * etat ) {
    // tester si un joueur a gagné
    int i,j,k,n = 0;
    for ( i=0;i < LIGNES ; i++) {
        for(j=0; j < COLONNES; j++) {
            if ( etat->plateau[i][j] != VIDE) {
                n++;	// nb coups joués

                // Vérifie sur la colonne
                k=0;
                while ( k < 4 && i+k < COLONNES && etat->plateau[i+k][j] == etat->plateau[i][j] )
					k++;
                if ( k == 4 )
					return etat->plateau[i][j] == 'O'? ORDI_GAGNE : HUMAIN_GAGNE;

                // Vérifie sur la ligne
                k=0;
                while ( k < 4 && j+k < LIGNES && etat->plateau[i][j+k] == etat->plateau[i][j] )
                    k++;
                if ( k == 4 )
                    return etat->plateau[i][j] == 'O'? ORDI_GAGNE : HUMAIN_GAGNE;

                // diagonales
                k=0;
                while ( k < 4 && i+k < COLONNES && j+k < LIGNES && etat->plateau[i+k][j+k] == etat->plateau[i][j] )
                    k++;
                if ( k == 4 )
                    return etat->plateau[i][j] == 'O'? ORDI_GAGNE : HUMAIN_GAGNE;

                k=0;
                while ( k < 4 && i+k < COLONNES && j-k >= 0 && etat->plateau[i+k][j-k] == etat->plateau[i][j] )
                    k++;
                if ( k == 4 )
                    return etat->plateau[i][j] == 'O'? ORDI_GAGNE : HUMAIN_GAGNE;
            }
        }
    }

    // et sinon tester le match nul
    if ( n == LIGNES*COLONNES )
        return MATCHNUL;

    return NON;
}

double calculerBValeur(Noeud * noeud){
    if(noeud->nb_simus == 0){
        // le noeud n'a aucune simulation, il est au max (éviter la division par 0)
        return DBL_MAX;
    }

    //calcul de la moyenne des récompenses
    double moyenneRecompense = (double)noeud->somme_recompense / noeud->nb_simus;

    // si le joueur du parent est 0 (humain), c'est un min ?
    if(noeud->parent->joueur == 0){
        moyenneRecompense *= -1;
	}

    return moyenneRecompense + C * sqrt(log(noeud->parent->nb_simus) / noeud->nb_simus);
}

Noeud * selectionnerNoeudUCT(Noeud * racine){

    //    noeud terminal      OU    tous les enfants ne sont pas dvlp
    if(testFin(racine->etat) != NON || racine->nb_enfants < nb_coups_possibles(racine->etat)){
        return racine;
    }

    int i = 0;
    Noeud * noeudBValeurMax = racine->enfants[i];
    double bValeurMax = calculerBValeur(noeudBValeurMax);
    for(int i=1; i<racine->nb_enfants; i++){
        double bValeurCompare = calculerBValeur(racine->enfants[i]);
        if(bValeurMax < bValeurCompare){
            noeudBValeurMax = racine->enfants[i];
			bValeurMax = bValeurCompare;
        }
    }

    return selectionnerNoeudUCT(noeudBValeurMax);
}

Noeud * developperFils(Noeud * noeud){
	// si on est sur un etat final, pas la peine de développer
	// TODO peut être le garder en mémoire dans la structure d'un noeud ?
	if(testFin(noeud->etat) != NON){
		return noeud;
	}

	// On récupère la liste des coups possibles
	Coup ** coups = coups_possibles(noeud->etat);
	Coup ** coupsNonDeveloppes = (Coup **) malloc((1+LARGEUR_MAX) * sizeof(Coup *) );

	// On filtre les coups déjàs développés
	//TODO n'utiliser qu'un seul tableau en décalant les indices, ou autre amélioration
	int k, indiceCoupsNonDeveloppes;
	k = 0;
	indiceCoupsNonDeveloppes = 0;
	while(coups != NULL){
		for(int i=1; i<noeud->nb_enfants; i++){
			if(!coups_egaux(coups[k], noeud->enfants[i]->coup)){
				coupsNonDeveloppes[indiceCoupsNonDeveloppes] = coups[k];
				indiceCoupsNonDeveloppes++;
			}
		}
		k++;
	}

	int choix = rand() % indiceCoupsNonDeveloppes;
	//liberer les listes de coups, mais conserver celui choisi
	k = 0;
    while (coups[k] != NULL && coupsNonDeveloppes[k] != NULL){
        if (k != choix){
            free(coups[k]);
			free(coupsNonDeveloppes[k]);
		}
        k++;
	}
	// On choisit et développe un fils au hasard dans ceux non développés
	Noeud * enfant = ajouterEnfant(noeud, coupsNonDeveloppes[choix]);

	return enfant;
}

FinDePartie simulerFinPartie(Etat * etat){
	FinDePartie resultat;
	resultat = testFin(etat);
	if(resultat != NON){
		return resultat;
	}
	Coup ** coups = coups_possibles(etat);
	jouerCoup(etat, coups[rand() % nb_coups_possibles(etat)]);

	free(coups);

	return simulerFinPartie(etat);

}

void mettreAJourBValeurs(Noeud * racine, FinDePartie resultat){
	while(racine != NULL){
		racine->nb_simus ++;

		if ( resultat == ORDI_GAGNE ){
			racine->nb_victoires ++;
			racine->somme_recompense += RECOMPENSE_ORDI_VAINQUEUR;
		}
		else if ( resultat == MATCHNUL )
			racine->somme_recompense += RECOMPENSE_MATCH_NUL;
		else if ( resultat == HUMAIN_GAGNE )
			racine->somme_recompense += RECOMPENSE_HUMAIN_VAINQUEUR;
		racine = racine->parent;
	}
    //TODO mettre à jour les B valeurs de tous les noeuds sur le chemin de la racine au noeud terminal, en remontant le résultat de la position finale
}


// Calcule et joue un coup de l'ordinateur avec MCTS-UCT
// en tempsmax secondes
void ordijoue_mcts(Etat * etat, int tempsmax) {

    clock_t tic, toc;
    tic = clock();
    int temps;

    Coup ** coups;
    Coup * meilleur_coup ;

    // Créer l'arbre de recherche
    Noeud * racine = nouveauNoeud(NULL, NULL);
    racine->etat = copieEtat(etat);

    // créer les premiers noeuds:
    coups = coups_possibles(racine->etat);
    int k = 0;
    Noeud * enfant;
    while ( coups[k] != NULL) {
        enfant = ajouterEnfant(racine, coups[k]);
        k++;
    }

    meilleur_coup = coups[ rand()%k ]; // choix aléatoire

    /*  TODO :
        - supprimer la sélection aléatoire du meilleur coup ci-dessus
        - implémenter l'algorithme MCTS-UCT pour déterminer le meilleur coup ci-dessous

        int iter = 0;

        do {



        // à compléter par l'algorithme MCTS-UCT...




        toc = clock();
        temps = (int)( ((double) (toc - tic)) / CLOCKS_PER_SEC );
        iter ++;
        } while ( temps < tempsmax );

        /* fin de l'algorithme  */

    // Jouer le meilleur premier coup
    jouerCoup(etat, meilleur_coup );

    // Penser à libérer la mémoire :
    freeNoeud(racine);
    free (coups);
}

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

                ordijoue_mcts( etat, TEMPS );

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
