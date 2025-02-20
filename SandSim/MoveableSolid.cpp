#include "MoveableSolid.h"
#include "Sand.h"
#include "Ground.h"
#include <random>

MoveableSolid::MoveableSolid() : Element()
{
	is_moveable = true;
	is_solid = true;
	speedx = 0;
	speedy = 0;
	time_since_movedx = 0;
	time_since_movedy = 0;
}



Sand::Sand() :MoveableSolid()
{
	element_id = sand_id;
	friction = 3;
	alpha = 20;
	beta = 1;
	int r = rand() % 20;
	int g = rand() % 20;
	int b = rand() % 15;
	color = sf::Color(214 + r, 158 + g, 40 + b);
}


