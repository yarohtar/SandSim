#include "Random.h"
#include <random>

unsigned int Random::seed_priv = 0;

Random::Random()
{
	seed = rng_priv();
}

Random::Random(unsigned int seed)
{
	this->seed = seed;
}

float Random::rng()
{
	seed = seed * 747796405 + 2891336453;
	unsigned int result = ((seed >> ((seed >> 28) + 4)) ^ seed) * 277803737;
	result = (result >> 22) ^ result;
	return result / 4294967295.0;
}

float Random::gamma(unsigned int a)
{
	float s = 1;
	for(int i = 0; i < a; i++)
	{
		float r = rng();
		while (r == 0)
			r = rng();
		s *= r;
	}
	return -std::log(s);
}

float Random::beta(unsigned int a, unsigned int b)
{
	float x = gamma(a);
	float y = gamma(b);
	return x / (x + y);
}

template<class It>
void Random::random_shuffle(It start, It end)
{
	std::shuffle(start, end, std::default_random_engine(seed));
}

Random* Random::makeRandom()
{
	return new Random(rngi());
}

unsigned int Random::rngi()
{
	seed = seed * 747796405 + 2891336453;
	unsigned int result = ((seed >> ((seed >> 28) + 4)) ^ seed) * 277803737;
	result = (result >> 22) ^ result;
	return result;
}


unsigned int Random::rng_priv()
{
	seed_priv = seed_priv * 747796405 + 2891336453;
	unsigned int result = ((seed_priv >> ((seed_priv >> 28) + 4)) ^ seed_priv) * 277803737;
	result = (result >> 22) ^ result;
	return result;
}