#pragma once

class Square;
class Element;
class Random;

class GameWorker
{
protected:
	Square* currentSquare;
	Random* r;
	virtual void UpdateCell(float x, float y, float delta) = 0;
	
public:
	GameWorker(Square*);
	void UpdateSquare(float delta);
	
protected:
	bool inBounds(float, float);
	Element& getParticle(float, float);
	float rng();
	float beta_rng(unsigned int a, unsigned int b);

	int getIndex(float, float);
	Element& getParticleByIndex(int);
};
