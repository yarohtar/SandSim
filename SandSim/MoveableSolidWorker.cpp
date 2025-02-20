#include "MoveableSolidWorker.h"
#include "Map.h"
#include "Element.h"
#include "Random.h"
#include "Square.h"
#include "Cell.h"

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
	Cell& cell = getParticle(x, y);
	if (!cell.isMoveable() || !cell.isSolid())
		return;
	int destx = x;
	int desty = y;
	cell.time_since_movedx += delta;
	cell.time_since_movedy += delta;


	float s = -1;
	if (cell.speedx > 0)
		s = 1;
	else if (-cell.speedx < 0.1 && rng() > 0.5)
		s = 1;
	if (getParticle(x, y + 1).isEmpty())
	{
		cell.speedy += delta * Element::gravity;
		map::alert(x, y);
	}
	else if (getParticle(x + s, y + 1).isEmpty())
	{
		float u = std::sqrt(cell.speedx * cell.speedx + cell.speedy * cell.speedy);
		cell.speedx = s * u * invsqrt2;
		cell.speedy = u * invsqrt2;
		cell.speedy += delta * Element::gravity * invsqrt2;
		cell.speedx += s * delta * Element::gravity * invsqrt2;
		map::alert(x, y);
	}
	else if (getParticle(x - s, y + 1).isEmpty())
	{
		float u = std::sqrt(cell.speedx * cell.speedx + cell.speedy * cell.speedy);
		cell.speedx = -s * u *invsqrt2;
		cell.speedy = u *invsqrt2;
		cell.speedy += delta * Element::gravity *invsqrt2;
		cell.speedx -= s * delta * Element::gravity *invsqrt2;
		map::alert(x, y);
	}
	else
	{
		cell.speedx += sgn(cell.speedx) * cell.speedy * beta_rng(cell.alpha, cell.beta);
		cell.speedy = 0;
	}

	if (abs(cell.speedx) * cell.time_since_movedx >= 1)
	{
		float direction = sgn(cell.speedx);
		destx += direction;
		cell.speedx -= direction * Element::gravity * cell.friction * cell.time_since_movedx;
		if (cell.speedx * direction < 0)
			cell.speedx = 0;
		cell.time_since_movedx = 0;
	}
	if (abs(cell.speedy) * cell.time_since_movedy >= 1)
	{
		desty += sgn(cell.speedy);
		cell.time_since_movedy = 0;
	}
	
	if (destx != x || desty != y) {
		if (getParticle(destx,desty).isEmpty())
		{
			currentSquare->requestSwap(x, y, destx, desty, currentSquare);
			map::alert(x, y);
		}
		else
		{
			cell.speedx = 0;
			cell.speedy = 0;
		}
	}

	if (cell.speedy * delta > 1)
	{
		cell.speedy = 1.f / delta;
	}
	if (abs(cell.speedx) * delta > 1)
	{
		cell.speedx =sgn(cell.speedx) *  1.f / delta;
	}
}