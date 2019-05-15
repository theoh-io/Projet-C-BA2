#ifndef SIMU_H
#define SIMU_H
#include <iostream>
#include <vector>
#include "maps.h"
#include "player.h"
#include "ball.h"

void lecture(char * nom_fichier);

void save_file(std::string filename);
bool verif(std::string nom_fichier);
void erasing_data(int& etat);
void erasing_data();
bool game_over();

int get_nbCell();
std::vector<Obstacle> get_obst();
std::vector<Player> get_player();
std::vector<Ball> get_ball();
Obstacle get_obst_i(int indice);
Player get_player_i(int indice);
Ball get_ball_i(int indice);
size_t get_taille_tab_obst();
size_t get_taille_tab_player();
size_t get_taille_tab_ball();


#endif
