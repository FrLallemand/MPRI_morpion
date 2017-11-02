#ifndef MCTS_H
#define MCTS_H

#include "jeu.h"

//Récompenses en fonction du vainqueur (voir sujet)
#define RECOMPENSE_ORDI_VAINQUEUR 1
#define RECOMPENSE_HUMAIN_VAINQUEUR 0
#define RECOMPENSE_MATCH_NUL 0.5
//Approximation de la valeur de √2
#define C sqrt(2)

#define TEMPS 8 	// temps de calcul pour un coup avec MCTS (en secondes)




// Valeurs possibles d'une case
typedef enum {MAX, ROBUSTE, MAX_ROBUSTE} Regle;

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
    double somme_recompense;
} Noeud;

// Créer un nouveau noeud en jouant un coup à partir d'un parent
// utiliser nouveauNoeud(NULL, NULL) pour créer la racine
Noeud * nouveauNoeud (Noeud * parent, Coup * coup );

// Ajouter un enfant à un parent en jouant un coup
// retourne le pointeur sur l'enfant ajouté
Noeud * ajouterEnfant(Noeud * parent, Coup * coup);

void freeNoeud ( Noeud * noeud);

double calculerBValeur(Noeud * noeud);

Noeud * selectionnerNoeudUCT(Noeud * racine);

Noeud * developperFils(Noeud * noeud);

FinDePartie simulerFinPartie(Etat * etat);

void mettreAJourBValeurs(Noeud * racine, FinDePartie resultat);

Noeud * choisirMeilleurCoup(Noeud * racine, Regle regle);

// Calcule et joue un coup de l'ordinateur avec MCTS-UCT
// en tempsmax secondes
void ordijoue_mcts(Etat * etat, int tempsmax, Regle regle);

#endif
