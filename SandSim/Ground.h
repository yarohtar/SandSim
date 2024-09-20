#pragma once
#include "ImmovableSolid.h"

class Map;

class Ground : public ImmovableSolid
{
public:
    Ground(float x, float y, Square* currentSquare);
};

