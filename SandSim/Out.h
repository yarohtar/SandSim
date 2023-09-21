#pragma once
#include "Element.h"

class Out : public Element
{
public:
    Out();
    Out(float x, float y, Square* s);
    bool isOut();
    void update(float delta, Map& map);
};

