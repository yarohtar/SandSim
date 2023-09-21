#include "Liquid.h"
#include "Map.h"
#include "Square.h"
#include "FakeLiquid.h"

Liquid::Liquid(float x, float y, Square* currentSquare) : Element(x, y, currentSquare)
{
	dontUpdate = 0;
	hasBeenPushed = false;
	pushPressure = 0;
}

bool Liquid::isLiquid()
{
	return true;
}

bool Liquid::isMoveable()
{
	return true;
}

void Liquid::update(float delta, Map& map)
{
	//preUpdate(delta, map);
	if (dontUpdate > 0) {
		dontUpdate--;
		return;
	}


	int s = -1;
	if (dirx > 0)
		s = 1;
	else if (dirx < 0)
		s = -1;
	else if (rng() > 0.5)
		s = 1;

	if (map(x, y + 1).isEmpty())
	{
		if (diry > 0)
			diry *= 2;
		else
			diry = 2;
		float yprev = y;
		moveInDir(dirx, diry, 1 + delta * std::sqrtf(2 * energy / mass), map);
		dirx = 0;
		diry = 1;
		energy += (y - yprev) * mass * gravity;
	}
	else if (map(x + s, y + 1).isEmpty()) {
		if (dirx == 0)
			dirx = s;
		float yprev = y;
		moveInDir(dirx, 1, 3 + delta * std::sqrtf(energy / mass), map);
		dirx = s;
		diry = 1;
		energy += (y - yprev) * mass * gravity;
	}
	else if (map(x - s, y + 1).isEmpty())
	{
		dirx = -s;
		diry = 1;
		float yprev = y;
		moveInDir(dirx, diry, 3 + delta * std::sqrtf(energy / mass), map);
		energy += (y - yprev) * mass * gravity;
	}
	else {
		energy = 0;
		diry = 0;
		dirx = 0;
		if (map(x + s, y).isEmpty())
		{
			dirx = s * 5;
			moveInDir(dirx, diry, 5, map);

		}
		else if (map(x - s, y).isEmpty())
		{
			dirx = -s * 5;
			moveInDir(dirx, diry, 5, map);
		}
	}
	getPressure(map);
}

void Liquid::preUpdate(float delta, Map& map)
{
	getPressure(map);
	color.a = 255 * ( 0.1 + pressure / 20 );
	map.updateParticle(x, y);
	//getPushPressure(map);
	if (pushPressure - pressure > 2 * mass)
	{
		dontUpdate = 1;
		if (map(x, y - 1).isEmpty())
		{
			if (!map(x, y - 1).isFakeLiquid())
			{
				Element* p = new FakeLiquid(x, y - 1, currentSquare);

				map.replaceParticle(x, y - 1, *p);
			}
			swapWith(x, y - 1, map);
			pushPressure -= mass;
		}
		else if (map(x + 1, y - 1).isEmpty())
		{
			if (!map(x+1, y - 1).isFakeLiquid())
			{
				Element* p = new FakeLiquid(x+1, y - 1, currentSquare);

				map.replaceParticle(x+1, y - 1, *p);
			}
			swapWith(x + 1, y - 1, map);
			pushPressure -= mass;
		}
		else if (map(x - 1, y - 1).isEmpty())
		{
			if (!map(x-1, y - 1).isFakeLiquid())
			{
				Element* p = new FakeLiquid(x-1, y - 1, currentSquare);

				map.replaceParticle(x-1, y - 1, *p);
			}
			swapWith(x - 1, y - 1, map);
			pushPressure -= mass;
		}
		else if (map(x + 1, y).isFakeLiquid())
		{
			swapWith(x + 1, y, map);
		}
		else if (map(x - 1, y).isFakeLiquid())
		{
			swapWith(x - 1, y, map);
		}
		else
		{
			for (int i = -1; i <= 1; i++)
			{
				for (int j = -1; j <= 1; j++)
				{
					if (i == 0 && j == 0)
						continue;
					push(x + i, y + j, pushPressure, map);
				}
			}
		}
	}
	else {
		for (int i = -1; i <= 1; i++)
		{
			for (int j = -1; j <= 1; j++)
			{
				if (i == 0 && j == 0)
					continue;
				push(x + i, y + j, pressure, map);
			}
		}
		hasBeenPushed = false;
	}
	currentSquare->updateNextFrame = true;
}

void Liquid::getPressure(Map& map)
{
	pressure = 10000;
	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j < 0; j++)
		{
			Element& p = map(x + i, y + j);
			if (p.isLiquid())
			{
				if (p.pressure + mass < pressure)
				{
					pressure = p.pressure + mass;
				}
			}
		}
	}
	if (pressure != 10000)
		return;
	if (map(x - 1, y - 1).isEmpty() || map(x, y - 1).isEmpty() || map(x + 1, y - 1).isEmpty())
	{
		pressure = 0;
		return;
	}

	for (int i = -1; i <= 1; i++)
	{
		Element& p = map(x + i, y);
		if (p.isLiquid())
		{
			if (p.pressure < pressure)
				pressure = p.pressure;
		}
	}

	pressure = 0;

	return;
}

void Liquid::getPushPressure(Map& map)
{
	pushPressure = pressure;
	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			if (i == 0 && j == 0)
				continue;
			Element& s = map(x + i, y + j);
			if (s.isLiquid())
			{
				float p = s.pressure;
				if (currentSquare->updated[(int)(x - currentSquare->x)][(int)(y - currentSquare->y)])
				{
					p = s.pushPressure;
				}
				if (p - j * mass > pushPressure)
					pushPressure = p - j* mass;
			}
		}
	}
}

void Liquid::push(float xp, float yp, float p, Map& m)
{
	if(pressure==0)
		m(xp, yp).receive_push(p - mass * (y-yp), m.frameCounter);
	else
		m(xp, yp).receive_push(p - mass * (y - yp), pushFrame);
}

void Liquid::receive_push(float p, unsigned long long pushFrame)
{
	if (p > pressure && !hasBeenPushed)
	{
		pushPressure = p;
		hasBeenPushed = true;
		this->pushFrame = pushFrame;
	}
	else if (hasBeenPushed && p > pushPressure && pushFrame > this->pushFrame)
	{
		pushPressure = p;
		this->pushFrame = pushFrame;
	}
}
