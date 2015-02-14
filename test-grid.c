//fichier test-grid.c
#include "grid.h"
#include <stdlib.h>
#include <stdio.h>

//fonction pour tester notre grille et son affichage
//i represente les colones et j les lignes

void afficher (grid g){
  printf (" +~~~~~~+~~~~~~~+~~~~~~~+~~~~~~~+ \n ");  
  for (int i=0; i<GRID_SIDE; ++i){
	for (int j=0; j<GRID_SIDE; ++j){
		tile res=(get_tile (g,i,j));
		if (res==0){
			printf ("|      |");
			}
		else{
	    		printf ("| %4d |",res);
			}
		}
	printf ("\n +~~~~~~+~~~~~~~+~~~~~~~+~~~~~~~+ \n ");
	}	
  unsigned long int score=(grid_score (g));
  printf ("votre score est de %d \n",score);
}

int main (void){
  grid g=new_grid ();
  afficher(g);  
  return EXIT_SUCCESS;
}
