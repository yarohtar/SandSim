#include "Sand.h"
#include <random>


Sand::Sand():MoveableSolid()
{
	int r = rand()%20;
	int g = rand() % 20;
	int b = rand() % 15;
	color = sf::Color(214+r, 158+g, 40+b);
	element_id = sand_id;
	friction = 3;
	alpha = 20;
	beta = 1;
}
