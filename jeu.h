#ifndef JEU_H
#define JEU_H

/*
  Canvas pour algorithmes de jeux à 2 joueurs

  joueur 0 : humain
  joueur 1 : ordinateur

*/

// Paramètres du jeu
#define LARGEUR_MAX 9 		// nb max de fils pour un noeud (= nb max de coups possibles)

//Dimensions standards d'un jeu de puissance 4
#define LIGNES 6
#define COLONNES 7

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
Etat * copieEtat( Etat * src );

// Etat initial
Etat * etat_initial( void );

void afficheJeu(Etat * etat);

// Nouveau coup
Coup * nouveauCoup( int i );

// Demander à l'humain quel coup jouer
Coup * demanderCoup ();

// Retourne la position (ligne) disponible sur la colonne
// Retourne -1 si aucune position disponible
int hautColonne(Etat * etat, int i);

// Modifier l'état en jouant un coup
// retourne 0 si le coup n'est pas possible
int jouerCoup( Etat * etat, Coup * coup );

// Retourne une liste de coups possibles à partir d'un etat
// (tableau de pointeurs de coups se terminant par NULL)
Coup ** coups_possibles( Etat * etat );

// Retourne la taille de la liste de coups possibles à partir d'un etat
int nb_coups_possibles( Etat * etat );

// Compare deux coups. (vérification basée sur le numéro de colonne, cf règles du puissance 4)
// Retour :
// 1 si identiques
// 0 si différents
int coups_egaux(Coup * coup1, Coup * coup2);

// Test si l'état est un état terminal
// et retourne NON, MATCHNUL, ORDI_GAGNE ou HUMAIN_GAGNE
FinDePartie testFin( Etat * etat );

#endif
