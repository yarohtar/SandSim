#include "Water.h"
#include "Map.h"

Water::Water(float x, float y, Square* currentSquare) : Liquid(x, y, currentSquare)
{
	element_id = water_id;
	color = sf::Color::Blue;
}