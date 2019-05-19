#include <iostream>
#include <fstream>
#include "gui.h"
#include <cairomm/context.h>
#include <vector>
#include <string>

using namespace std;

#define HAUTEUR_FENETRE 							SIDE + 100
#define TRAIT_NUL				0.0
#define RAD_NUL					0.0
#define RAPP_TRAIT				0.4
#define RAPP_RAYON				0.8
#define TOUR_RAD				2*M_PI
#define TROIS_DEMI_PI			3*M_PI/2
#define HAUT_BOUT				10
#define ORIGINE					0


enum ETAT_JOUEUR {CRITIQUE =1, RISQUE, TOUCHE, SAIN};

void dessin_fenetre(const Cairo::RefPtr<Cairo::Context>& cr);
void dessin_player(const Cairo::RefPtr<Cairo::Context>& cr);
void dessin_ball(const Cairo::RefPtr<Cairo::Context>& cr);
void dessin_obstacle(const Cairo::RefPtr<Cairo::Context>& cr);



MyArea::MyArea()
{
}

MyArea::~MyArea()
{
}


void MyArea::refresh() 
{
  auto win = get_window();
  if(win)
  {
    Gdk::Rectangle r(0,0, get_allocation().get_width(),
									get_allocation().get_height());  
	win->invalidate_rect(r,false);
  }
} 

bool MyArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
	Gdk::Rectangle r(0,0, get_allocation().get_width(), get_allocation().get_height());
	dessin_fenetre(cr);
	cr->set_line_width(TRAIT_NUL);
	dessin_obstacle(cr);
	dessin_ball(cr);
	dessin_player(cr);
	return true;
}

void dessin_fenetre(const Cairo::RefPtr<Cairo::Context>& cr)
{
	cr->set_line_width(1.);
	cr->save();
	cr->move_to(ORIGINE,ORIGINE);
	cr->line_to(SIDE,ORIGINE);
	cr->line_to(SIDE,SIDE);
	cr->line_to(ORIGINE,SIDE);
	cr->line_to(ORIGINE,ORIGINE);
	cr->set_source_rgb(1.0, 1.0, 1.0);
	cr->fill_preserve();
	cr->restore();
	cr->stroke();
}


void dessin_obstacle(const Cairo::RefPtr<Cairo::Context>& cr)
{
	for(size_t j(0); j<get_taille_tab_obst(); ++j)
	{
		Coord carre = chgmnt_coord(indice_en_coord(
							get_obst_i(j).getcarre(), get_nbCell()));
		double moit_haut = DIM_MAX/(get_nbCell());
		cr->save();
	    cr->move_to(carre.x+moit_haut, carre.y+moit_haut);
	    cr->line_to(carre.x+moit_haut, carre.y-moit_haut);
	    cr->line_to(carre.x-moit_haut, carre.y-moit_haut);
	    cr->line_to(carre.x-moit_haut, carre.y+moit_haut);
	    cr->move_to(carre.x+moit_haut, carre.y+moit_haut);
	    cr->set_source_rgb(0.4, 0.0, 0.0);
	    cr->fill_preserve();
		cr->restore();
		cr->stroke();
	}
}

void dessin_ball(const Cairo::RefPtr<Cairo::Context>& cr)
{
	for(size_t m(0); m<get_taille_tab_ball(); ++m)
	{
		Coord ball_orig;
		ball_orig.x = get_ball_i(m).getPos_ball().x;
		ball_orig.y = get_ball_i(m).getPos_ball().y;
		Coord ball_changed = chgmnt_coord(ball_orig);
		cr->save();
		cr->arc(ball_changed.x, ball_changed.y,
				get_ball_i(m).getPos_ball().rayon, RAD_NUL,
				TOUR_RAD);
		cr->set_source_rgb(0.0, 0.0, 0.8);
		cr->fill_preserve();
		cr->restore();
		cr->stroke();
	  }
}

void dessin_player(const Cairo::RefPtr<Cairo::Context>& cr)
{
	for(size_t k(0); k<get_taille_tab_player(); ++k)
	{
		Coord player_orig;
		player_orig.x = get_player_i(k).getPos_joueur().x;
		player_orig.y = get_player_i(k).getPos_joueur().y;
		Coord player_changed = chgmnt_coord(player_orig);

		//dessin joueur avec état
		cr->set_line_width(TRAIT_NUL);
		cr->save();
		cr->arc(player_changed.x, player_changed.y,
				get_player_i(k).getPos_joueur().rayon, RAD_NUL,
				TOUR_RAD);
		switch(get_player_i(k).getNb_touches())
		{
			case CRITIQUE:
			cr->set_source_rgb(0.8, 0.0, 0.0);
			break;
			case RISQUE:
			cr->set_source_rgb(1.0, 0.4, 0.0);
			break;
			case TOUCHE:
			cr->set_source_rgb(1.0, 0.9, 0.0);
			break;
			case SAIN:
			cr->set_source_rgb(0.4, 0.8, 0.2);
			break;
		}
		cr->fill_preserve();
		cr->stroke();
		cr->restore();

	 	//dessin de l'arc compteur
	 	cr->save();
		cr->set_line_width(RAPP_TRAIT*get_player_i(k).getPos_joueur().rayon);
		cr->arc(player_changed.x, player_changed.y,
				get_player_i(k).getPos_joueur().rayon*RAPP_RAYON,
				TROIS_DEMI_PI,
				tps_angle(get_player_i(k).getCompteur()));
		cr->set_source_rgb(0.2, 0.6, 0.6);
		cr->stroke();
		cr->restore();
	}
}


//--------------------------------------

MyWindow::MyWindow() :
  m_Box(Gtk::ORIENTATION_VERTICAL,HAUT_BOUT),
  m_Box_Top(Gtk::ORIENTATION_HORIZONTAL),
	m_Box_Bottom(Gtk::ORIENTATION_HORIZONTAL), m_Button_Exit("Exit"),
	m_Button_Open("Open"),m_Button_Save("Save"),m_Button_Start("Start"),
  m_Button_Step("Step"),m_Label_Info("No game to run"),
	timer_added(false),disconnect(false),timeout_value(100)//DELTA_T) // 500 ms = 0.5 seconds
{
	set_title("Dodgeball");

  	add(m_Box);

  	m_Box.pack_start(m_Box_Top);
  	m_Box.pack_start(m_Box_Bottom);


  	m_Box_Top.pack_start(m_Button_Exit,false,false);// keep fixed width
  	m_Box_Top.pack_start(m_Button_Open,false,false); // and aligned to left;
  	m_Box_Top.pack_start(m_Button_Save,false,false);
  	m_Box_Top.pack_start(m_Button_Start,false,false);
  	m_Box_Top.pack_start(m_Button_Step,false,false);
  	m_Box_Top.pack_start(m_Label_Info);

  	m_Area.set_size_request(SIDE,SIDE);
  	m_Box_Bottom.pack_start(m_Area);

  	// Connect the clicked signal of the button to
  	// their signal handler
  	m_Button_Exit.signal_clicked().connect(sigc::mem_fun(*this,
							&MyWindow::on_button_exit_clicked) );
  	m_Button_Open.signal_clicked().connect(sigc::mem_fun(*this,
							&MyWindow::on_button_open_clicked) );
  	m_Button_Save.signal_clicked().connect(sigc::mem_fun(*this,
							&MyWindow::on_button_save_clicked) );
  	m_Button_Start.signal_clicked().connect(sigc::mem_fun(*this,
							&MyWindow::on_button_start_clicked) );
  	m_Button_Step.signal_clicked().connect(sigc::mem_fun(*this,
							&MyWindow::on_button_step_clicked) );

  	// Show all children of the window
  	show_all_children();
}





MyWindow::~MyWindow()
{
}

void MyWindow::on_button_exit_clicked()
{
	hide();
}
void MyWindow::on_button_open_clicked()
{
	Gtk::FileChooserDialog dialog("Please choose a file",
  	        Gtk::FILE_CHOOSER_ACTION_OPEN);
  	dialog.set_transient_for(*this);
  	dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
  	dialog.add_button("_Open", Gtk::RESPONSE_OK);

  	//Show the dialog and wait for a user response:
  	int result = dialog.run();

  	switch(result)
  	{
    	case(Gtk::RESPONSE_OK):
    	{
      		std::string filename = dialog.get_filename();
      		erasing_data();
      		if(!verif(filename))set_Label("No game to run");
      		else 
      		{
			  set_Label("Game ready to run");
      		  if(game_over()) set_Label("Game's over!");
		    }
      		break;
    	}
    	case(Gtk::RESPONSE_CANCEL):
    	{
      	break;
    	}
  	}
}

void MyWindow::set_Label(string s)
{
	m_Label_Info.set_text(s);
}

void MyWindow::on_button_save_clicked()
{
	Gtk::FileChooserDialog dialog("Please choose a path",
    	      Gtk::FILE_CHOOSER_ACTION_SAVE);
  	dialog.set_transient_for(*this);
 	dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
  	dialog.add_button("_Save", Gtk::RESPONSE_OK);
  	int result = dialog.run();
  	switch(result)
  	{
    	case(Gtk::RESPONSE_OK):
    	{
      		std::string filename = dialog.get_filename();
      		save_file(filename);
      		break;
    	}
    	case(Gtk::RESPONSE_CANCEL):
    	{
      		cout<<"file is not saved"<<endl;
      		break;
    	}
  	}
}

void MyWindow::on_button_start_clicked()
{
	//bouton en mode start
	if(not timer_added)
	{
		Glib::signal_timeout().connect( sigc::mem_fun(*this, &MyWindow::on_timeout),
										timeout_value );
		timer_added = true;
		disconnect = false;  //?
		start_stop();
	}
	//bouton mode stop
	else
	{
	    disconnect  = true;
		timer_added = false;
		start_stop();
	}
}

bool MyWindow::on_timeout()
{
  if(disconnect)
  {
	  disconnect = false; // reset for next time a Timer is created

	  return false; // End of Timer
  }
  //action a faire a chaque tic = mise a jour
  if(game_over())
  {
	set_Label("Game's over!");
	disconnect =true;
	timer_added = false;
	start_stop();
  }
  else mise_a_jour();
  m_Area.refresh();
  return true; // keep the Timer working
}


void MyWindow::start_stop()
{
	if(timer_added) m_Button_Start.set_label("Stop");
	else m_Button_Start.set_label("Start");
}


void MyWindow::on_button_step_clicked()
{
	if(game_over())
	{
	  set_Label("Game's over!");
	  disconnect =true;
	  timer_added = false;
	  start_stop();
	}
	else mise_a_jour();
	m_Area.refresh();
}

int execut_seul()
{
	auto app = Gtk::Application::create();
	MyWindow window;
	window.set_default_size(HAUTEUR_FENETRE,SIDE);
	window.set_resizable(false);
	return app->run(window);
}

void entree_texte(string s)
{
	auto app =Gtk::Application::create();
	MyWindow window;
	window.set_default_size(HAUTEUR_FENETRE,SIDE);
	window.set_resizable(false);
	if(s=="Game Ready to run")
	{
		window.set_Label("Game Ready to run");
		if(game_over()) window.set_Label("Game's over!");
	}
	else if(s=="No Game to run") window.set_Label("No Game to run");
	app->run(window);
}
