#include <string>
#include <cstdlib>
#include <iostream>
#include "simulation.h"
#include "gui.h"
#include <gtkmm/application.h>
#include "define.h"

using namespace std;

enum ETAT_TERMINAL {EXECUT_SEUL = 1,ENTREE_TEXTE,ENTREE_ERROR,ENTREE_STEP};
enum LIGNE_COMMANDE {MODE=1,FICHIER,SORTIE};


int main(int argc, char * argv[])
{
	switch(argc)
	{
		case EXECUT_SEUL:
		{
			execut_seul();
			break;
		}
		case ENTREE_TEXTE:
		{
			if(verif(argv[MODE])) entree_texte("Game Ready to run");
			else
			{
				entree_texte("No Game to run");
			}
			break;
		}
		case ENTREE_ERROR:
		{
			std::string s;
			s = argv[MODE];
			if(s=="Error")	lecture(argv[FICHIER]);
			exit(0);
		}
		case ENTREE_STEP:
		{
			std::string s;
			s = argv[MODE];
			if(s=="Step") lecture(argv[FICHIER]);
			mise_a_jour();
			save_file(argv[SORTIE]);
		}
	}
	exit(0);
}
