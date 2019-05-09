#include <string>
#include <cstdlib>
#include <iostream>
#include "simulation.h"
#include "gui.h"
#include <gtkmm/application.h>
#include "define.h"

using namespace std;

#define HAUTEUR_FENETRE 							SIDE + 100

enum ETAT_TERMINAL {EXECUT_SEUL = 1,ENTREE_TEXTE,ENTREE_ERROR,ENTREE_STEP};
enum LIGNE_COMMANDE {MODE=1,FICHIER,SORTIE};

//mode step

//fonction d'analyse de la ligne de commande
int main(int argc, char * argv[])
{
	switch(argc)
	{
		case EXECUT_SEUL:
		{
			auto app = app_creation(argc, argv, "org.gtkmm.example");
			MyWindow window;
			window.set_default_size(HAUTEUR_FENETRE,SIDE);
			window.set_resizable(false);
			return app->run(window);

		}
		case ENTREE_TEXTE:
		{
			auto app = app_creation();
			MyWindow window;
			window.set_default_size(HAUTEUR_FENETRE,SIDE);
			window.set_resizable(false);
			if(verif(argv[MODE])) window.set_Label("Game Ready to run");
			else window.set_Label("No Game to run");
			if(game_over()) window.set_Label("Game's over!");
			return app->run(window);
		}
		case ENTREE_ERROR:
		{
			std::string s;
			s = argv[MODE];
			if(s=="Error")	lecture(argv[FICHIER]);
		}
		case ENTREE_STEP:
		{
			std::string s;
			s = argv[MODE];
			if(s=="Step") lecture(argv[FICHIER]);
			//appeler la fonction de mise a jour pour 1 boucle
			save_file(SORTIE);
		}
	}
	exit(0);
}
