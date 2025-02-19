#include "GameWorker.h"
#include "Map.h"
#include "Square.h"
#include "Element.h"


GameWorker::GameWorker(Square* currentSquare)
{
	this->currentSquare = currentSquare;
	r = new Random();
}

float GameWorker::rng()
{
	return r->rng();
}

float GameWorker::beta_rng(unsigned int a, unsigned int b)
{
	return r->beta(a,b);
}

void GameWorker::UpdateSquare(float delta)
{
	for (int i = currentSquare->lazyStartX; i < currentSquare->lazyEndX; i++)
	{
		for (int j = currentSquare->lazyStartY; j < currentSquare->lazyEndY; j++)
		{
			UpdateCell(currentSquare->x + i, currentSquare->y+ j, delta);
		}
	}
}

bool GameWorker::inBounds(float x, float y)
{
	x = round(x);
	y = round(y);
	return x >= currentSquare->x && y >= currentSquare->y && x <= currentSquare->x + sqdim - 1 && y <= currentSquare->y + sqdim - 1;
}

inline int GameWorker::getIndex(float x, float y)
{
	return (int)round(x) + sqdim * round(y);
}

Element& GameWorker::getParticle(float x, float y)
{
	if (!inBounds(x, y))
		return map::grid(x, y);
	return currentSquare->getParticle(x, y);
}

Element& GameWorker::getParticleByIndex(int ind)
{
	float x = currentSquare->x + ind % sqdim;
	float y = currentSquare->y + floor(ind / sqdim);
	if (!inBounds(x, y))
		return map::grid(x, y);
	return currentSquare->getParticle(x, y);
}

