#pragma once
#include "Element.h"
class Liquid : public Element
{
protected:
	double df[9];
	double feq[9];
	double fcoll[9];
	double speedx;
	double speedy;
	double rho0;
	double deformation_coef;
	double rho;
	double eps;
	double d0;
	double c2;
	double invc2;
	double tau;
	double future_liq_density[9];
	double f_stream[9];
	double f_stream_future[9];
	unsigned long long received_liq_density_frame[9];
	bool is_interface;
protected:
	virtual double force();
public:
	Liquid();
	void make_empty();
private:
	Liquid* make_interface();
	void get_equib();
	void collision(float delta);
	void get_eps();
	void stream();
	void deriveQuantities();
};
