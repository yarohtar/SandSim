#include "MoveableSolid.h"

MoveableSolid::MoveableSolid() : Element()
{
	speedx = 0;
	speedy = 0;
	time_since_movedx = 0;
	time_since_movedy = 0;
	is_moveable = true;
	is_solid = true;
}

