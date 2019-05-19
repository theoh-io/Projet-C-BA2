#include "tools.h"
#include <iostream>
#include <cmath>


#define ML					 	(COEF_MARGE_JEU/2)*(SIDE/nbCell)
#define MJ					 	COEF_MARGE_JEU*(SIDE/nbCell)
#define MOIT_HAUT 				SIDE/(2*nbCell)
#define TOUR_RAD				2*M_PI
#define TROIS_DEMI_PI			3*M_PI/2
#define PRECISION				0.001 //tests d'egalite sur double

enum EtatsSupp {JJ, BB, JB};


Coord indice_en_coord(Carre c, int nbCell)
{
	Coord c1;
	c1.x = -DIM_MAX + c.colonne*(SIDE/nbCell) + MOIT_HAUT;
	c1.y = DIM_MAX - c.ligne*(SIDE/nbCell) - MOIT_HAUT;
	if(fabs(c1.x)<PRECISION)
	{
		c1.x=0;
	}
	if(fabs(c1.y)<PRECISION)
	{	
		c1.y=0;
	}
	return c1;
}

Carre spot_en_carre(Spot spot, int nbCell)
{
	Carre carre;
	carre.ligne=spot.ligne;
	carre.colonne=spot.colonne;
	carre.taille=2*MOIT_HAUT;
	return carre;
}

bool supp_cercles_en_jeu(Rond co1, Rond co2, int nbCell)
{
	Coord r1, r2;
	r1.x = co1.x;
	r1.y = co1.y;
	r2.x = co2.x;
	r2.y = co2.y;
	//r1.rayon=COEF_RAYON_JOUEUR*(SIDE/nbCell);
	if(dist_deux_points(r1,r2)<(co1.rayon+co2.rayon+MJ)) return true;
	else return false;
}

bool supp_cercles(Rond co1, Rond co2, int nbCell)
{
	Coord r1, r2;
	r1.x = co1.x;
	r1.y = co1.y;
	r2.x = co2.x;
	r2.y = co2.y;
	if(dist_deux_points(r1,r2)<(co1.rayon+co2.rayon+ ML)) return true;
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

double norme(Coord v)
{
	double norme=sqrt(v.x*v.x+v.y*v.y);
	return norme;
}

double calcul_angle(Coord pred, Coord cible)
{
	double tan_a(0);
	tan_a=(cible.y-pred.y)/(cible.x-pred.x);
	if(std::abs(pred.y-cible.y)<PRECISION) 
	{
		if(pred.x>=cible.x) return M_PI;
		else return 0;
	}
	if(tan_a>=0)
	{
		if(cible.y>=pred.y) return atan(tan_a);
		else return M_PI+atan(tan_a);
	}
	else
	{
	  if(cible.y>=pred.y) return M_PI+atan(tan_a);
	  else return atan(tan_a);
    }
	
}

double pente_chemin(Coord j1, Coord j2)
{
	return ((j1.y-j2.y)/(j1.x-j2.x));
}

double calcul_b(Coord predateur, double pente)
{
	return predateur.y - pente*predateur.x;
}

double calcul_b_angle(Rond predateur, Rond proie, double pente)
{
	Coord pred,cible;
	pred.x=predateur.x;
	pred.y=predateur.y;
	cible.x=proie.x;
	cible.y=proie.y;
	std::cout<<"angle"<<calcul_angle(pred,cible)<<std::endl;
	std::cout<<"b1 neg"<<pente*(pred.x+cos(calcul_angle(pred,cible))*predateur.rayon
					-(pred.y-sin(calcul_angle(pred,cible))*predateur.rayon))<<std::endl;
	std::cout<<"b1 pos"<< pente*(pred.x-cos(calcul_angle(pred,cible))*predateur.rayon
					-(pred.y+sin(calcul_angle(pred,cible))*predateur.rayon))<<std::endl;
	

	if((calcul_angle(pred,cible)>M_PI)||(calcul_angle(pred,cible)<0))
		return pente*(pred.x+cos(calcul_angle(pred,cible))*predateur.rayon
					-(pred.y-sin(calcul_angle(pred,cible))*predateur.rayon));
	if((calcul_angle(pred,cible)<=M_PI)||(calcul_angle(pred,cible)>=0))
		return pente*(pred.x-cos(calcul_angle(pred,cible))*predateur.rayon
					-(pred.y+sin(calcul_angle(pred,cible))*predateur.rayon));

}

bool carre_dans_zone(Rond p1, Rond p2, Coord carre, int nbCell)					//faire un passage par référence pour faire une bonne copie
{
	if(p1.y<p2.y)
	{
		Rond p_bis;
		p_bis.x=p1.x;
		p_bis.y=p1.y;
		p1.x=p2.x;
		p1.y=p2.y;
		p2.x=p_bis.x;
		p2.y=p_bis.y;
	}
	if(p1.x<p2.x)
		if(((carre.x>p1.x-ML-MOIT_HAUT-p1.rayon)&&(carre.x<p2.x+ML+MOIT_HAUT+p1.rayon)) &&
		   ((carre.y<=p1.y+ML+MOIT_HAUT+p1.rayon)&&(carre.y>=p2.y-ML-MOIT_HAUT-p1.rayon)))
			return true;
	if(p1.x>p2.x)
		if(((carre.x<p1.x+ML+MOIT_HAUT+p1.rayon)&&(carre.x>p2.x-ML-MOIT_HAUT-p1.rayon)) &&
		   ((carre.y<=p1.y+ML+MOIT_HAUT+p1.rayon)&&(carre.y>=p2.y-ML-MOIT_HAUT-p1.rayon)))
			return true;
	if(p1.x==p2.x)
		if((carre.y<p1.y-ML-MOIT_HAUT-p1.rayon)&&(carre.y>p2.y+ML+MOIT_HAUT+p1.rayon)&&
			(fabs(carre.x-p1.x)<=MJ+MOIT_HAUT+p1.rayon))
			return true;
	return false;
}

double fonct_maths(double x, double pente, double b)
{
	return x*pente + b;
}

double fonct_maths_inv(double y, double pente, double b)
{
	return (y-b)/pente;
}
/////test si l'image de l'abscisse est entre le niveau bas et le niveau haut
bool inter_cote_carre(double abscisse, double niveau_bas, double niveau_haut, double pente, 
				      double b1, double b2, int nbCell)  
{
	if ((fonct_maths(abscisse,pente,b1)>=niveau_bas-MJ)&&(fonct_maths(abscisse,pente,b1)<=niveau_haut+MJ))
{
		return true;
}
	if ((fonct_maths(abscisse,pente,b2)>=niveau_bas-MJ)&&(fonct_maths(abscisse,pente,b2)<=niveau_haut+MJ))
{
		return true;
}
	return false;
}
////test si l'antecedent de l'ordonnee est entre le niveau gauche et le niveau droit
bool inter_haut_carre(double ordonnee, double niveau_gauche, double niveau_droit, double pente, 
				      double b1, double b2, int nbCell) 
{
	if ((fonct_maths_inv(ordonnee,pente,b1)>=niveau_gauche-MJ)&&(fonct_maths_inv(ordonnee,pente,b1)<=niveau_droit+MJ))
{
		return true;
}
	if ((fonct_maths_inv(ordonnee,pente,b2)>=niveau_gauche-MJ)&&(fonct_maths_inv(ordonnee,pente,b2)<=niveau_droit+MJ))
{
		return true;
}
	return false;
}

bool sup_rect_obst(Coord obstacle, Rond j1, Rond j2, int nbCell)
{
	if (fabs(obstacle.x-j1.x)<=MJ+MOIT_HAUT+j1.rayon)
	{
		if(j1.y<j2.y)
			if((obstacle.y>j1.y)&&(obstacle.y<j2.y))
				return true;
		if(j1.y>j2.y)
			if((obstacle.y<j1.y)&&(obstacle.y>j2.y))
				return true;
	}
	return false;
}

int numero_spot(int ligne, int colonne, std::vector<Spot> tab_spots)
{
	for (size_t k = 0; k < tab_spots.size(); ++k)
	{
		if((tab_spots[k].ligne==ligne)&&(tab_spots[k].colonne==colonne))
			return tab_spots[k].numero;
	}
	return 0;
} 