#include "tools.h"


#define ML					 	COEF_MARGE_JEU*(SIDE/nbCell)
#define MOIT_HAUT 				SIDE/(2*nbCell)
#define TOUR_RAD				2*M_PI
#define TROIS_DEMI_PI			3*M_PI/2

enum EtatsSupp {JJ, BB, JB};


Coord indice_en_coord(Carre c, int nbCell)
{
	Coord c1;
	c1.x = -DIM_MAX + c.colonne*(SIDE/nbCell) + MOIT_HAUT; 
	c1.y = DIM_MAX - c.ligne*(SIDE/nbCell) - MOIT_HAUT;
	return c1;
}

bool supp_cercles(Rond co1, Rond co2, int nbCell)
{
	Coord r1, r2;
	r1.x = co1.x;
	r1.y = co1.y;
	r2.x = co2.x;
	r2.y = co2.y;
	if(dist_deux_points(r1,r2)<(co1.rayon+co2.rayon) + ML) return true;									
	else return false;
}

bool supp_cercle_carre(Rond ro, Carre ca, int nbCell)
{
	Coord c1 = indice_en_coord(ca,nbCell);
	Coord p1, p2, p3, p4;   // points des 4 coins de l'obstacle
	p1.x = c1.x + MOIT_HAUT;
	p1.y = c1.y + MOIT_HAUT;
	p2.x = c1.x + MOIT_HAUT;
	p2.y = c1.y - MOIT_HAUT;
	p3.x = c1.x - MOIT_HAUT;
	p3.y = c1.y + MOIT_HAUT;
	p4.x = c1.x - MOIT_HAUT;
	p4.y = c1.y - MOIT_HAUT;
	Coord r1;
	r1.x = ro.x ; r1.y = ro.y;
	
	
	if (fabs(c1.x-ro.x)<=MOIT_HAUT || fabs(c1.y-ro.y)<=MOIT_HAUT)
	{
		if ((MOIT_HAUT + ro.rayon + ML >= fabs(c1.x-ro.x)) &&
			(MOIT_HAUT + ro.rayon + ML >= fabs(c1.y-ro.y))) return true;
	}
	if ((dist_deux_points(p1,r1) <= ro.rayon+ML) ||
		(dist_deux_points(p2,r1) <= ro.rayon+ML) ||	
		(dist_deux_points(p3,r1) <= ro.rayon+ML) ||
		(dist_deux_points(p4,r1) <= ro.rayon+ML))  return true ;
	else return false;
} 

double dist_deux_points(Coord p1, Coord p2)
{
	return sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y));
}

Coord chgmnt_coord(Coord pt_original)
{
	Coord pt_nouv;
	pt_nouv.x = pt_original.x + DIM_MAX;
	pt_nouv.y = -pt_original.y + DIM_MAX;
	return pt_nouv;
}

double tps_angle(int compteur)
{
	double angle;
	return angle = ((compteur*TOUR_RAD)/MAX_COUNT)+TROIS_DEMI_PI;
}

