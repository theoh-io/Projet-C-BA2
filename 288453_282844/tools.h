#ifndef TOOLS_H
#define TOOLS_H
#include <cmath>
#include "define.h"
#include <vector>

#define PRECISION				0.001 //tests d'egalite sur double
#define MJ					 	COEF_MARGE_JEU*(SIDE/nbCell)
#define ML					 	(COEF_MARGE_JEU/2)*(SIDE/nbCell)



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

struct Spot
{
	int ligne;
	int colonne;
	int numero;
};


Coord indice_en_coord(Carre c, int nbCell);

Carre spot_en_carre(Spot spot, int nbCell);

bool supp_cercles(Rond co1, Rond co2, int nbcell);

bool supp_cercles_en_jeu(Rond co1, Rond co2, int nbCell);

bool supp_cercle_carre(Rond co1, Carre ca1, int nbcell);

double dist_deux_points(Coord p1, Coord p2);

Coord chgmnt_coord(Coord pt_original);

double tps_angle(int compteur);

double norme(Coord v);

double calcul_angle(Coord pred, Coord cible);

double pente_chemin(Coord j1, Coord j2);

double calcul_b(Coord predateur, double pente);

double calcul_b_angle(Rond predateur, Rond proie, double pente);

double fct_math(double angle, double x, double b);

double fonct_maths_inv(double y, double pente, double b);

bool inter_cote_carre(double abscisse, double niveau_bas, double niveau_haut, double pente, 
				      double b1, double b2);

bool inter_haut_carre(double ordonnee, double niveau_gauche, double niveau_droit, double pente, 
				      double b1, double b2);

bool carre_dans_zone(Rond p1, Rond p2, Coord carre, int nbCell);

double fonct_maths(double x, double pente, double b);

double fonct_maths_inv(double y, double pente, double b);

bool sup_rect_obst(Coord obstacle, Rond j1, Rond j2, int nbCell);

int numero_spot(int ligne, int colonne, std::vector<Spot> tab_spots);


#endif
