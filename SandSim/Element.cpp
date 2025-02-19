#include "Element.h"

Element::Element()
{
	energy = 0;
	mass = 1;
	isFreeFalling = false;
	isActive = false;
	health = 0;
	heatResistance = 1;
	corrosionResistance = 0;
	heat = 20;
	heatEmitTreshold = 35;
	pressure = 0;
	is_empty = false;
	is_out = false;
	is_liquid = false;
	is_gas = false;
	is_solid = false;
	is_moveable = false;
}

Element::~Element()
{
}

bool Element::isEmpty()
{
	return is_empty;
}
bool Element::isOut()
{
	return is_out;
}
bool Element::isLiquid()
{
	return is_liquid;
}
bool Element::isFakeLiquid()
{
	return false;
}
bool Element::isGas()
{
	return is_gas;
}
bool Element::isSolid()
{
	return is_solid;
}
bool Element::isMoveable()
{
	return is_moveable;
}

bool Element::canMoveThrough(Element& e)
{
	return e.isEmpty();
}

void Element::applyHeat(float delta)
{
	//get surrounding and send heat if heat over threshold
}
void Element::receiveHeat(int receivedHeat, float delta)
{
	heat = (heat - receivedHeat) * std::exp(-delta / heatResistance) + receivedHeat;
}

void Element::receiveCorrosion()
{
	return;
}

sf::Color Element::makeColor()
{
	return sf::Color::Black;
}


void Element::push(float x, float y, float p)
{
	return;
}

void Element::receive_push(float p, unsigned long long pushFrame)
{
	return;
}

void Element::receive_liquid_density(int i, float f)
{
	return;
}

float Element::gravity = 150.f;

