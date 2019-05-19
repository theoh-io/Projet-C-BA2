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
#define MOIT_HAUT 					SIDE/(2*nbCell)
#define PRECISION2					2

static int nbCell=0;
static int nbSpots=0;
static vector<Obstacle> tab_obst;
static vector<Player> tab_player;
static vector <Ball> tab_ball;
static vector <Spot> tab_spots;
static vector<vector<int> > grille_terrain;
static vector<vector<double> > tab_floyd;

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
void lancer_balles();
void creer_balle(Player& p);
void deplacement_balles();
void collisions_balles();
void destroy_balle(int i);
void destroy_obstacle(int k);
void destroy_player();
bool inters_obst_rect(Rond proie, Player predateur, Coord obstacle);
bool chemin_libre(Player predateur);
void remplis_grille();
void tableau_spots(); 
void rempl_voisins(); 
void alg_floyd(); 
Coord direction_pred(Player predateur); 
void init_tab(); 
void affich_test(); 
Player recup_cible(Rond cible,vector<Player>& tab_before);
void rapprochement(Player predateur,vector<Player>& tab_before);


void mise_a_jour()
{
  analyse();
  lancer_balles();
  deplacement_balles();
  collisions_balles();
  destroy_player();
}

void collisions_balles()
{
	for(size_t i(tab_ball.size());i>0;--i)
	{
		for(size_t j(tab_player.size());j>0;--j)
		{
			if(supp_cercles_en_jeu(tab_ball[i-1].getPos_ball(),tab_player[j-1].getPos_joueur(), nbCell))
			{
				tab_player[j-1].shot();
				destroy_balle(i-1);
				break;
			}
		}
	}
	for(size_t i(tab_ball.size());i>0;--i)
	{
		for(size_t k(tab_obst.size());k>0;k--)
		{
			if(supp_cercle_carre(tab_ball[i-1].getPos_ball(),tab_obst[k-1].getcarre() ,nbCell))
			{
				destroy_balle(i-1);
				destroy_obstacle(k-1);
				break;
			}
		}
	}
	for(size_t i(tab_ball.size());i>0;--i)
	{
		for(size_t l(tab_ball.size());l>0;--l)
		{
			if(i != l)
			{
			  if(supp_cercles_en_jeu(tab_ball[i-1].getPos_ball(),tab_ball[l-1].getPos_ball(), nbCell))
			  {
				  if(l>i)
				  {
				    destroy_balle(l-1);
				    destroy_balle(i-1);
			      }
			      else
			      {
				    destroy_balle(i-1);
				    destroy_balle(l-1);
			      }
			      --i;
			      break;
			  }
		    }
		}
	}						
}


void destroy_balle(int i)
{
	swap(tab_ball[i],tab_ball.back());
	tab_ball.pop_back();
}

void destroy_obstacle(int k)
{
	swap(tab_obst[k],tab_obst.back());
	tab_obst.pop_back();
	void init_tab(); 
}

void destroy_player()
{
	for(size_t i(tab_player.size());i>0;--i)
	{
		if(tab_player[i-1].getNb_touches()==0)
		{
			swap(tab_player[i-1],tab_player.back());
			tab_player.pop_back();
		}
	}
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


void direction_deplacement()
{
  Coord direction;
  Coord direction_normee;
  double n;
  for(size_t j(0);j<tab_player.size();++j)
  {
    if(chemin_libre(tab_player[j]))
    {
      //deplacer tout droit vers cible
    	  cout<<"mouvement tout droit"<<endl;
		  direction.x=(tab_player[j].getCible().x-tab_player[j].getAbcisse());
		  direction.y=(tab_player[j].getCible().y-tab_player[j].getOrdonnee());
		  n=norme(direction);
		  direction_normee.x=direction.x/n;
		  direction_normee.y=direction.y/n;
		  futur_deplacement(tab_player[j],direction_normee);
    }
   else
        //deplacer dans la direction donnée par algo de Floyd
        {
        	cout<<"pas mouvement tout droit"<<endl;
		  direction=direction_pred(tab_player[j]);
		  n=norme(direction);
		  direction_normee.x=direction.x/n;
		  direction_normee.y=direction.y/n;
	      futur_deplacement(tab_player[j],direction_normee);
	    }
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
  Player cible;
  vector<Player> tab_before(tab_player);
  for(size_t i(0);i<tab_before.size();++i)
  {
	cible=recup_cible(tab_before[i].getCible(),tab_before);
    if(!supp_cercles_en_jeu(tab_before[i].getfutur_pos(),cible.getfutur_pos(),nbCell))
      tab_player[i].valider_deplacement();
    else
    {
		rapprochement(tab_player[i],tab_before);
		tab_player[i].annuler_deplacement();
	}
  }
}

//jamais appelée
void rapprochement(Player predateur,vector<Player>& tab_before)
{
	cout<<"rapprochement effectue"<<endl<<endl<<endl;
	Player cible;
	Coord direction,direction_normee;
	cible=recup_cible(predateur.getCible(),tab_before);
	double new_dist=dist_deux_points(predateur.getfutur_pos_coord(),cible.getfutur_pos_coord());
	direction.x=(cible.getPos_joueur().x-predateur.getPos_joueur().x);
	direction.y=(cible.getPos_joueur().y-predateur.getPos_joueur().y);
	double n=norme(direction);
	direction_normee.x=direction.x/n;
	direction_normee.y=direction.y/n;
	double distance=2*predateur.getPos_joueur().rayon-new_dist;
	predateur.bouger_joueur(distance*direction_normee.x,distance*direction_normee.y);
	predateur.valider_deplacement();
}


Player recup_cible(Rond cible,vector<Player>& tab_before)
{
	for(size_t i(0);i<tab_before.size();++i)
	{
		cout<<(abs(tab_before[i].getPos_joueur().x-cible.x)<=PRECISION)<<"  "<<(abs(tab_before[i].getPos_joueur().y-cible.y)<=PRECISION)<<endl;
		if((abs(tab_before[i].getPos_joueur().x-cible.x)<=PRECISION) and (abs(tab_before[i].getPos_joueur().y-cible.y)<=PRECISION))
		{
		  return tab_player[i];
	    }
	}
    Player p(cible.x,cible.y);
    return p;
}



void lancer_balles()
{
  bool compteur_max=0;
  for(size_t i(0);i<tab_player.size();++i)
	{
	  compteur_max = tab_player[i].upCompteur();	
	  if(chemin_libre(tab_player[i]))
	  {
		if(compteur_max) creer_balle(tab_player[i]);
	  }
  }
}

void creer_balle(Player& p)
{
  p.resetCompteur();
  double angle= calcul_angle(p.getCoord(),p.getCible_coord());
  double abs=p.getAbcisse();
  double ord=p.getOrdonnee();
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
		init_tab();
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
		init_tab();
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
	nbSpots=0;
	tab_ball.clear();
	tab_obst.clear();
	tab_player.clear();
	tab_spots.clear();
	grille_terrain.clear();
	tab_floyd.clear();
}

void erasing_data()
{
	nbCell=0;
	nbSpots=0;
	tab_ball.clear();
	tab_obst.clear();
	tab_player.clear();
	tab_spots.clear();
	grille_terrain.clear();
	tab_floyd.clear();

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

bool inters_obst_rect(Rond proie, Player predateur, Coord obstacle)
{
	Coord c1 = obstacle;
	Coord p1, p2, p3, p4,proie_coord;   // points des 4 coins de l'obstacle
	p1.x = c1.x + MOIT_HAUT;		// p1: haut droit, p2: bas droit, p3: bas gauche, p4: haut gauche
	p1.y = c1.y + MOIT_HAUT;
	p2.x = c1.x + MOIT_HAUT;
	p2.y = c1.y - MOIT_HAUT;
	p3.x = c1.x - MOIT_HAUT;//p3 en haut a gauche au lieu de bas gaucghe
	p3.y = c1.y + MOIT_HAUT;
	p4.x = c1.x - MOIT_HAUT;
	p4.y = c1.y - MOIT_HAUT;
	proie_coord.x=proie.x;
	proie_coord.y=proie.y;
	
	if (fabs(proie.x-predateur.getPos_joueur().x)<PRECISION)//PRECISION)
	{
		if (sup_rect_obst(c1, predateur.getPos_joueur(),
												proie, nbCell)) // a tester
		{
			return true;
		}
	}
	else
	{
		cout<<endl<<endl;
		cout<<"coord pred :"<<predateur.getCoord().x<<"  "<<predateur.getCoord().y<<endl;
		cout<<"coord cible :"<<proie_coord.x<<"  "<<proie_coord.y<<endl;
		Coord a1,a2;
		//cas particulier cible a la verticale
		cout<<fabs(calcul_angle(predateur.getCoord(),proie_coord))<<"  "<<fabs(calcul_angle(predateur.getCoord(),proie_coord)-M_PI)<<endl;
		if((fabs(calcul_angle(predateur.getCoord(),proie_coord)-M_PI/2)<0.1)||(fabs(calcul_angle(predateur.getCoord(),proie_coord)-3*M_PI/2)<0.1))
		{
			if (sup_rect_obst(c1, predateur.getPos_joueur(),
												proie, nbCell)) // a tester
			{
				return true;
			}
		}
				//cas particulier cible a la verticale
		else if(fabs(calcul_angle(predateur.getCoord(),proie_coord)<0.1)||(fabs(calcul_angle(predateur.getCoord(),proie_coord)-M_PI)<0.1))
		{
			cout<<"horizontale"<<endl;
			a1.x=predateur.getCoord().x;
			a1.y=predateur.getCoord().y+(predateur.getPos_joueur().rayon+PRECISION2);
			a2.x=predateur.getCoord().x;
			a2.y=predateur.getCoord().y-(predateur.getPos_joueur().rayon+PRECISION2);
		}
		else if((calcul_angle(predateur.getCoord(),proie_coord)>M_PI)||(calcul_angle(predateur.getCoord(),proie_coord)<0))
		{
			a1.x=predateur.getCoord().x+cos(calcul_angle(predateur.getCoord(),proie_coord))*(predateur.getPos_joueur().rayon+PRECISION2);
			a1.y=predateur.getCoord().y-sin(calcul_angle(predateur.getCoord(),proie_coord))*(predateur.getPos_joueur().rayon+PRECISION2);
			a2.x=predateur.getCoord().x-cos(calcul_angle(predateur.getCoord(),proie_coord))*(predateur.getPos_joueur().rayon+PRECISION2);
			a2.y=predateur.getCoord().y+sin(calcul_angle(predateur.getCoord(),proie_coord))*(predateur.getPos_joueur().rayon+PRECISION2);

		}
		else if((calcul_angle(predateur.getCoord(),proie_coord)<=M_PI)||(calcul_angle(predateur.getCoord(),proie_coord)>=0))
		{
			a2.x=predateur.getCoord().x+cos(calcul_angle(predateur.getCoord(),proie_coord))*(predateur.getPos_joueur().rayon+PRECISION2);
			a1.y=predateur.getCoord().y+sin(calcul_angle(predateur.getCoord(),proie_coord))*(predateur.getPos_joueur().rayon+PRECISION2);
			a1.x=predateur.getCoord().x-cos(calcul_angle(predateur.getCoord(),proie_coord))*(predateur.getPos_joueur().rayon+PRECISION2);
			a2.y=predateur.getCoord().y-sin(calcul_angle(predateur.getCoord(),proie_coord))*(predateur.getPos_joueur().rayon+PRECISION2);
						//switch des x
		}

		cout<<"a1 "<<a1.x<<"  "<<a1.y<<endl;
		cout<<"a2 "<<a2.x<<"  "<<a2.y<<endl;
		cout<<"l'angle est "<<calcul_angle(predateur.getCoord(),proie_coord)<<endl;

		double pente = pente_chemin(predateur.getCoord(),proie_coord);
		double b1 = calcul_b(a1,pente);  //b1 au dessus 
		double b2 = calcul_b(a2,pente);	
		if(b2>b1)
		{
			double temp=0;
			temp=b1;
			b1=b2;
			b2=temp;
		}
		cout<<"pent :"<<pente<<endl;
		cout<<"b1 "<<b1<<endl;
		cout<<"b2 "<<b2<<endl;
		if(inter_cote_carre(p1.x,p2.y-PRECISION2,p1.y+PRECISION2,pente,b1,b2))
		{
			return true;
		}

		if(inter_cote_carre(p3.x,p4.y-PRECISION2,p3.y+PRECISION2,pente,b1,b2))
		{
			return true;
		}


		if(inter_haut_carre(p2.y,p3.x-PRECISION2,p2.x+PRECISION2,pente,b1,b2))
		{
			return true;
		}

		if(inter_haut_carre(p1.y,p3.x-PRECISION2,p1.x+PRECISION2,pente,b1,b2))
		{
			cout<<"intersection coté haut"<<endl;
			return true;
		}
	}
	return false;
}

bool chemin_libre(Player predateur)
{
	for (size_t i(0); i<tab_obst.size(); ++i) // a mettre en decrementation
	{
		cout<<"test sur l'obst :"<<(indice_en_coord(tab_obst[i].getcarre(),nbCell)).x<<(indice_en_coord(tab_obst[i].getcarre(),nbCell)).y<<endl;
		if(carre_dans_zone(predateur.getPos_joueur(),
						   predateur.getCible(), 														//on regarde si l'obstacle est dans la zone concernée pour réduire la complexité de calcul
						   indice_en_coord(tab_obst[i].getcarre(),nbCell),
						   nbCell))
		{
			cout<<"il est en zone"<<endl;
			if(inters_obst_rect(predateur.getCible(), predateur, indice_en_coord(tab_obst[i].getcarre(),nbCell)))
			{ 
				return false;			
			}
		}														//si obstacle, le chemin n'est pas libre ainsi false, sinon true
	}
	return true;
}

void remplis_grille()
{
	for(int k(0); k<nbCell; ++k)
	{
		vector<int> colonne(nbCell,0);
		grille_terrain.push_back(colonne);
	}
	for(size_t i(0); i<tab_obst.size(); ++i)
	{
		grille_terrain[tab_obst[i].getcarre().ligne][tab_obst[i].getcarre().colonne] = 1;
	}
}

void tableau_spots()
{
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
	for(int k(0); k<nbSpots; ++k)
	{
		vector<double> colonne(nbSpots,nbCell*nbCell);
		tab_floyd.push_back(colonne);
	}
	int debut_i, fin_i, debut_j, fin_j, ligne, colonne;
	for (size_t k(0); k<tab_spots.size() ; ++k)
	{
		debut_i = 0;
		fin_i=2;
		debut_j = 0;
		fin_j=2;
		ligne=tab_spots[k].ligne;
		colonne=tab_spots[k].colonne;
		if(ligne==0) debut_i = 1;
		if(ligne==nbCell-1) fin_i=1;
		if(colonne==0) debut_j = 1;
		if(colonne==nbCell-1) fin_j=1;   // cas speciaux pour les bords
	//	cout<<"numero "<<tab_spots[k].numero<<endl;

		for (int i(debut_i); i<=fin_i ; ++i)
		{
			for(int j(debut_j); j<=fin_j; ++j)
			{
	//			cout<<"ligne pointee: "<< ligne-1+i <<" et col pointee: "<< colonne-1+j <<endl;
				if(grille_terrain[ligne-1+i][colonne-1+j]==0)   //si pas d'obstacles
				{
					int spot_pointe = numero_spot(ligne-1+i, colonne-1+j, tab_spots);
	//				cout<<"spot libre detecte"<<endl;
					if((i==1)||(j==1))
					{
						tab_floyd[tab_spots[k].numero-1][spot_pointe-1]=1;
	//					cout<<"i=1 ou j=1"<<endl;
					}
					else
					{
	//					cout<<"dans le else"<<endl;
						if((i==0)&&(j==0)) //haut gauche
						{
							tab_floyd[tab_spots[k].numero-1][spot_pointe-1]=1.41;
							if((grille_terrain[ligne][colonne-1]==1)||(grille_terrain[ligne-1][colonne]==1))
								tab_floyd[tab_spots[k].numero-1][spot_pointe-1]=2;
							if((grille_terrain[ligne][colonne-1]==1)&&(grille_terrain[ligne-1][colonne]==1))
								tab_floyd[tab_spots[k].numero-1][spot_pointe-1]=nbCell*nbCell;
						}
	//					cout<<"je suis al 1"<<endl;
						if((i==0)&&(j==2))	//haut droite
						{
							tab_floyd[tab_spots[k].numero-1][spot_pointe-1]=1.41;
							if((grille_terrain[ligne-1][colonne]==1)||(grille_terrain[ligne][colonne+1]==1))
								tab_floyd[tab_spots[k].numero-1][spot_pointe-1]=2;							
							if((grille_terrain[ligne-1][colonne]==1)&&(grille_terrain[ligne][colonne+1]==1))
								tab_floyd[tab_spots[k].numero-1][spot_pointe-1]=nbCell*nbCell;

						}
	//					cout<<"je suis al 2"<<endl;
						if((i==2)&&(j==2))	//bas droite
						{
							tab_floyd[tab_spots[k].numero-1][spot_pointe-1]=1.41;
							if((grille_terrain[ligne][colonne+1]==1)||(grille_terrain[ligne+1][colonne]==1))
								tab_floyd[tab_spots[k].numero-1][spot_pointe-1]=2;
							if((grille_terrain[ligne][colonne+1]==1)&&(grille_terrain[ligne+1][colonne]==1))
								tab_floyd[tab_spots[k].numero-1][spot_pointe-1]=nbCell*nbCell;
						}
	//					cout<<"je suis al 3"<<endl;
						if((i==2)&&(j==0))	//bas gauche
						{
	//						cout<<"je suis dans le coin bas gauche"<<endl;
							tab_floyd[tab_spots[k].numero-1][spot_pointe-1]=1.41;
	//						cout<<"jai passe la premiere action"<<endl;
							if((grille_terrain[ligne][colonne-1]==1)||(grille_terrain[ligne+1][colonne]==1))
								tab_floyd[tab_spots[k].numero-1][spot_pointe-1]=2;
							if((grille_terrain[ligne][colonne-1]==1)&&(grille_terrain[ligne+1][colonne]==1))
								tab_floyd[tab_spots[k].numero-1][spot_pointe-1]=nbCell*nbCell;
						}
	//					cout<<"je suis al 4"<<endl;
					}
				}
			}
		}
	}
}

void alg_floyd()
{
	for(int k(0); k<nbSpots ; ++k)
	{
		tab_floyd[k][k]=0;
	}
	for (int m = 0; m < nbSpots; ++m)
	{
		for (int i = 0; i < nbSpots; ++i)
		{
			for (int j = 0; j < nbSpots; ++j)
			{
				if(tab_floyd[i][j]>tab_floyd[i][m]+tab_floyd[m][j])
					tab_floyd[i][j]=tab_floyd[i][m]+tab_floyd[m][j];
			}
		}
	}
}

Coord direction_pred(Player predateur)
{
	//a deja en attribut sa proie
	//choix du prochain spot, qui sera transformé en coord:
	Spot spot_pred;
	Spot spot_proie;
	int debut_i,debut_j,fin_i,fin_j;
	for(int k(0);k<nbSpots;++k)  //on cherche le spot du predateur
	{
		Carre carre;
		carre.ligne = tab_spots[k].ligne;
		carre.colonne = tab_spots[k].colonne;
		Coord centre_carre = indice_en_coord(carre,nbCell);
		if((fabs(centre_carre.x-predateur.getPos_joueur().x)<=MOIT_HAUT)&&  //test
		   (fabs(centre_carre.y-predateur.getPos_joueur().y)<=MOIT_HAUT))
			{
				spot_pred = tab_spots[k];
		//		cout<<"un nv spot pred affectè"<<endl;
			}
	}
//	cout<<"numero du spot du pred: "<< spot_pred.numero <<endl;
	for(int m(0);m<nbSpots;++m)  //on cherche le spot de la proie
	{
		Carre carre;
		carre.ligne = tab_spots[m].ligne;
		carre.colonne = tab_spots[m].colonne;
		Coord centre_carre = indice_en_coord(carre,nbCell);
		Rond proie = predateur.getCible();
		if((fabs(centre_carre.x-proie.x)<=MOIT_HAUT)&&
		   (fabs(centre_carre.y-proie.y)<=MOIT_HAUT))
			spot_proie = tab_spots[m];
	}
	int ligne = spot_pred.ligne;
	int colonne = spot_pred.colonne;
	debut_i = 0;
	fin_i=2;
	debut_j = 0;
	fin_j=2;
	if(ligne==0) debut_i = 1;
	if(ligne==nbCell-1) fin_i=1;
	if(colonne==0) debut_j = 1;
	if(colonne==nbCell-1) fin_j=1;
	double dist_min=tab_floyd[spot_pred.numero-1][spot_proie.numero-1];
	Carre empl_choisi;
	for (int i(debut_i); i<=fin_i ; ++i)
	{
	//	cout <<"pointe sur la ligne   "<<ligne-1+i<<endl;
		for(int j(debut_j); j<=fin_j; ++j)
		{
	//		cout <<"pointe sur la colonne "<<colonne-1+j<<endl;
			if(grille_terrain[ligne-1+i][colonne-1+j]==0)
			{
				if(tab_floyd[numero_spot(ligne-1+i,colonne-1+j,tab_spots)-1][spot_proie.numero-1]
					<=dist_min)
				{
						if(tab_floyd[spot_pred.numero-1][numero_spot(ligne-1+i,colonne-1+j,tab_spots)-1]==1.41
							||(i==1)||(j==1))	//on veut que le joueur passe que par les diagonales tot libres ou verticalem ou horiz								
						{	
							if(tab_floyd[numero_spot(ligne-1+i,colonne-1+j,tab_spots)-1][spot_proie.numero-1]
							   ==dist_min)
							{
								if(j==2)
								{
									dist_min = tab_floyd[numero_spot(ligne-1+i,colonne-1+j,tab_spots)-1][spot_proie.numero-1];
									empl_choisi.ligne=ligne-1+i;
									empl_choisi.colonne=colonne-1+j;		//a encapsuler
			//						cout<<"numero du spot choisi "<<numero_spot(ligne-1+i,colonne-1+j,tab_spots)<<endl; 
								}
								else if (i==0)
								{
									dist_min = tab_floyd[numero_spot(ligne-1+i,colonne-1+j,tab_spots)-1][spot_proie.numero-1];
									empl_choisi.ligne=ligne-1+i;
									empl_choisi.colonne=colonne-1+j;
			//						cout<<"numero du spot choisi "<<numero_spot(ligne-1+i,colonne-1+j,tab_spots)<<endl;
								}
								else if (i==2)
								{
									dist_min = tab_floyd[numero_spot(ligne-1+i,colonne-1+j,tab_spots)-1][spot_proie.numero-1];
									empl_choisi.ligne=ligne-1+i;
									empl_choisi.colonne=colonne-1+j;
			//						cout<<"numero du spot choisi "<<numero_spot(ligne-1+i,colonne-1+j,tab_spots)<<endl;
								}
							}
							else	
							dist_min = tab_floyd[numero_spot(ligne-1+i,colonne-1+j,tab_spots)-1][spot_proie.numero-1];
							empl_choisi.ligne=ligne-1+i;
							empl_choisi.colonne=colonne-1+j;
			//				cout<<"numero du spot choisi "<<numero_spot(ligne-1+i,colonne-1+j,tab_spots)<<endl;
						}
					//}	
				}
			}
		}
	}
	Coord direction;
	direction.x=indice_en_coord(empl_choisi,nbCell).x-predateur.getPos_joueur().x;
	direction.y=indice_en_coord(empl_choisi,nbCell).y-predateur.getPos_joueur().y;
	return direction;
}

void init_tab()
{
	nbSpots=(nbCell*nbCell)-tab_obst.size();
	remplis_grille();
	tableau_spots();
	rempl_voisins();
	alg_floyd();
}
