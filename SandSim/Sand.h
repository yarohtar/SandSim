#pragma once
#include "MoveableSolid.h"

class Map;

class Sand : public MoveableSolid
{
public:
    Sand(float x, float y, Square* currentSquare);
};

