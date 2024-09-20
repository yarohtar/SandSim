#include "Sand.h"
#include "Map.h"
#include <iostream>


Sand::Sand(float x, float y, Square* currentSquare):MoveableSolid(x,y, currentSquare)
{
	color = sf::Color::Yellow;
	element_id = sand_id;
	chanceToReceiveImpulse = 1;
	friction = 10;
	alpha = 20;
	beta = 1;
}
