#pragma once
#include "Element.h"
class Liquid : public Element
{
protected:
	int dontUpdate;
	bool hasBeenPushed;
	unsigned long long pushFrame;
protected:
	virtual bool isLiquid();
	virtual bool isMoveable();
public:
	Liquid(float x, float y, Square* currentSquare);
	virtual void update(float delta, Map& map);
	virtual void preUpdate(float delta, Map& map);
	void getPressure(Map& map);
	void getPushPressure(Map& map);
	void push(float x, float y, float p, Map& m);
	void receive_push(float p, unsigned long long pushFrame);
};
