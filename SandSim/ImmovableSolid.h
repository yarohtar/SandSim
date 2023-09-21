#pragma once
#include "Element.h"
class ImmovableSolid : public Element
{
protected:
	virtual void applyImpulse(Map& map);

public:
	ImmovableSolid(float x, float y, Square* currentSquare);
	virtual void update(float delta, Map& map);
	virtual bool isSolid();
};

