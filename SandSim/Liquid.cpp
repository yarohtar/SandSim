#include "Liquid.h"


namespace {
	double ex[9] = { 0, 1, 0, -1, 0, 1, -1, -1, 1 };
	double ey[9] = { 0, 0, 1, 0, -1, 1, 1, -1, -1 };
	double w[9] = { 4.0 / 9.0, 
		1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0,
		1.0 / 36.0, 1.0 / 36.0, 1.0 / 36.0, 1.0 / 36.0 };
	int invDir[9] = {0, 3, 4, 1, 2, 7, 8, 5, 6};
	double gravity = 0.005;
	double omega = 0.05;
}

void Liquid::deriveQuantities()
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

void Liquid::get_equib()
{
	double u2 = speedx * speedx + speedy * speedy;
	for (int i = 0; i < 9; i++)
	{
		double eiu = ex[i] * speedx + ey[i] * speedy;
		feq[i] = w[i] * rho * 
			(1 + eiu * invc2 + eiu * eiu * (0.5 * invc2 * invc2) - u2 * (0.5 * invc2));
	
	}
}

void Liquid::collision(float delta)
{
	get_equib();
	for (int i = 0; i < 9; i++)
	{
		fcoll[i] = (1 - omega) * df[i] + omega * feq[i];
	}
	fcoll[2] += rho * w[2] * gravity;
	fcoll[4] -= rho * w[4] * gravity;
}

void Liquid::get_eps()
{
	if (!is_interface)
	{
		eps = 1;
		return;
	}
	eps = rho / rho0;
}

void Liquid::stream()
{
	if (!is_interface)
	{
		for (int i = 1; i < 9; i++)
		{

		}
		return;
	}
	
}

Liquid::Liquid() : Element()
{
	pushPressure = 0;
	for (int i = 0; i < 9; i++) {
		df[i] = w[i];
	}
	d0 = 0.5;
	tau = 2;
	c2 = 0.33333;
	invc2 = 1.0 / c2;
	last_frame_updated = 0;
	rho0 = 1;
	rho = 1;
	deformation_coef = 0.1;
	is_interface = false;
}

void Liquid::make_empty()
{
	for (int i = 0; i < 9; i++) {
		df[i] = 0;
		future_liq_density[i] = 0;
	}
}

Liquid* Liquid::make_interface()
{
	Liquid l;
	l.is_interface = true;
	l.make_empty();
	return nullptr;
}


double Liquid::force()
{
	return deformation_coef*(rho0-rho);
}

