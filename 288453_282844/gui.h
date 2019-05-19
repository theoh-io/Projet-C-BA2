#ifndef GTKMM_EXAMPLE_MYEVENT_H
#define GTKMM_EXAMPLE_MYEVENT_H
#include <gtkmm.h>
#include "simulation.h"
#include <string>


class MyArea : public Gtk::DrawingArea
{
  public:
  MyArea();
  virtual ~MyArea();
  void refresh();
  
  protected:
  //Override default signal handler:
  bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
  private:
  
  
};


class MyWindow : public Gtk::Window
{
  public:
  MyWindow();
  virtual ~MyWindow();
  void switch_etat();
  bool get_etat();

  void set_Label(std::string s);

  protected:
  //Button Signal handlers:
  void on_button_exit_clicked();
  void on_button_open_clicked();
  void on_button_save_clicked();
  void on_button_start_clicked();
  void on_button_step_clicked();

  void start_stop();
  // This is the standard prototype of the Timer callback function
  bool on_timeout();

  Gtk::Box m_Box, m_Box_Top, m_Box_Bottom;
  MyArea              m_Area;
  Gtk::Button    m_Button_Exit,m_Button_Open,m_Button_Save,
				 m_Button_Start, m_Button_Step;
  Gtk::Label     m_Label_Info;

  private:
	void draw();
  // Keep track of the timer status (created or not)
  bool timer_added;

  // to store a timer disconnect request
  bool disconnect;

  // This constant is initialized in the constructor's member initializer:
  const int timeout_value;
};

//functions to be used in projet

int execut_seul();
void entree_texte(std::string s);

#endif // GTKMM_EXAMPLE_MYEVENT_H
