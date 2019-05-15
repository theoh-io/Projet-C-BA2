#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include "define.h"
#include "error.h"
#include "maps.h"
#include "player.h"
#include "ball.h"
#include "tools.h"
#include "simulation.h"
using namespace std;

#define NB_MIN_PLAYER				1
#define ML					COEF_MARGE_JEU*(SIDE/nbCell)
#define MOIT_HAUT 				SIDE/(2*nbCell)

static int nbCell=0;
static vector<Obstacle> tab_obst;
static vector<Player> tab_player;
static vector <Ball> tab_ball;

//états de l'automate de lecture
enum Etat_lecture {NB_CELL,NB_PLAYER,PLAYER,NB_OBSTACLE,OBSTACLE,
				   NB_BALL,BALL,FIN};


//prototypes des fonctions confidentielles au module
void decodage_ligne(string line,bool mode,int& etat); //mode=true dans mode ERROR
//verifie la position pour joueur ou balle
bool falseposition(double x, double y);
//verifie la position pour obstacle
bool falseposition(int colonne,int ligne,int& k);
void add_player(double abs, double ord, int touch, int compt);
void add_ball(double abs, double ord, double a);
void add_obs(int total1,int total2);
void err_jb(bool mode);
void err_jo(bool mode);
void err_bo(bool mode);
void err_bb(bool mode);
void err_jj(bool mode);
void err_oo(bool mode);
void check_error(bool mode);
void analyse();
void choix_cible();
void futur_deplacement(Player &p, Coord direction);
void direction_deplacement();
void validation_deplacement();
void test_cible();
void lancer_balles();
void creer_balle(Player& p);
void deplacement_balles();

void mise_a_jour()
{
  analyse();
  lancer_balles();
  deplacement_balles();
	//detection collisions balles
	//suppression des éléments
}

void analyse()
{
  choix_cible();
  direction_deplacement();
  validation_deplacement();
}

void choix_cible()
{
	//O(nbplayer²)
	double dist;
	Coord r1, r2;
	Rond cible;
	for(size_t i(0);i<tab_player.size();++i)
	{
		dist=SIDE*SIDE;  //represente l'infini
		r1.x=tab_player[i].getPos_joueur().x;
		r1.y=tab_player[i].getPos_joueur().y;
  	  for(size_t j(0);j<tab_player.size();++j)
	  {
		  r2.x = tab_player[j].getPos_joueur().x;
		  r2.y = tab_player[j].getPos_joueur().y;
		  if((dist_deux_points(r1,r2)>tab_player[j].getPos_joueur().rayon)
										&&(dist_deux_points(r1,r2)<dist))
			{
				dist=dist_deux_points(r1,r2);
				cible=tab_player[j].getPos_joueur();
			}
	  }
		tab_player[i].set_cible(cible);
	}
}

void test_cible()
{
	for(size_t j(0);j<tab_player.size();++j)
	{
	  cout<<"position p "<<tab_player[j].getCoord().x<<"  "<<tab_player[j].getCoord().y<<endl;
	  cout<<"position c:"<<tab_player[j].getCible().x<<"  "<<tab_player[j].getCible().y<<endl;
	  cout<<"future pos vaut"<<tab_player[j].getfutur_pos().x<<"  "<<tab_player[j].getfutur_pos().y<<endl;
	}
}

void direction_deplacement()
{
  for(size_t j(0);j<tab_player.size();++j)
  {
    if(true)//test_ligne_droite)
    {
      //deplacer tout droit vers cible
		  Coord direction;
		  Coord direction_normee;
		  double n;
		  direction.x=(tab_player[j].getCible().x-tab_player[j].getAbcisse());
		  direction.y=(tab_player[j].getCible().y-tab_player[j].getOrdonnee());
		  n=norme(direction);
		  direction_normee.x=direction.x/n;
		  direction_normee.y=direction.y/n;
		  futur_deplacement(tab_player[j],direction_normee);
    }
   // else
		//{
      //deplacer dans la direction donnée par algo de Floyd
	   // Coord direction; //direction=Algo Floyd
	   // futur_deplacement(tab_player[j],direction);
    //}
  }
}

void futur_deplacement(Player &p, Coord direction)
{
	double distance;
	distance = COEF_VITESSE_JOUEUR*(SIDE/nbCell)* DELTA_T;
	p.bouger_joueur(distance*direction.x,distance*direction.y);
}

void validation_deplacement()
{

  for(size_t i(0);i<tab_player.size();++i)
  {
	//traitement contact joueur-joueur(voir 3.2.2 modifier la fonction supperposition marge MJ+prediction déplacement ?)
    if(!supp_cercles_en_jeu(tab_player[i].getfutur_pos(),tab_player[i].getCible(),nbCell)) //reimplementer les poineurs vers cible pour pouvoir tester avec la future position de la cible
      tab_player[i].valider_deplacement();
  }
}

void lancer_balles()
{
  for(size_t i(0);i<tab_player.size();++i)
	{
	  if(true)//test_ligne_droite())
	  {
		if(tab_player[i].upCompteur()) creer_balle(tab_player[i]);
	  }
  }
}

void creer_balle(Player& p)
{
  p.resetCompteur();
  double angle= calcul_angle(p.getCoord(),p.getCible_coord());
  double abs=p.getAbcisse();
  double ord=p.getOrdonnee();
  cout<<"l'angle est "<<angle<<endl;
  Ball b(abs,ord, angle, nbCell);
  double distance=p.getPos_joueur().rayon+COEF_MARGE_JEU*(SIDE/nbCell)+b.getPos_ball().rayon;
  abs=distance*cos(b.getAngle_b());
  ord=distance*sin(b.getAngle_b());
  b.deplacer_ball(abs,ord);
  tab_ball.push_back(b);
}


void deplacement_balles()
{
  for(size_t i(0);i<tab_ball.size();++i)
	{
	  double distance=COEF_VITESSE_BALLE*(SIDE/nbCell)*DELTA_T;
	  double abcisse= distance*cos(tab_ball[i].getAngle_b());
	  double ordonnee=distance*sin(tab_ball[i].getAngle_b());
	  tab_ball[i].deplacer_ball(abcisse,ordonnee);
    }
}

//fonction main fournit le pointeur vers le fichier (argv[1])
void lecture(char * nom_fichier)
{
	static int etat=NB_CELL;
    string line;
    ifstream fichier(nom_fichier);
    if(!fichier.fail())
    {
        while(getline(fichier >> ws,line))
        {
			if(etat==FIN) break;
			if(line[0]=='#')  continue;
			decodage_ligne(line,true,etat);
        }
		check_error(true);
        cout << FILE_READING_SUCCESS << endl;
	}
}

//décodage selon l'etat courant d'une ligne lue dans le fichier
void decodage_ligne(string line,bool mode,int& etat)
{
	istringstream data(line);
	//static int etat(NB_CELL); //état initial
	static int i(0), total(0);
	double x(0),y(0),angle(0);
	int buffer_int1(0),buffer_int2(0),k(0);
	switch(etat)
	{
		case NB_CELL:
			(data >> total);
			if((total<MIN_CELL)or(total>MAX_CELL))
			{
				if(mode==true) exit(0);
				else
				{
					cout<<"empty file opened"<<endl;
					erasing_data(etat);
					break;
				}
			}
			etat=NB_PLAYER;
			nbCell=total;
			break;
		case NB_PLAYER:
			(data >> total);
			i=0 ;
			if(total==0) etat=NB_OBSTACLE;
			else etat=PLAYER;
			break;
		case PLAYER:
			(data>>x>>y>>buffer_int1>>buffer_int2);
		//utilise buffer_int1&2 pour stocker le nombre de touche et compteur
			++i;
			add_player(x,y,buffer_int1,buffer_int2);
			if(falseposition(x,y))
			{
				cout<<PLAYER_OUT(i)<<endl;
				if(mode==true) exit(0);
				else
				{
					erasing_data(etat);
					break;
				}
			}
			if(i == total) etat=NB_OBSTACLE ;
			break;
		case NB_OBSTACLE:
			(data >> total);
			i=0 ;
			if(total==0) etat=NB_BALL;
			else etat=OBSTACLE ;
			break;
		case OBSTACLE:
		//utilisation de buffer_int1&2 pour stocker ligne et col d'un obstacle
			(data >>buffer_int1>>buffer_int2);
			++i;
			add_obs(buffer_int1,buffer_int2);
			//test de validité sur la position de l'obstacle
			if(falseposition(buffer_int1,buffer_int2,k))
			{
				cout<<OBSTACLE_VALUE_INCORRECT(k)<<endl;
				if(mode==true) exit(0);
				else
				{
					erasing_data(etat);
					break;
				}
			}
			if(i == total) etat=NB_BALL ;
			break;
		case NB_BALL:
			(data >> total);
			i=0;
			if(total==0) etat=FIN; else etat=BALL ;
			break;
		case BALL:
			(data>>x>>y>>angle);
			++i;
			add_ball(x,y,angle);
			if(falseposition(x,y))
			{
				cout<<BALL_OUT(i)<<endl;
				if(mode==true) exit(0);
				else erasing_data(etat);
			}
			if(i == total) etat=FIN ;
			break;
		case FIN:
			cout<<"la fin a été atteinte"<<endl;
			break;
	}
}

bool falseposition(double x, double y)
{
	if((x>DIM_MAX)||(x<-DIM_MAX)||(y<-DIM_MAX)||(y>DIM_MAX)) return true;
	else return false;
}

bool falseposition(int colonne,int ligne,int& k)
{
	k=0;
	if((colonne<0)||(colonne>(nbCell-1)))
	{
		k=colonne;
		return true;
	}
	if((ligne<0)||(ligne>(nbCell-1)))
	{
		k=ligne;
		return true;
	}
	return false;
}

void add_player(double abs, double ord, int touch, int compt)
{
	Player p(abs,ord,touch,compt,nbCell);
	tab_player.push_back(p);
}

void add_ball(double abs, double ord, double a)
{
	Ball b(abs, ord, a, nbCell);
	tab_ball.push_back(b);
}

void add_obs(int total1,int total2)
{
	Obstacle ob(total1,total2);
	tab_obst.push_back(ob);
}



void check_error(bool mode)
{
	err_oo(mode);
	err_jb(mode);
	err_jo(mode);
	err_bo(mode);
	err_jj(mode);
	err_bb(mode);
}

void err_jj(bool mode)
{
	for (size_t i(0); i < tab_player.size() ; ++i)
	{
		for (size_t j(tab_player.size()-1); j > i ; --j)
		{
			if (supp_cercles( tab_player[i].getPos_joueur(),
											tab_player[j].getPos_joueur(), nbCell))
			{
				cout<<PLAYER_COLLISION(i+1, j+1)<<endl;
				if(mode==true) exit(0);
				else erasing_data();
			}
		}
	}
}

void err_bb(bool mode)
{
	for (size_t i(0); i < tab_ball.size() ; ++i)
	{
		for (size_t j(tab_ball.size()-1); j > i ; --j)
		{
			if (supp_cercles(tab_ball[i].getPos_ball(), tab_ball[j].getPos_ball(),nbCell))
			{
				cout<<BALL_COLLISION(i+1, j+1)<<endl;
				if(mode==true) exit(0);
				else erasing_data();
			}
		}
	}
};

void err_oo(bool mode)
{
	for (size_t i(0); i < tab_obst.size() ; ++i)
	{
		for (size_t j(tab_obst.size()-1); j > i ; --j)
		{
			if (tab_obst[i].getcarre().ligne == tab_obst[j].getcarre().ligne &&
				tab_obst[i].getcarre().colonne == tab_obst[j].getcarre().colonne)
			{
				cout<<MULTI_OBSTACLE(tab_obst[i].getcarre().ligne,
							 tab_obst[i].getcarre().colonne)<<endl;
				if(mode==true) exit(0);
				else erasing_data();
			}
		}
	}
}

void err_jb(bool mode)
{
	for (size_t i(0); i < tab_player.size() ; ++i)
	{
		for (size_t m(0); m < tab_ball.size() ; ++m)
		{
			if (supp_cercles(tab_player[i].getPos_joueur(), tab_ball[m].getPos_ball(),
				nbCell))
			{
				cout<<PLAYER_BALL_COLLISION(i+1, m+1)<<endl;
				if(mode==true) exit(0);
				else erasing_data();
			}
		}
	}
}

void err_jo(bool mode)
{
	for (size_t i(0); i < tab_player.size() ; ++i)
	{
		for (size_t m(0); m < tab_obst.size() ; ++m)
		{
			if (supp_cercle_carre(tab_player[i].getPos_joueur(), tab_obst[m].getcarre(),
				nbCell))
			{
				cout<<COLL_OBST_PLAYER(m+1, i+1)<<endl;
				if(mode==true) exit(0);
				else erasing_data();
			}
		}
	}
}

void err_bo(bool mode)
{
	for (size_t i(0); i < tab_ball.size() ; ++i)
	{
		for (size_t m(0); m < tab_obst.size() ; ++m)
		{
			if (supp_cercle_carre(tab_ball[i].getPos_ball(), tab_obst[m].getcarre(),
				nbCell))
			{
				cout<<COLL_BALL_OBSTACLE(i+1)<<endl;
				if(mode==true) exit(0);
				else erasing_data();
			}
		}
	}
}

vector<Obstacle> get_obst()
{
	return tab_obst;
}

vector<Player> get_player()
{
	return tab_player;
}

vector<Ball> get_ball()
{
	return tab_ball;
}

void save_file(string filename)
{
	ofstream file(filename);
	//opérations a écrire dans le fichier
	//nbCell
	file<<"#nbcell"<<endl<<nbCell<<endl;
	//nb of player
	file<<"#nb player"<<endl<<tab_player.size()<<endl;
	//plzayer position tch et compt
	file<<"#player"<<endl;
	for(auto element:tab_player)
	{
		file<<element.getPos_joueur().x<<"  "<<element.getPos_joueur().y;
		file<<"  "<<element.getNb_touches()<<"  "<<element.getCompteur()<<endl;
	}
	//nb obstacle
	file<<"#nb obstacles"<<endl<<tab_obst.size()<<endl;
	//position obstacle
	file<<"#obstacles"<<endl;
	for(auto element:tab_obst)
		file<<element.getcarre().ligne<<"  "<<element.getcarre().colonne<<endl;
	//nb ball
	file<<"#nb ball"<<endl<<tab_ball.size()<<endl;
	//position ball
	file<<"#ball"<<endl;
	for(auto element:tab_ball)
	{
		file<<element.getPos_ball().x<<"  "<<element.getPos_ball().y<<"  ";
		file<<element.getAngle_b()<<endl;
	}
	file<<"# and that's all"<<endl;
	file.close();
}

bool verif(string nom_fichier)
{
	static int etat;
	etat=NB_CELL;
    string line;
    ifstream fichier(nom_fichier);
    if(!fichier.fail())
    {
        while(getline(fichier >> ws,line))
        {
			if(etat==FIN) break;
			if(line[0]=='#')  continue;
			decodage_ligne(line,false,etat);
        }
		check_error(false);
	}
	//nbCell vaut 0 si erreur lecture car effacage struct de données
	if(nbCell==0) return false;
	return true;

}
//doit arreter les test et effacer les donnees mais pas fermer la fenetre
void erasing_data(int& etat)
{
	etat=FIN;
	nbCell=0;
	tab_ball.clear();
	tab_obst.clear();
	tab_player.clear();
}

void erasing_data()
{
	nbCell=0;
	tab_ball.clear();
	tab_obst.clear();
	tab_player.clear();

}

bool game_over()
{
	if(tab_player.size() <= NB_MIN_PLAYER) return true;
	else return false;
}

int get_nbCell()
{
	return nbCell;
}

size_t get_taille_tab_obst()
{
	return tab_obst.size();
}

Obstacle get_obst_i(int i)
{
	return tab_obst[i];   //retourne le i+1eme obstacle
}

size_t get_taille_tab_player()
{
	return tab_player.size();
}

Player get_player_i(int i)
{
	return tab_player[i];   //retourne le i+1eme joueur
}

size_t get_taille_tab_ball()
{
	return tab_ball.size();
}

Ball get_ball_i(int i)
{
	return tab_ball[i];   //retourne la i+1eme balle
}


//ajout yucef


bool inters_obst_rect(Player proie, Player predateur, Obstacle obstacle)
{
	Coord c1 = indice_en_coord(obstacle.getcarre(),nbCell);
	Coord p1, p2, p3, p4;   // points des 4 coins de l'obstacle
	p1.x = c1.x + MOIT_HAUT;		// p1: haut droit, p2: bas droit, p3: bas gauche, p4: haut gauche
	p1.y = c1.y + MOIT_HAUT;
	p2.x = c1.x + MOIT_HAUT;
	p2.y = c1.y - MOIT_HAUT;
	p3.x = c1.x - MOIT_HAUT;
	p3.y = c1.y + MOIT_HAUT;
	p4.x = c1.x - MOIT_HAUT;
	p4.y = c1.y - MOIT_HAUT;
	if (proie.getPos_joueur().y==predateur.getPos_joueur().y)
		if (sup_rect_obst(c1, predateur.getPos_joueur(),
												proie.getPos_joueur(), nbCell))
			return true;
	if (proie.getPos_joueur().y!=predateur.getPos_joueur().y)
	{
		double pente = pente_chemin(predateur.getCoord_joueur(),
													   proie.getCoord_joueur());
		double b1 = calcul_b(predateur.getCoord_joueur(),pente) + predateur.getPos_joueur().rayon;
		double  2 = calcul_b(predateur.getCoord_joueur(),pente) - predateur.getPos_joueur().rayon;
		if ((fonct_maths(p1.x,pente,b1)<=p1.y+ML) && (fonct_maths(p1.x,pente,b1)>=p2.y+ML))
			return true;
		if ((fonct_maths(p3.x,pente,b1)<=p4.y+ML) && (fonct_maths(p3.x,pente,b1)>=p3.y+ML))
			return true;
		if ((fonct_maths(p1.x,pente,b2)<=p1.y+ML) && (fonct_maths(p1.x,pente,b2)>=p2.y+ML))
			return true;
		if ((fonct_maths(p3.x,pente,b2)<=p4.y+ML) && (fonct_maths(p3.x,pente,b2)>=p3.y+ML))
			return true;
	}
	return false;
}

bool chemin_libre(Player predateur, Player proie)
{
	for (size_t i(0); i<tab_obst.size(); ++i)
	{
		if(carre_dans_zone(predateur.getPos_joueur(),
						   proie.getPos_joueur(), 														//on regarde si l'obstacle est dans la zone concernée pour réduire la complexité de calcul
						   indice_en_coord(tab_obst[i].getcarre(),nbCell),
						   nbCell))
			if(inters_obst_rect(predateur, proie, tab_obst[i]))
				return false;																				//si obstacle, le chemin n'est pas libre ainsi false, sinon true
	}
	return true;
}

void remplis_grille()
{
	///static grille_terrain dans la fonction au dessus
	//remplissage grille
	for(size_t i(0); i<tab_obst.size(); ++i)
	{
		grille_terrain[tab_obst[i].getcarre().ligne][tab_obst[i].getcarre().ligne] = 1;
	}
}

void tableau_spots()
{
	//vector<Spots> tab_spots(nbSpots)
	int compteur(0);
	for (int i(0); i<nbCell ; ++i)
	{
		for (int j(0) ; j<nbCell ; ++j)
		{
			if(grille_terrain[i][j]==0)
			{
				compteur++;
				Spot spot;
				spot.ligne=i;
				spot.colonne=j;
				spot.numero=compteur;
				tab_spots.push_back(spot);
			}
		}
	}
}

void rempl_voisins()
{
	for (size_t k(0); k<tab_spots.size() ; ++k)
	{
		debut_i = 0;
		fin_i=0;
		debut_j = 2;
		fin_j=2;
		tab_spots[k].ligne=ligne;
		tab_spots[k].colonne=colonne;
		if(ligne==0) debut_i = 1;
		if(ligne==nbCell-1) fin_i=1;
		if(colonne==0) debut_j = 1;
		if(colonne==nbCell-1) fin_j=1;

		for (int i(debut_i); i<fin_i ; ++i)
		{
			for(size_t j(debut_j); j<fin_i; ++j)
			{
				if(grille_terrain[ligne-1+i][colonne-1+j]==0)
				{
					if((i==1)||(j==1))
						tab_floyd[tab_spots[k].indice][nbCell*(ligne+i)+colonne+1]==1;
				}
				else
				{
					if((i==0)&&(j==0)) //haut gauche
					{
						tab_floyd[tab_spots[k].indice][nbCell*(ligne+i)+colonne+1]==2;
						if((grille_terrain[ligne][colonne-1]==1)&&(grille_terrain[ligne-1][colonne]==1))
							tab_floyd[tab_spots[k].indice][nbCell*(ligne+i)+colonne+1]==nbCell*nbCell;
						if((grille_terrain[ligne][colonne-1]==1)||(grille_terrain[ligne-1][colonne]==1))
							tab_floyd[tab_spots[k].indice][nbCell*(ligne+i)+colonne+1]==1.41;
					}
					if((i==0)&&(j==2))	//haut droite
					{
						tab_floyd[tab_spots[k].indice][nbCell*(ligne+i)+colonne+1]==2;
						if((grille_terrain[ligne+1][colonne]==1)&&(grille_terrain[ligne][colonne+1]==1))
							tab_floyd[tab_spots[k].indice][nbCell*(ligne+i)+colonne+1]==nbCell*nbCell;
						if((grille_terrain[ligne+1][colonne]==1)||(grille_terrain[ligne][colonne+1]==1))
							tab_floyd[tab_spots[k].indice][nbCell*(ligne+i)+colonne+1]==1.41;
					}
					if((i==2)&&(j==2))	//bas droite
					{
						tab_floyd[tab_spots[k].indice][nbCell*(ligne+i)+colonne+1]==2;
						if((grille_terrain[ligne][colonne+1]==1)&&(grille_terrain[ligne-1][colonne]==1))
							tab_floyd[tab_spots[k].indice][nbCell*(ligne+i)+colonne+1]==nbCell*nbCell;
						if((grille_terrain[ligne][colonne+1]==1)||(grille_terrain[ligne-1][colonne]==1))
							tab_floyd[tab_spots[k].indice][nbCell*(ligne+i)+colonne+1]==1.41;
					}
					if((i==2)&&(j==0))	//bas gauche
					{
						tab_floyd[tab_spots[k].indice][nbCell*(ligne+i)+colonne+1]==2;
						if((grille_terrain[ligne][colonne-1]==1)&&(grille_terrain[ligne-1][colonne]==1))
							tab_floyd[tab_spots[k].indice][nbCell*(ligne+i)+colonne+1]==nbCell*nbCell;
						if((grille_terrain[ligne][colonne-1]==1)||(grille_terrain[ligne-1][colonne]==1))
							tab_floyd[tab_spots[k].indice][nbCell*(ligne+i)+colonne+1]==1.41;
					}
				}
			}
		}
	}
}

void alg_floyd()
{
	//////tab_floyd initialisé aux valeurs nbCell*nbCell
	for(size_t k(0); k<nbSpots ; ++k)
	{
		tab_floyd[k][k]=0;
	}
	for (int m = 0; m < nbSpots; ++m)
	{
		for (int i = 0; i < nbSpots; ++i)
		{
			for (int j = 0; j < nbSpots; ++j)
			{
				if(tab_floyd[i][j]>dist[i][m]+dist[m][j])
					tab_floyd[i][j]=dist[i][m]+dist[m][j]
			}
		}
	}
}

Coord (Player predateur)
{
	//a deja en attribut sa proie
	//choix du prochain spot, qui sera transformé en coord:
	//
	Spot spot_pred;
	Spot spot_proie;
	for(int k(0);k<nbSpots;++k)
	{
		Carre carre;
		carre.ligne = tab_spots[k].ligne;
		carre.ligne = tab_spots[k].colonne;
		centre_carre = indice_en_coord(carre);
		if((fabs(centre_carre.x-predateur.getPos_joueur().x)<=MOIT_HAUT)&&
		   (fabs(centre_carre.y-predateur.getPos_joueur().y)<=MOIT_HAUT))
			spot_pred = tab_spots[k];
	}
	for(int m(0);m<nbSpots;++m)
	{
		Carre carre;
		carre.ligne = tab_spots[m].ligne;
		carre.ligne = tab_spots[m].colonne;
		centre_carre = indice_en_coord(carre);
		Player proie = predateur.getProie();
		if((fabs(centre_carre.x-proie.getPos_joueur().x)<=MOIT_HAUT)&&
		   (fabs(centre_carre.y-proie.getPos_joueur().y)<=MOIT_HAUT))
			spot_proie = tab_spots[k];
	}
	int ligne = spot_pred.ligne;
	int colonne = spot_pred.colonne;
	debut_i = 0;
	fin_i=0;
	debut_j = 2;
	fin_j=2;
	if(ligne==0) debut_i = 1;
	if(ligne==nbCell-1) fin_i=1;
	if(colonne==0) debut_j = 1;
	if(colonne==nbCell-1) fin_j=1;
	double dist_min=tab_floyd[spot_proie.predateur][spot_proie.numero]
	Carre empl_choisi;
	for (int i(debut_i); i<fin_i ; ++i)
	{
		for(size_t j(debut_j); j<fin_i; ++j)
		{
			if(grille_terrain[ligne-1+i][colonne-1+j]==0)
			{
				if(tab_floyd[nbCell*(ligne+i)+colonne+1][spot_proie.numero]
					<min_proie)
				{
					min_proie = tab_floyd[nbCell*(ligne+i)+colonne+1][spot_proie.numero];
					empl_choisi.ligne=ligne-1+i;
					empl_choisi.colonne=colonne-1+j;
				}
			}
		}
	}
	return indice_en_coord(empl_choisi);
}
