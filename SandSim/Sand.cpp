#include "Sand.h"
#include "Map.h"
#include <iostream>


Sand::Sand(float x, float y, Square* currentSquare):MoveableSolid(x,y, currentSquare)
{
	color = sf::Color::Yellow;
	element_id = sand_id;
	chanceToReceiveImpulse = 0.6;
	friction = 10;
	alpha = 5;
	beta = 1;
}
