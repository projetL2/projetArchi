//fichier grid.c
#include "grid.h"
#include <stdlib.h>
#include <assert.h>
#include <time.h>

// La structure de la grille, un pointeur de pointeur qui nous donnera la matrice
// et une unsigned long int score
struct grid_s {
    unsigned long int score;
    tile** grid;
};

//Le constructeur, on initialise le score à 0
//et on fait des malloc pour la struct et la grille
grid new_grid () {

    grid g = malloc(sizeof(struct grid_s));
    assert (g!=NULL);

    g->score=0;
    g->grid = malloc(GRID_SIDE * sizeof(tile*));
    assert (g->grid!=NULL);

    for (int i=0; i<GRID_SIDE; ++i) {
        g->grid[i] = malloc (GRID_SIDE * sizeof(tile));
        assert (g->grid[i]!=NULL);
    }

    return g;
}

//Destructeur de la grille, on libère d'abord les zones mémoire pointées par la variable grid puis la grille elle-même
void delete_grid (grid g) {

    for (int i=0; i<GRID_SIDE; ++i)
        free(g->grid[i]);
    free(g->grid);
    free(g);
}

//Constructeur par copie de grid
void copy_grid (grid src, grid dst) {

    dst->score = src->score;

    for (int i=0; i<GRID_SIDE; ++i)
        for (int j=0; j<GRID_SIDE; ++j)
            dst->grid[i][j] = src->grid[i][j];

}

//Accesseur qui retourne le score
unsigned long int grid_score (grid g) {

    return g->score;
}

//Accesseur qui renvoie une tile en fonction des coordonnées en paramètres
tile get_tile (grid g, int x, int y) {

    if ((x>=0 && x<=GRID_SIDE) && (y>=0 && y<=GRID_SIDE))
        return g->grid[x][y];
    else
        return -1; // solution temporaire pour avoir toujours un return
}

//Accesseur qui instancie la valeur d'une tile en fonction des coordonnées et de la valeur en paramètres
void set_tile (grid g, int x, int y, tile t) {

    g->grid[x][y] = t;
}

//Fonction booléene qui renvoie true si le joueur peut effectuer un movement dans une direction
bool can_move (grid g, dir d) {

    // on vérifie la direction demandée (d)
    // suivant cette dernière, on parcourt la grille pour voir si on peut déplacer ou fusionner,
    //si oui, on renvoie true
    switch (d) {

    case LEFT:
        for (int i=1; i<GRID_SIDE; ++i)  //i represente les colones
            for (int j=0; j<GRID_SIDE; ++j) //j represente les lignes
                if ( (g->grid[i-1][j] == 0) || (g->grid[i][j] == g->grid[i-1][j]) ) // si case suivante vide ou fusion possible
                    return true;
        break;

    case RIGHT:
        for (int i=0; i<GRID_SIDE-1; ++i)
            for (int j=0; j<GRID_SIDE; ++j)
                if ( (g->grid[i+1][j] == 0) || (g->grid[i][j] == g->grid[i+1][j]) )
                    return true;
        break;

    case UP:
        for (int i=0; i<GRID_SIDE; ++i)
            for (int j=1; j<GRID_SIDE; ++j)
                if ( (g->grid[i][j-1] == 0) || (g->grid[i][j-1] == g->grid[i][j]) )
                    return true;
        break;

    case DOWN:
        for (int i=0; i<GRID_SIDE; ++i)
            for (int j=0; j<GRID_SIDE-1; ++j)
                if ( (g->grid[i][j+1] == 0) || (g->grid[i][j+1] == g->grid[i][j]) )
                    return true;
        break;
    }
    // Si le mouvement demandé est impossible, on renvoie false
    return false;
}

//Fonction booléenne qui renvoie TRUE si aucun mouvement n'est plus possible.
bool game_over(grid g) {

    if ( !can_move(g,UP) &&
            !can_move(g,DOWN) &&
            !can_move(g,LEFT) &&
            !can_move(g,RIGHT))
        return true;

    else
        return false;
}

//Fonction qui déplace les tiles de la grid si le mouvement est possible
void do_move (grid g, dir d) {

    if (can_move(g,d)) {
        // dans tout le switch, on utilisera i pour parcourir les colonnes et j pour les lignes
        switch (d) {

        case LEFT:
            for (int ligne = 0; ligne<GRID_SIDE; ++ligne)	// on parcourt de gauche à droite
                for (int colonne = 1; colonne<GRID_SIDE; ++colonne) {   // et de haut en bas
                    if (g->grid[colonne][ligne] != 0) { // si la case n'est pas vide
                        if (g->grid[colonne-1][ligne] == 0) {  // si la case de gauche est vide, on décale la case actuelle
                            g->grid[colonne-1][ligne] = g->grid[colonne][ligne];
                            g->grid[colonne][ligne] = 0;
                            colonne-=2; // on décrémente i au cas où il y ait à déplacer la tile plusieurs fois
                        }
                        else if (g->grid[colonne][ligne] == g->grid[colonne-1][ligne]) { // s'il doit y avoir une fusion
                            g->grid[colonne][ligne] = 0;
                            g->grid[colonne-1][ligne] *= 2;				// on vide la case actuelle et on double la valeur de celle qu'elle a heurté
                            g->score += g->grid[colonne-1][ligne];		// on actualise le score
                        }
                    }
                }

            break;

        case RIGHT:
            for (int ligne=0; ligne<GRID_SIDE; ++ligne) {// on parcourt de droite à gauche
                for (int colonne = GRID_SIDE-2; colonne>=0; --colonne) { // et haut en bas
                    if (g->grid[colonne][ligne] != 0 && colonne < 3) { // si la case n'est pas vide
                        if (g->grid[colonne+1][ligne] == 0) {  // si la case de droite est vide, on décale la case actuelle
                            g->grid[colonne+1][ligne] = g->grid[colonne][ligne];
                            g->grid[colonne][ligne] = 0;
                            colonne+=2; // on incrémente i au cas où il y ait à déplacer la tile plusieurs fois
                        }
                        else if (g->grid[colonne][ligne] == g->grid[colonne+1][ligne]) { // s' il doit y avoir une fusion
                            g->grid[colonne][ligne] = 0;
                            g->grid[colonne+1][ligne] *= 2;			// on vide la case actuelle et on double la valeur de celle qu'elle a heurté
                            g->score += g->grid[colonne+1][ligne];		// on actualise le score
                        }
                    }
                }
            }
            break;

        case DOWN:
            for (int colonne=0; colonne<GRID_SIDE; ++colonne) // on parcourt de gauche à droite
                for (int ligne = GRID_SIDE-2; ligne>=0; --ligne) // de bas en haut
                    if (g->grid[colonne][ligne] != 0 && ligne < 3) {		// si la case n'est pas vide
                        if (g->grid[colonne][ligne+1] == 0) {		// si la case en dessous d'elle est vide, on décale
                            g->grid[colonne][ligne+1] = g->grid[colonne][ligne];
                            g->grid[colonne][ligne] = 0;
                            ligne+=2; // on incrémente les lignes au cas ou on doive déplacer plusieurs fois la tile
                        }
                        else if (g->grid[colonne][ligne+1] == g->grid[colonne][ligne]) {
                            g->grid[colonne][ligne] = 0;
                            g->grid[colonne][ligne+1] *= 2;
                            g->score += g->grid[colonne][ligne+1];
                        }
                    }
            break;
            /*
                  case DOWN:
                      for (int i=0; i<GRID_SIDE; ++i)
                          for (int j=0; j<GRID_SIDE-1; ++j)
                              if ( (g->grid[i][j+1] == 0) || (g->grid[i][j+1] == g->grid[i][j]) ) // si case suivante vide ou fusion possible
                                  return true;
                  break;
            	}*/
        }
    }
}

void add_tile (grid g) {

	int numberRand = rand()%10;
	
	int xrand = rand()%GRID_SIDE;
	int yrand = rand()%GRID_SIDE;

	while(g->grid[xrand][yrand]!=0) {
		xrand = rand()%GRID_SIDE;
		yrand = rand()%GRID_SIDE;
	}

	if(numberRand == 9)
		set_tile(g,xrand,yrand,4);
	else
		set_tile(g,xrand,yrand,2);
}

void play (grid g, dir d) {

    if(can_move(g,d)) {
        do_move(g,d);
        add_tile;
    }
}
