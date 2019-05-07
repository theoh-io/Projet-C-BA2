#include <iostream>
#include "maps.h"
#include "error.h"


//constructeur par défaut
Obstacle::Obstacle(){
	pos_obs.ligne=0;
	pos_obs.colonne=0;
}

//constructeur initialisant ligne et colonne
Obstacle::Obstacle(int i, int j){
	pos_obs.ligne=i;
	pos_obs.colonne=j;
}

// Accesseurs
Carre Obstacle::getcarre() const{
	return pos_obs;
}

// Manipulateurs
void Obstacle::setcarre(int i, int j, double dim)
{
	pos_obs.ligne = i;
	pos_obs.colonne = j;
	pos_obs.taille = dim;
}


