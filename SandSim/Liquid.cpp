#include "Liquid.h"
#include "Water.h"


namespace {
	double ex[9] = { 0, 1, 0, -1, 0, 1, -1, -1, 1 };
	double ey[9] = { 0, 0, 1, 0, -1, 1, 1, -1, -1 };
	double w[9] = { 4.0 / 9.0, 
		1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0,
		1.0 / 36.0, 1.0 / 36.0, 1.0 / 36.0, 1.0 / 36.0 };
	int invDir[9] = {0, 3, 4, 1, 2, 7, 8, 5, 6};
	double gravity = 0.005;
}



Liquid::Liquid() : Element()
{
	for (int i = 0; i < 9; i++) {
		df[i] = w[i];
	}
	last_frame_updated = 0;
	rho0 = 1;
	rho = 1;
}

Water::Water() : Liquid()
{
	element_id = water_id;
	color = sf::Color::Blue;
}
