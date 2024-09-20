#include "Ground.h"

Ground::Ground(float x, float y, Square* currentSquare) : ImmovableSolid(x,y, currentSquare)
{
	element_id = ground_id;
	color = sf::Color(150, 75, 0);
}

