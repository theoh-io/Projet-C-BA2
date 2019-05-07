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
		
		// accesseurs
		double getAbcisse() const;
		double getOrdonnee() const;
		int getCompteur() const;
		Rond getPos_joueur() const; 
	    int getNb_touches() const;
	    
		// manipulateurs
		void setAbcisse(double);
		void setOrdonnee(double);
		void setCompteur(double);
		void setNb_touches(int);
		void setPos_joueur(double, double);
		
	
	private:
		//declaration des attributs
		Rond pos_joueur;
		int compteur;
		int nb_touches;
		
};

// fonction(s) 



	

#endif
