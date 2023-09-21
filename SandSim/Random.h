#pragma once
#include<cmath>

class Random
{
private:
	unsigned int seed;
private:
	Random(unsigned int seed);
	unsigned int rngi();
public:
	Random();
	float rng();
	float gamma(unsigned int a);
	float beta(unsigned int a, unsigned int b);
	Random* makeRandom();
//static
private:
	static unsigned int seed_priv;
private:
	static unsigned int rng_priv();
};

