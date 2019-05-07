#include <string>
#include <cstdlib>
#include <iostream>
#include "simulation.h"
#include "gui.h"
#include <gtkmm/application.h>
#include "define.h"

using namespace std;

#define HAUTEUR_FENETRE 							SIDE + 100

enum ETAT_TERMINAL {EXECUT_SEUL = 1,ENTREE_TEXTE,ENTREE_EXCES};
enum LIGNE_COMMANDE {MODE=1,FICHIER,NBMOTS};

//fonction d'analyse de la ligne de commande
int main(int argc, char * argv[])
{
	switch(argc)
	{
		case EXECUT_SEUL:
		{
			auto app = Gtk::Application::create(argc, argv, "org.gtkmm.example");
			MyWindow window;
			window.set_default_size(HAUTEUR_FENETRE,SIDE);
			window.set_resizable(false);
			return app->run(window);
			
		}
		case ENTREE_TEXTE:
		{
			auto app = Gtk::Application::create();
			MyWindow window;
			window.set_default_size(HAUTEUR_FENETRE,SIDE);
			window.set_resizable(false);
			if(verif(argv[MODE])) window.set_Label("Game Ready to run");
			else window.set_Label("No Game to run");
			if(game_over()) window.set_Label("Game's over!");
			return app->run(window);
		}
		case ENTREE_EXCES:
		{
			std::string s;
			s = argv[MODE];
			if(s=="Error")	lecture(argv[FICHIER]);
		
		}		
	}
	exit(0);
}
