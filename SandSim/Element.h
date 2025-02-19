#pragma once
#include <SFML/Graphics.hpp>

#define PI 3.14159265359f


enum { water_id, sand_id, empty_id, fire_id, out_id, ground_id };

class Element
{
protected:
	bool is_empty;
	bool is_solid;
	bool is_liquid;
	bool is_moveable;
	bool is_out;
	bool is_gas;
	int element_id;
	float mass;
	float energy;
	bool isFreeFalling;
	bool isActive;
	int health;
	int heat;
	int heatResistance;
	int heatEmitTreshold;
	int corrosionResistance;
	unsigned long long last_frame_updated;
public:
	float time_since_movedx;
	float time_since_movedy;
	float speedx;
	float speedy;
	float friction;
	float alpha;
	float beta;
	float pressure;
	float pushPressure;
	sf::Color color;

//funcitons
protected:
	virtual sf::Color makeColor();
	virtual void applyHeat(float delta);
public:
	Element();
	virtual ~Element();

	bool isEmpty();
	bool isOut();
	bool isLiquid();
	bool isSolid();
	bool isMoveable();
	bool isGas();
	virtual bool isFakeLiquid();

	virtual bool canMoveThrough(Element& e);

	virtual void receiveHeat(int receivedHeat, float delta);
	virtual void receiveCorrosion();
	virtual void push(float x, float y, float p);
	virtual void receive_push(float p, unsigned long long pushFrame);
	virtual void receive_liquid_density(int i, float f);

	//static
public:
	static float gravity;
};
