#include "ImmovableSolid.h"

ImmovableSolid::ImmovableSolid(float x, float y, Square* currentSquare) :Element(x, y, currentSquare)
{

}

void ImmovableSolid::update(float delta, Map& map)
{
	return;
}

bool ImmovableSolid::isSolid()
{
	return true;
}

void ImmovableSolid::applyImpulse(Map& map)
{
	return;
}