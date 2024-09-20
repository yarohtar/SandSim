#include "Liquid.h"
#include "Map.h"
#include "Square.h"
#include "FakeLiquid.h"


double ex[9] = { 0, 1, 0, -1, 0, 1, -1, -1, 1 };
double ey[9] = { 0, 0, 1, 0, -1, 1, 1, -1, -1 };
double w[9] = { 4.0 / 9.0, 1.0 / 9.0,
	1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0,
	1.0 / 36.0, 1.0 / 36.0, 1.0 / 36.0, 1.0 / 36.0 };

Liquid::Liquid(float x, float y, Square* currentSquare) : Element(x, y, currentSquare)
{
	dontUpdate = 0;
	hasBeenPushed = false;
	pushPressure = 0;
	for (int i = 0; i < 9; i++) {
		liq_density[i] = w[i];
		future_liq_density[i] = w[i];
	}
	d0 = 0.5;
	tau = 2;
	c2 = 90000;
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
		liq_density[i] = 0;
		future_liq_density[i] = 0;
	}
}

Liquid* Liquid::make_interface()
{
	Liquid l(0,0,nullptr);
	l.is_interface = true;
	l.make_empty();
	return nullptr;
}

void Liquid::surround_interface(Map& m)
{
	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			if(m(x+i, y+i).isEmpty())
				m.replaceParticle(x + i, y + j, *make_interface());
		}
	}
}

bool Liquid::isLiquid()
{
	return true;
}

bool Liquid::isMoveable()
{
	return true;
}

Liquid* Liquid::specialize()
{
	return nullptr;
}

double Liquid::force()
{
	return deformation_coef*(rho0-rho);
}

void Liquid::get_rho()
{
	rho = 0;
	for(int i=0; i<9; i++)
	{
		rho += liq_density[i];
	}
}


void Liquid::get_speed()
{
	speedx = 0;
	speedy = 0;
	for (int i = 1; i < 9; i++)
	{
		speedx += liq_density[i] * ex[i];
		speedy += liq_density[i] * ey[i];
	}
	//speedx /= rho;
	//speedy /= rho;
}

void Liquid::get_equib(float delta, Map& m)
{
	get_rho();
	get_speed();
	double u2 = speedx * speedx + speedy * speedy;
	for (int i = 0; i < 9; i++)
	{
		double eiu = ex[i] * speedx + ey[i] * speedy;
		f_equib[i] = w[i] * (rho
			+ eiu / c2
			+ eiu * eiu / (2.0 * c2 * c2)
			- u2 / (2.0 * c2));

	}
}

void Liquid::get_eq(double rho, double ux, double uy, double feq[])
{
	double u2 = ux * ux + uy * uy;
	for (int i = 0; i < 9; i++)
	{
		double eiu = ex[i] * ux + ey[i] * uy;
		feq[i] = w[i] * 
			(rho +
			eiu * invc2 +
			0.5 * eiu * eiu * invc2 * invc2
			- 0.5 * u2 * u2);
	}
}

int opp[9] = { 0, 3, 4, 1, 2, 7, 8, 5, 6 };

int oppI(int i) ///optimize
{
	if (i == 0)
		return 0;
	if (i < 5)
		return (i + 1) % 4 + 1;
	return (i - 3) % 4 + 5;
}

void Liquid::interface_update(float delta, Map& map)
{
	double feq_air[9];
	get_speed();
	get_eq(rho, speedx, speedy, feq_air);
	double feq_interface[9];
	get_eq(rho, speedx, speedy, feq_interface);

	for (int i = 0; i < 9; i++)
	{
		if(map(x+ex[i], y+ey[i]).isEmpty())
			f_stream_future[opp[i]] = feq_air[i] + feq_air[opp[i]] - f_stream[i];
		else if (map(x + ex[i], y + ey[i]).isLiquid())
		{

		}
	}
}

void Liquid::update(float delta, Map& map)
{
	if (last_frame_updated != 0) {
		for (int i = 0; i < 9; i++)
		{
			if (received_liq_density_frame[i] == map.frameCounter)
				liq_density[i] = future_liq_density[i];
			else if(received_liq_density_frame[i]<map.frameCounter)
				liq_density[i] = 0;
		}
	}
	last_frame_updated = map.frameCounter;

	get_equib(delta, map);
	color.a = 255 * (0.1 + rho / 2);
	if (rho > 1.1)
		color = sf::Color(255, 255, 255, 255);
	if (rho < 0)
		color = sf::Color(255, 0, 0, 255);
	map.updateParticle(x, y);
	currentSquare->updateNextFrame = true;
	if (rho < 0.000001)
		return;

	
	double Fx = 0, Fy = 0;
	for (int i = 1; i < 9; i++)
	{
		Element& e = map(x + ex[i], y + ey[i]);
		double f = map(x + ex[i], y + ey[i]).force();
		Fx += f * ex[i];
		Fy += f * ey[i];
	}

	double f1[9];

	liq_density[0] = f1[0] = liq_density[0] - (liq_density[0] - f_equib[0]) / tau;

	for (int i = 1; i < 9; i++)
	{
		liq_density[i] = f1[i] = liq_density[i] - (liq_density[i] - f_equib[i]) / tau;
	}



	for (int i = 0; i < 9; i++)
	{
		int x1 = x + ex[i];
		int y1 = y + ey[i];
		if (map(x1, y1).isLiquid())
		{
			Element& l = map(x1, y1);
			l.receive_liquid_density(i, f1[i], map);
		}
		else if (map(x1,y1).isEmpty() && rho > 0.5)
		{
			Liquid* l = specialize();
			map.replaceParticle(x1, y1, *l);
			l->make_empty();
			l->update(delta, map);
			l->receive_liquid_density(i, f1[i], map);
		}
		else
		{
			int j;
			if (i < 5)
				j = (i + 1) % 4 + 1;
			else
				j = (i - 3) % 4 + 5;
			receive_liquid_density(j, f1[i], map);

		}
	}

	return;

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
}



void Liquid::receive_liquid_density(int i, float p, Map& map)
{
	if (p < 0)
		std::cout << "NOOOO " << p << std::endl;
	unsigned long long frame = map.frameCounter;
	if (frame == last_frame_updated)
	{
		future_liq_density[i] = liq_density[i]= p;
		received_liq_density_frame[i] = frame+1;
		return;
	}
	liq_density[i] = future_liq_density[i];
	received_liq_density_frame[i] = frame+1;
	future_liq_density[i] = p;
}
