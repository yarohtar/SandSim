#pragma once
#include <SFML/Graphics.hpp>

#define PI 3.14159265359f


enum { water_id, sand_id, empty_id, fire_id, out_id, ground_id };

class Element
{
protected:
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
	 bool is_empty;
	 bool is_solid;
	 bool is_liquid;
	 bool is_moveable;
	 bool is_out;
	 bool is_gas;
	 int element_id;

	 float speedx;
	 float speedy;

	//moveable solids
	 float time_since_movedx;
	 float time_since_movedy;
	float friction;
	 float alpha;
	 float beta;

	//lbm stuff
	 float df[9];
	 float feq[9];
	 float fcoll[9];
	 float rho;
	 float eps;
	 float rho0;
	 float mass;
	sf::Color color;

//funcitons
protected:
	Element();
	virtual ~Element();
	virtual sf::Color makeColor();
	virtual void applyHeat(float delta);
public:
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
	static const Element Water;
	static const Element Sand;
	static const Element Ground;
	static const Element Empty;
	static const Element Out;
};
