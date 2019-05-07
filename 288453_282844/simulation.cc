#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "define.h"
#include "error.h"
#include "maps.h"
#include "player.h"
#include "ball.h"
#include "tools.h"
#include "simulation.h"
using namespace std;

#define NB_MIN_PLAYER				1


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

bool falseposition(double x, double y){
	if((x>DIM_MAX)||(x<-DIM_MAX)||(y<-DIM_MAX)||(y>DIM_MAX)) return true;
	else return false;
}

bool falseposition(int colonne,int ligne,int& k){ 
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
