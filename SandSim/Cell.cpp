#include "Cell.h"
#include "Element.h"


void Cell::specialize(const Element& e)
{
	element_id = e.element_id;
	is_empty = e.is_empty;
	is_solid = e.is_solid;
	is_liquid = e.is_liquid;
	is_moveable = e.is_moveable;
	is_out = e.is_out;
	is_gas = e.is_gas;

	//make color here instead of just copying 
	color = e.color;

	if (is_empty)
		return;
	
	mass = e.mass;

	if (is_moveable) 
	{
		speedx = e.speedx;
		speedy = e.speedy;
		//moveable solids
		if (is_solid) 
		{
			time_since_movedx = e.time_since_movedx;
			time_since_movedy = e.time_since_movedy;

			alpha = e.alpha;
			beta = e.beta;
			friction = e.friction;
		}
	}

	//lbm stuff
	if (is_liquid)
	{
		for (int i = 0; i < 9; i++)
		{
			df[i] = e.df[i];
			feq[i] = e.feq[i];
			fcoll[i] = e.fcoll[i];
		}
		rho0 = e.rho0;
		rho = e.rho;
		eps = e.eps;
	}

}


Cell::Cell()
{
	specialize(Element::Empty);
}

Cell::Cell(const Element& e)
{
	specialize(e);
}

namespace {
	double ex[9] = { 0, 1, 0, -1, 0, 1, -1, -1, 1 };
	double ey[9] = { 0, 0, 1, 0, -1, 1, 1, -1, -1 };
	double w[9] = { 4.0 / 9.0, 
		1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0,
		1.0 / 36.0, 1.0 / 36.0, 1.0 / 36.0, 1.0 / 36.0 };
	int invDir[9] = {0, 3, 4, 1, 2, 7, 8, 5, 6};
	double gravity = 0.005;
}

void Cell::deriveQuantities()
{
	rho = 0;
	for (int i = 0; i < 9; i++)
	{
		rho += df[i];
	}
	speedx = 0;
	speedy = 0;
	for (int i = 1; i < 9; i++)
	{
		speedx += df[i] * ex[i];
		speedy += df[i] * ey[i];
	}
	speedx = speedx / rho;
	speedy = speedy / rho;
}

void Cell::get_feq()
{
	double u2 = speedx * speedx + speedy * speedy;
	for (int i = 0; i < 9; i++)
	{
		double eiu = ex[i] * speedx + ey[i] * speedy;
		//feq[i] = w[i] * rho * 
			//(1 + eiu * invc2 + eiu * eiu * (0.5 * invc2 * invc2) - u2 * (0.5 * invc2));
	
	}
}

void Cell::collision(float delta, float omega)
{
	deriveQuantities();
	get_feq();
	for (int i = 0; i < 9; i++)
	{
		fcoll[i] = (1 - omega) * df[i] + omega * feq[i];
		fcoll[i] += rho * w[i] * ey[i] * gravity;
	}
}

void Cell::get_eps()
{
	//if (!is_interface)
	{
		eps = 1;
		return;
	}
	eps = rho / rho0;
}


bool Cell::isEmpty() const
{
	return is_empty;
}
bool Cell::isOut() const
{
	return is_out;
}
bool Cell::isLiquid() const
{
	return is_liquid;
}
bool Cell::isGas() const
{
	return is_gas;
}
bool Cell::isSolid() const
{
	return is_solid;
}
bool Cell::isMoveable() const
{
	return is_moveable;
}
