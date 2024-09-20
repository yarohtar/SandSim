#pragma once
#include "Element.h"
class MoveableSolid : public Element
{
protected:
	float chanceToReceiveImpulse;
	int alpha;
	int beta; //dispersion = alpha/(alpha+beta)
	float friction;

public:
	MoveableSolid(float x, float y, Square* currentSquare);
	virtual void update(float delta, Map& map);
	bool isSolid();
	bool isMoveable();
	virtual void receiveImpulse();

};

