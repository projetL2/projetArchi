//fichier test-grid.c
#include "grid.h"
#include <stdlib.h>
#include <stdio.h>

//fonction pour tester notre grille et son affichage
//i représente les colonnes et j les lignes (commentaire à corriger)

void afficher (grid g) {
	
	printf (" +~~~~+~~~~+~~~~+~~~~+ \n ");  
	
	for (int i=0; i<GRID_SIDE; ++i) {
		printf("|");
		for (int j=0; j<GRID_SIDE; ++j) {
			tile res = (get_tile (g,j,i));
			if (res == 0)
				printf ("    |");
			else
				printf ("%4d|",res);
		}
		printf ("\n +~~~~+~~~~+~~~~+~~~~+ \n ");
	}
	
	printf ("Votre score est de %lu points\n", grid_score(g));	
}
  

int main (void) {
	
	grid g = new_grid ();
	
	for (int i=0; i<GRID_SIDE; ++i)
		for (int j=0; j<GRID_SIDE; ++j)
			set_tile(g,j,i,2);
			
	afficher(g);
	do_move(g, RIGHT);
	afficher(g);
	 
	return EXIT_SUCCESS;
}
