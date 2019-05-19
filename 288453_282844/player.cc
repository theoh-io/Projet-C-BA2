#include "player.h"
#include "define.h"
#include "error.h"
#include <iostream>

//constructeur par défaut
Player::Player()
{
	pos_joueur.x=0;
	pos_joueur.y=0;
	cible.x=0;
	cible.y=0;
	futur_pos.x=0;
	futur_pos.y=0;

}

//constructeur initialisant la position en x et y
Player::Player(double abs, double ord)	
{
	pos_joueur.x=abs;
	pos_joueur.y=ord;
	cible.x=0;
	cible.y=0;
	futur_pos.x=0;
	futur_pos.y=0;

}

Player::Player(double abs, double ord, int total1, int total2,int nbcell)
{
	pos_joueur.x=abs;
	pos_joueur.y=ord;
	nb_touches= total1;
	compteur= total2;
	pos_joueur.rayon=COEF_RAYON_JOUEUR*(SIDE/nbcell);
	futur_pos.rayon=COEF_RAYON_JOUEUR*(SIDE/nbcell);
	cible.x=0;
	cible.y=0;
	futur_pos.x=0;
	futur_pos.y=0;
}


//Destructeur
Player::~Player()
{
}

Player::Player(const Player& autre)
{
	pos_joueur.x=autre.getPos_joueur().x;
	pos_joueur.y=autre.getPos_joueur().y;
	nb_touches= autre.getNb_touches();
	compteur= autre.getCompteur();
	pos_joueur.rayon=autre.getPos_joueur().rayon;
	futur_pos.rayon=autre.getfutur_pos().rayon;
	cible.x=autre.getCible().x;
	cible.y=autre.getCible().y;
	futur_pos.x=autre.getfutur_pos().x;
	futur_pos.y=autre.getfutur_pos().y;
	
}
;



// accesseurs
double Player::getAbcisse() const
{
	return pos_joueur.x;
}
		
double Player::getOrdonnee() const
{
	return pos_joueur.y;
}

int Player::getCompteur () const
{
	return compteur;
}

Rond Player::getPos_joueur () const
{
	return pos_joueur;
}

Coord Player::getCoord() const
{
	Coord position;
	position.x=pos_joueur.x;
	position.y=pos_joueur.y;
	return position;
}

int Player::getNb_touches () const
{
	return nb_touches;
}

Rond Player::getCible() const
{
  return cible;
}

Coord Player::getCible_coord() const
{
  Coord c;
  c.x=cible.x;
  c.y=cible.y;
  return c;
}

Coord Player::getfutur_pos_coord() const
{
  Coord futur_pos_coord;
  futur_pos_coord.x=futur_pos.x;
  futur_pos_coord.y=futur_pos.y;
  return futur_pos_coord;
}


Rond Player::getfutur_pos() const
{
  return futur_pos;
}

// manipulateurs


void Player::set_cible(Rond autre)
{
	cible.x= autre.x;
	cible.y= autre.y;
	cible.rayon= autre.rayon;
}

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


void  Player::bouger_joueur(double x1,double y1)
{
  futur_pos.x=pos_joueur.x + x1;
  futur_pos.y=pos_joueur.y + y1;
  
}

void Player::valider_deplacement()
{
  pos_joueur.x=futur_pos.x;
  pos_joueur.y=futur_pos.y;
}

void Player::annuler_deplacement()
{
  futur_pos.x=pos_joueur.x;
  futur_pos.y=pos_joueur.y;
}

bool Player::upCompteur()
{
	if(compteur==MAX_COUNT) 
	{
		return true;
	}
	++compteur;
	return false;
}

void Player::resetCompteur()
{
	compteur=0;
}

void Player::shot()
{
	if(nb_touches>0) nb_touches=nb_touches-1;
}
