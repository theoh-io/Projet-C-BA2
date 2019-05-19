#ifndef MAPS_H
#define MAPS_H

#include <vector>
#include "tools.h"


class Obstacle
{
	public:
		// constructeurs
		Obstacle(int,int);
		Obstacle();
		
		// accesseurs
	    Carre getcarre() const;
	    
		// manipulateurs
		void setcarre(int, int, double);
		//void destroy();
			
	private:
		//declaration des attributs
		Carre pos_obs;
		//bool mort;


};





#endif
