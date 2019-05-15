#ifndef BALL_H
#define BALL_H

#include <vector>
#include "tools.h"

class Ball
{
	public:
		// constructeurs
		Ball();	
		Ball(double abs, double ord, double a, int nbcell);
				
		// accesseurs
		double getAngle_b() const;
		Rond getPos_ball() const; 
	
		// manipulateurs
		void setAngle_b(double);
		void setPos_ball(double,double);
		
	private:
		//declaration des attributs
		double angle_b;
		Rond pos_ball;
};


#endif

//yeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeees
