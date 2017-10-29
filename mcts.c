#include "mcts.h"
#include "jeu.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <float.h>
#include <stdbool.h>

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
    if(testFin(racine->etat) != NON || racine->nb_enfants != nb_coups_possibles(racine->etat)){
        return racine;
    }

    int i = 0;
    Noeud * noeudBValeurMax = racine->enfants[i];
    double bValeurMax = calculerBValeur(noeudBValeurMax);
    for( i=1; i<racine->nb_enfants; i++){
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
	// On filtre les coups déjàs développés
	//TODO n'utiliser qu'un seul tableau en décalant les indices, ou autre amélioration
	/*
	  Après plusieurs méthodes testées (refaire un lancer au hasard si le choix est déjà developpé,
	  stocker les méthodes disponibles dans un tableau secondaire)
	  celle ci semble la plus efficace
	 */
	int coupsDispos, dejaDev;
	coupsDispos = 0;
	while(coups[coupsDispos] != NULL){
		dejaDev = 0;
		for(int i=0; i<noeud->nb_enfants; i++){
			if(coups_egaux(coups[coupsDispos], noeud->enfants[i]->coup)){
				dejaDev = 1;
				break;
			}
		}

		if(!dejaDev){
			coupsDispos++;
		}
		else {
			int k = coupsDispos;
			k = coupsDispos;
			while(coups[k] != NULL){
				coups[k] = coups[k+1];
				k++;
			}
		}
	}

	int choix = 0;
	if(coupsDispos > 0){
		choix = rand() % coupsDispos;
	}

	// On choisit et développe un fils au hasard dans ceux non développés
	Noeud * enfant = ajouterEnfant(noeud, coups[choix]);

	//liberer les listes de coups, mais conserver celui choisi
	coupsDispos = 0;
	while(coups[coupsDispos] != NULL){
		if(coupsDispos != choix){
			free(coups[coupsDispos]);
		}
		coupsDispos++;
	}

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
	if(racine != NULL){
		racine->nb_simus ++;

		switch(resultat){
		case ORDI_GAGNE:
			racine->nb_victoires ++;
			racine->somme_recompense += RECOMPENSE_ORDI_VAINQUEUR;
			break;
		case MATCHNUL:
			racine->somme_recompense += RECOMPENSE_MATCH_NUL;
			break;
		case HUMAIN_GAGNE:
			racine->somme_recompense += RECOMPENSE_HUMAIN_VAINQUEUR;
			break;
		default:
			break;
		}
		mettreAJourBValeurs(racine->parent, resultat);
	}
}

Noeud * choisirMeilleurCoup(Noeud * racine, Regle regle){
	int indiceMeilleurCoup = 0;
	double maxRecompense = 0;
	int maxSimulations = 0;
	switch(regle){
	case MAX:
		// on choisit le noeud qui maximise la récompense
		maxRecompense = racine->enfants[indiceMeilleurCoup]->nb_simus == 0 ?  0:(double)racine->enfants[indiceMeilleurCoup]->somme_recompense / racine->nb_simus;
		double tmpRecompense = 0;
		for(int i=1; i<racine->nb_enfants; i++){
			tmpRecompense = racine->enfants[i]->nb_simus == 0 ?  0:(double)racine->somme_recompense / racine->enfants[i]->nb_simus;
			if(tmpRecompense > maxRecompense){
				maxRecompense = tmpRecompense;
				indiceMeilleurCoup = i;
			}
		}
		break;
	case ROBUSTE:
		// on choisit le noeud qui maximise le nombre de simulations
		maxSimulations = racine->enfants[indiceMeilleurCoup]->nb_simus;
		int tmpSimulations = 0;
		for(int i=1; i<racine->nb_enfants; i++){
			tmpSimulations = racine->enfants[i]->nb_simus;
			if(tmpSimulations > maxSimulations){
				maxSimulations = tmpSimulations;
				indiceMeilleurCoup = i;
			}
		}

		break;
	default:
		break;
	}
	return racine->enfants[indiceMeilleurCoup];
}

// Calcule et joue un coup de l'ordinateur avec MCTS-UCT
// en tempsmax secondes
void ordijoue_mcts(Etat * etat, int tempsmax, Regle regle) {

    clock_t tic, toc;
    tic = clock();
    int temps;

    Coup ** coups;
    Coup * meilleur_coup ;
    Noeud * noeudMeilleurCoup = NULL;

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

	int iter = 0;
	do {
		// Selection du noeud avec la meilleure BValeur
		Noeud * noeudBValeurMax = selectionnerNoeudUCT(racine);

		// Developpement du noeud
		Noeud * enfantChoisi = developperFils(noeudBValeurMax);

		//Simuler la fin de partie
		// On fait une copie de l'etat, pour ne pas le modifier
		Etat * copie = copieEtat(enfantChoisi->etat);
		FinDePartie resultat = simulerFinPartie(copie);
		free(copie);

		// on remonte la récompense
		mettreAJourBValeurs(enfantChoisi, resultat);

		toc = clock();
		temps = (int)( ((double) (toc - tic)) / CLOCKS_PER_SEC );
		iter ++;
	} while ( temps < tempsmax );

	noeudMeilleurCoup = choisirMeilleurCoup(racine, regle);
	meilleur_coup = noeudMeilleurCoup->coup;
	/* fin de l'algorithme  */

	printf("\n");
	printf("Coup en colonne %d \n", meilleur_coup->colonne);
	printf("Simulations %d \n", racine->nb_simus);
	printf("Victoires : %d \n", noeudMeilleurCoup->nb_victoires);
	printf("Proba de victoire : ");
	if (noeudMeilleurCoup->nb_simus > 0)
		printf("%0.2f%%", (double)noeudMeilleurCoup->nb_victoires/noeudMeilleurCoup->nb_simus*100);
	else
		printf(" 0 %%");

	// Jouer le meilleur premier coup
	jouerCoup(etat, meilleur_coup );

	// Penser à libérer la mémoire :
	freeNoeud(racine);
	free (coups);
}
