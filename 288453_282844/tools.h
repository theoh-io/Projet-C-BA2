#ifndef TOOLS_H
#define TOOLS_H
#include <cmath>
#include "define.h"


struct Coord 
{
	double x;
	double y;
};

struct Carre
{
	int ligne;
	int colonne;
	double taille;
};

struct Rond
{
	double x;
	double y;
	double rayon;
};



Coord indice_en_coord(Carre c, int nbCell);

bool supp_cercles(Rond co1, Rond co2, int nbcell);

bool supp_cercle_carre(Rond co1, Carre ca1, int nbcell); 
///zesssss
double dist_deux_points(Coord p1, Coord p2);

Coord chgmnt_coord(Coord pt_original);

double tps_angle(int compteur);

double pente_chemin(Coord j1, Coord j2);

double calcul_b(Coord predateur, double pente);

double fct_math(double angle, double x, double b);

bool carre_dans_zone(Rond p1, Rond p2, Coord carre, int nbCell);

double fonct_maths(double x, double pente, double b);

bool sup_rect_obst(Coord obstacle, Rond j1, Rond j2, int nbCell);

#endif
