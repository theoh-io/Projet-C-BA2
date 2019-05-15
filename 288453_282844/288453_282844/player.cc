#include "player.h"
#include "define.h"
#include "error.h"
#include <iostream>

//constructeur par défaut
Player::Player()
{
	pos_joueur.x=0;
	pos_joueur.y=0;
}

//constructeur initialisant la position en x et y
Player::Player(double abs, double ord)
{
	pos_joueur.x=abs;
	pos_joueur.y=ord;
}
// constructeur qui initialise position et aussi le rayon
Player::Player(double abs, double ord, int total1, int total2,
															int nbcell)
{
	pos_joueur.x=abs;
	pos_joueur.y=ord;
	nb_touches= total1;
	compteur= total2;
	pos_joueur.rayon=COEF_RAYON_JOUEUR*(SIDE/nbcell);
}


// accesseurs
int Player::getCompteur () const 
{ 
	return compteur;
}

Rond Player::getPos_joueur () const 
{ 
	return pos_joueur;
}

int Player::getNb_touches () const 
{ 
	return nb_touches;
}


// manipulateurs

void Player::setCompteur(double compt1)
{
	compteur = compt1;
}

void Player::setNb_touches(int touche1)
{
	nb_touches = touche1;
}

void Player::setPos_joueur(double x1, double y1)
{
	pos_joueur.x = x1;
	pos_joueur.y = y1;
}


