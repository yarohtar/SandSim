#include "Element.h"
#include "Water.h"
#include "Sand.h"
#include "Ground.h"
#include "Empty.h"
#include "Out.h"

Element::Element()
{
	is_empty = false;
	is_solid = false;
	is_liquid = false;
	is_moveable = false;
	is_out = false;
	is_gas = false;
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

const Element Element::Water = Water::Water();
const Element Element::Sand = Sand::Sand();
const Element Element::Ground = Ground::Ground();
const Element Element::Empty = Empty::Empty();
const Element Element::Out = Out::Out();
