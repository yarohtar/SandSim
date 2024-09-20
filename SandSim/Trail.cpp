#include "Trail.h"
#include "Map.h"

Trail::Trail(float x, float y, Square* currentSquare, sf::Color& color) : Empty(x,y,currentSquare)
{
	this->color = color;
}

void Trail::update(float delta, Map& map)
{
	map.replaceParticle(x, y, *new Empty(x,y,currentSquare));
}
