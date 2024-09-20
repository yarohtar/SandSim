#pragma once
#include "Element.h"
class Liquid : public Element
{
protected:
	int dontUpdate;
	bool hasBeenPushed;
	unsigned long long pushFrame;

	double liq_density[9];
	double f_equib[9];
	double speedx;
	double speedy;
	double rho0;
	double deformation_coef;
	double rho;
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
	virtual bool isLiquid();
	virtual bool isMoveable();
	virtual Liquid* specialize();
	virtual double force();
public:
	Liquid(float x, float y, Square* currentSquare);
	virtual void update(float delta, Map& map);
	//virtual void preUpdate(float delta, Map& map);
	virtual void receive_liquid_density(int i, float p, Map& map);
	void make_empty();
private:
	void interface_update(float delta, Map& map);
	Liquid* make_interface();
	void surround_interface(Map& m);
	void get_equib(float delta, Map& m);
	void get_eq(double rho, double ux, double uy, double feq[]);
	void get_speed();
	void get_rho();
};
