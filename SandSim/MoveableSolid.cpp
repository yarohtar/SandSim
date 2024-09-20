#include "MoveableSolid.h"
#include "Map.h"

MoveableSolid::MoveableSolid(float x, float y, Square* currentSquare) : Element(x,y,currentSquare)
{
	chanceToReceiveImpulse = 1;
}

float sgn(float s)
{
	if (s < 0)
		return -1;
	if (s > 0)
		return 1;
	return s;
}

void MoveableSolid::update(float delta, Map& map)
{
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
		isFreeFalling = true;
		moveInDir(dirx, diry, 1 + delta * std::sqrtf(2 * energy / mass), map);
		dirx = 0;
		diry = 1;
		energy += (y - yprev) * mass * gravity;
	}
	else if (map(x + s, y + 1).isEmpty())
	{
		if (isFreeFalling) {
			if (dirx == 0)
				dirx = s;
			float yprev = y;
			moveInDir(dirx, 1, 1 + delta * std::sqrtf(energy / mass), map);
			dirx = s;
			diry = 1;
			energy += (y - yprev) * mass * gravity;
		}
	}
	else if (map(x - s, y + 1).isEmpty())
	{
		if (isFreeFalling) {
			dirx = -s;
			diry = 1;
			float yprev = y;
			moveInDir(dirx, diry, 1 + delta * std::sqrtf(energy / mass), map);
			energy += (y - yprev) * mass * gravity;
		}
	}
	else 
	{
		if (diry > 0) {
			float c = (float)std::abs(dirx) / (std::abs(dirx) + diry);
			energy = energy * (c + (1.f - c) * beta_rng(alpha, beta));
		}
		float xprev = x;
		int n = std::min(delta * std::sqrtf(2.f * energy / mass), energy / (gravity * friction * mass));
		moveInDir(s, 0, n, map);
		if (!map(x, y + 1).isEmpty()) {
			energy -= std::abs(xprev - x) * mass * gravity * friction;
		}
		diry = 0;
		dirx = 3 * s;
		if (x == xprev)
		{
			dirx = 0;
			energy = 0;
			isFreeFalling = false;
		}
	}
	if (energy < 0) {
		energy = 0;
		isFreeFalling = false;
	}
}

bool MoveableSolid::isSolid()
{
	return true;
}

bool MoveableSolid::isMoveable()
{
	return true;
}

void MoveableSolid::receiveImpulse()
{
	if (rng() < chanceToReceiveImpulse)
		isFreeFalling = true;
}
