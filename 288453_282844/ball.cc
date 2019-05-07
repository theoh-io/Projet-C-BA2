#include "ball.h"
#include "define.h"
#include "error.h"
#include <iostream>

//constructeur initialise le rayon
Ball::Ball(double abs, double ord, double a,int nbcell){
	pos_ball.x=abs;
	pos_ball.y=ord;
	angle_b=a;
	pos_ball.rayon=COEF_RAYON_BALLE*(SIDE/nbcell);
}

//constructeur par defaut
Ball::Ball(){
	pos_ball.x=0;
	pos_ball.y=0;
	angle_b=0;
	pos_ball.rayon=0;
}


// accesseurs

double Ball::getAngle_b () const 
{ 
	return angle_b;
}

Rond Ball::getPos_ball()const 
{ 
	return pos_ball;
}


// manipulateurs

void Ball::setAngle_b(double alpha)
{
	angle_b = alpha;
}

void Ball::setPos_ball(double x1, double y1)
{
	pos_ball.x = x1;
	pos_ball.y = y1;
}





