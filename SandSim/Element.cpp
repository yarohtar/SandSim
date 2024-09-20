#include "Element.h"
#include "Map.h"
#include <iostream>
#include <cmath>

Element::Element(float x, float y, Square* currentSquare, Random* rand)
{
	this->x = x;
	this->y = y;
	energy = 0;
	mass = 1;
	isFreeFalling = false;
	isActive = false;
	health = 0;
	heatResistance = 1;
	corrosionResistance = 0;
	heat = 20;
	heatEmitTreshold = 35;
	this->currentSquare = currentSquare;
	r = rand;
	pressure = 0;
}

Element::~Element()
{
	delete r;
}

bool Element::isEmpty()
{
	return false;
}
bool Element::isOut()
{
	return false;
}
bool Element::isLiquid()
{
	return false;
}
bool Element::isFakeLiquid()
{
	return false;
}
bool Element::isGas()
{
	return false;
}
bool Element::isSolid()
{
	return false;
}
bool Element::isMoveable()
{
	return false;
}

bool Element::canMoveThrough(Element& e)
{
	return e.isEmpty();
}

void Element::applyImpulse(Map& map)
{
	if (isFreeFalling)
	{
		for (int i = -1; i <= 1; i++)
			for (int j = -1; j <= 1; j++)
				map(x + i, y + j).receiveImpulse();
	}
}
void Element::receiveImpulse()
{
	return;
}

void Element::applyHeat(float delta, Map& map)
{
	//get surrounding and send heat if heat over threshold
}
double Element::force()
{
	return 0.0;
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

void Element::preUpdate(float delta, Map& map)
{
	return;
}

void Element::update(float delta, Map& map)
{
	preUpdate(delta, map);
	postUpdate(delta, map);
	return;
}

void Element::postUpdate(float delta, Map& map)
{
	return;
}

void Element::setPosition(float x, float y)
{
	this->x = x;
	this->y = y;
}

void Element::setCurrentSquare(Square* square)
{
	currentSquare = square;
}

float Element::rng()
{
	return r->rng();
}

float Element::beta_rng(unsigned int a, unsigned int b)
{
	return r->beta(a,b);
}

bool Element::swapWith(float x2, float y2, Map& m)
{
	if (m(x,y).canMoveThrough(m(x2,y2))) {
		m.swap(x, y, x2, y2);
		return true;
	}
	return false;
}

void Element::moveTo(float x2, float y2, Map& m)
{
	if (std::abs(x2 - x) > std::abs(y2 - y))
	{
		if (x < x2)
		{
			float coef = (y2 - y) / (x2 - x);
			for (float i = x + 1; i <= x2; i++)
			{
				if (!swapWith(i, coef * (i - x) + y, m))
					break;
			}
		}
		else if (x2 < x)
		{
			float coef = (y2 - y) / (x2 - x);
			for (float i = x - 1; i >= x2; i--)
			{
				if (!swapWith(i, coef * (i-x)+y, m))
					break;
			}
		}
	}
	else {
		if (y < y2) {
			float coef = (x2 - x) / (y2 - y);
			for (float i = y + 1; i <= y2; i++)
			{
				if (!swapWith(coef * (i - y) + x, i, m))
					break;
			}
		}
		else if (y2<y) {
			float coef = (x2 - x) / (y2 - y);
			for (float i = y - 1; i >= y2; i--)
			{
				if (!swapWith(coef * (i - y) + x, i, m))
					break;
			}
		}
	}
}

void Element::moveInDir(float dirx, float diry, int n, Map& m)
{
	if (n > 8)
		n = 8;
	if (std::abs(dirx) >= std::abs(diry))
	{
		int step=-1;
		if (dirx>0)
			step = 1;
		for (int i = 0; i < n; i++)
		{
			applyImpulse(m);
			float x2 = x + step;
			float y2 = y + step * diry / dirx;
			if (!swapWith(x2, y2, m))
				return;
		}
	}
	else
	{
		int step = -1;
		if (diry>0)
			step = 1;
		for (int i = 0; i < n; i++)
		{
			applyImpulse(m);
			float y2 = y + step;
			float x2 = x + step * dirx / diry;
			if (!swapWith(x2, y2, m))
				return;
		}
	}
}

void Element::push(float x, float y, float p, Map& m)
{
	return;
}

void Element::receive_push(float p, unsigned long long pushFrame)
{
	return;
}

void Element::receive_liquid_density(int i, float f, Map& map)
{
	return;
}

float Element::gravity = 100.f;