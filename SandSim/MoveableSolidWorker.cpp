#include "MoveableSolidWorker.h"
#include "Map.h"
#include "Element.h"
#include "Random.h"
#include "Square.h"

namespace {
	float invsqrt2 = 1.f / sqrt(2);

	float sgn(float x)
	{
		if (x > 0)
			return 1;
		if (x < 0)
			return -1;
		return 0;
	}
}

MoveableSolidWorker::MoveableSolidWorker(Square* s) : GameWorker(s)
{

}

void MoveableSolidWorker::UpdateCell(const float x, const float y, float delta)
{
	Element& e = getParticle(x, y);
	if (!e.isMoveable() || !e.isSolid())
		return;
	int destx = x;
	int desty = y;
	e.time_since_movedx += delta;
	e.time_since_movedy += delta;


	float s = -1;
	if (e.speedx > 0)
		s = 1;
	else if (-e.speedx < 0.1 && rng() > 0.5)
		s = 1;
	if (getParticle(x, y + 1).isEmpty())
	{
		e.speedy += delta * Element::gravity;
		map::alert(x, y);
	}
	else if (getParticle(x + s, y + 1).isEmpty())
	{
		float u = std::sqrt(e.speedx * e.speedx + e.speedy * e.speedy);
		e.speedx = s * u * invsqrt2;
		e.speedy = u * invsqrt2;
		e.speedy += delta * Element::gravity * invsqrt2;
		e.speedx += s * delta * Element::gravity * invsqrt2;
		map::alert(x, y);
	}
	else if (getParticle(x - s, y + 1).isEmpty())
	{
		float u = std::sqrt(e.speedx * e.speedx + e.speedy * e.speedy);
		e.speedx = -s * u *invsqrt2;
		e.speedy = u *invsqrt2;
		e.speedy += delta * e.gravity *invsqrt2;
		e.speedx -= s * delta * Element::gravity *invsqrt2;
		map::alert(x, y);
	}
	else
	{
		e.speedx += sgn(e.speedx) * e.speedy * beta_rng(e.alpha, e.beta);
		e.speedy = 0;
	}

	if (abs(e.speedx) * e.time_since_movedx >= 1)
	{
		float direction = sgn(e.speedx);
		destx += direction;
		e.speedx -= direction * Element::gravity * e.friction * e.time_since_movedx;
		if (e.speedx * direction < 0)
			e.speedx = 0;
		e.time_since_movedx = 0;
	}
	if (abs(e.speedy) * e.time_since_movedy >= 1)
	{
		desty += sgn(e.speedy);
		e.time_since_movedy = 0;
	}
	
	if (destx != x || desty != y) {
		if (getParticle(destx,desty).isEmpty())
		{
			currentSquare->requestSwap(x, y, destx, desty, currentSquare);
			map::alert(x, y);
		}
		else
		{
			e.speedx = 0;
			e.speedy = 0;
		}
	}

	if (e.speedy * delta > 1)
	{
		e.speedy = 1.f / delta;
	}
	if (abs(e.speedx) * delta > 1)
	{
		e.speedx =sgn(e.speedx) *  1.f / delta;
	}
}