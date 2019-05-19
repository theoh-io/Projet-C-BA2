#ifndef PLAYER_H
#define PLAYER_H


#include <vector>
#include "tools.h"


class Player
{
	public:
		// constructeurs
		Player();
		Player(double abs, double ord);
		Player(double abs, double ord, int total1, int total2, int nbcell);
		Player(const Player& autre);
		virtual ~Player();
		// accesseurs
		double getAbcisse() const;
		double getOrdonnee() const;
		int getCompteur() const;
		Rond getPos_joueur() const;
		Coord getCoord() const;
		Coord getCible_coord() const;
	    int getNb_touches() const;
		Rond getCible() const;
		Rond getfutur_pos() const;
		Coord getfutur_pos_coord() const;
		
		

		// manipulateurs
		void setAbcisse(double);
		void setOrdonnee(double);
		void setCompteur(double);
		void setNb_touches(int);
		void setPos_joueur(double, double);
		void set_cible(Rond autre);
        void bouger_joueur(double,double);
		void valider_deplacement();
		void annuler_deplacement();
		bool upCompteur();
		void resetCompteur();
		void shot();
		//bool vivant();

	private:
		//declaration des attributs
		Rond cible;
		Rond pos_joueur;
		Rond futur_pos;
		int compteur;
		int nb_touches;

};

// fonction(s)





#endif
