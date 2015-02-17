//fichier test-grid.c
#include "grid.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

//fonction pour tester notre grille et son affichage

void afficher (grid g) {

    printf (" +~~~~+~~~~+~~~~+~~~~+ \n ");

    for (int colonne=0; colonne<GRID_SIDE; ++colonne) {
        printf("|");
        for (int ligne=0; ligne<GRID_SIDE; ++ligne) {
            tile res = (get_tile (g,colonne,ligne));
            if (res == 0)
                printf ("    |"); // on affiche pas 0 pour plus de lisibilité.
            else
                printf ("%4d|",res); //on reserve la place de 4chiffres quel que soit le nombre réel de 						// la tile. 
        }
        printf ("\n +~~~~+~~~~+~~~~+~~~~+ \n ");
    }

    printf ("Votre score est de %lu points\n", grid_score(g));
}


int main (void) {

    grid g = new_grid (); //creation d'une nouvelle grille


    srand(time (NULL)); // srand est une fonction de la bibliothéque stdlib.h.
			// cette ligne permet d'initiliser la fonction time (de la bibliotheque time.h) 
    
    afficher(g);
    add_tile(g);
	afficher(g);
    for(int i = 0; i<10; i++) { // creation d'une boucle pour tester add_tile 
		 		
		int dirRand= rand()%3; // valariable pour choisir la direction de facon aléatoire
		switch (dirRand){
			case 0:
				do_move(g,LEFT);
			break;
			case 1:
				do_move (g,RIGHT);
			break;
			case 2:
				do_move (g,DOWN);
			break;
		}
		add_tile(g);
		afficher(g);
	}


    return EXIT_SUCCESS;
}
