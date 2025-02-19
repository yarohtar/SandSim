#pragma once
#include "GameWorker.h"

class Square;

class MoveableSolidWorker : public GameWorker
{
public:
	MoveableSolidWorker(Square* s);

protected:
	virtual void UpdateCell(float x, float y, float delta);
};
