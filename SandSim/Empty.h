#pragma once
#include "Element.h"

class Map;

class Empty : public Element
{
protected:
    void applyImpulse(Map& map);
public:
    Empty(float x, float y, Square* currentSquare);
    virtual void update(float delta, Map& map);

    bool isEmpty();
};

