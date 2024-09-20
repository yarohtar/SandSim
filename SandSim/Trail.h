#pragma once
#include "Empty.h"
class Trail : public Empty
{
public:
	Trail(float x, float y, Square* currentSquare, sf::Color& color);
	void update(float delta, Map& map);
};

