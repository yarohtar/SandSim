#include "ImmovableSolid.h"
#include "Ground.h"

ImmovableSolid::ImmovableSolid() :Element()
{
	is_solid = true;
}

Ground::Ground() : ImmovableSolid()
{
	element_id = ground_id;
	color = sf::Color(150, 75, 0);
	sf::Color::Black;
}