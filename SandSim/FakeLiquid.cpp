#include "FakeLiquid.h"
#include "Map.h"
#include "Square.h"
#include "Empty.h"

FakeLiquid::FakeLiquid(float x, float y, Square* currentSquare) : Liquid(x,y,currentSquare)
{
	color = sf::Color::Red;
}

void FakeLiquid::preUpdate(float delta, Map& map)
{
	currentSquare->updateNextFrame = true;
}

void FakeLiquid::update(float delta, Map& map)
{
	preUpdate(delta, map);
	bool s = true;
	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			if (!map(x + i, y + j).isLiquid()) {
				timeInAir++;
				if(map(x+i,y+j).isEmpty())
					s = false;
			}
		}
	}
	if (s)
		timeInAir = 0;
	else if (timeInAir >= 2)
	{
		Element* p = new Empty(x, y, currentSquare);
		map.replaceParticle(x, y, *p);
	}
}

bool FakeLiquid::isEmpty()
{
	return true;
}
bool FakeLiquid::isFakeLiquid()
{
	return true;
}