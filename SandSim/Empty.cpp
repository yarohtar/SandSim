#include "Empty.h"

Empty::Empty(float x, float y, Square* currentSquare) : Element(x,y, currentSquare)
{
	element_id = empty_id;
}

bool Empty::isEmpty()
{
	return true;
}

void Empty::applyImpulse(Map& map)
{
	return;
}

void Empty::update(float delta, Map& map)
{
	return;
}
