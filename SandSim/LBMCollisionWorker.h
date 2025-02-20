#pragma once
#include "GameWorker.h"

class LBMCollisionWorker : GameWorker
{
public:
	LBMCollisionWorker();
protected:
	virtual void UpdateCell(float x, float y, float delta);
};
