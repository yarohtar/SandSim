#pragma once
#include "Liquid.h"

class Water : public Liquid
{
public:
	Water(float x, float y, Square* currentSquare);
protected:
	virtual Liquid* specialize();
};

