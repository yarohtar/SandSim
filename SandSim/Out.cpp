#include "Out.h"

Out::Out() : Element(0,0,nullptr)
{
	element_id = out_id;
}

Out::Out(float x, float y, Square* s) : Element(x, y, s)
{
	element_id = out_id;
}

bool Out::isOut()
{
	return true;
}

void Out::update(float delta, Map& map)
{
	return;
}