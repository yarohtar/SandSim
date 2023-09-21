#pragma once
#include "Liquid.h"
class FakeLiquid : public Liquid
{
private:
    int timeInAir;

public:
    FakeLiquid(float x, float y, Square* currentSquare);
    virtual void preUpdate(float delta, Map& map);
    virtual void update(float delta, Map& map);
    virtual bool isEmpty();
    virtual bool isFakeLiquid();
};

