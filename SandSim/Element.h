#pragma once
#include <SFML/Graphics.hpp>
#include "Random.h"

#define PI 3.14159265359f

class Map;
class Square;

enum { water_id, sand_id, empty_id, fire_id, out_id, ground_id };

class Element
{
private:
	Random* r;
protected:
	int element_id;
	float x;
	float y;
	float mass;
	float energy;
	int dirx;
	int diry;
	bool isFreeFalling;
	bool isActive;
	int health;
	int heat;
	int heatResistance;
	int heatEmitTreshold;
	int corrosionResistance;
	Square* currentSquare;
	unsigned long long last_frame_updated;
public:
	float pressure;
	float pushPressure;
	sf::Color color;

//funcitons
protected:
	float rng();
	float beta_rng(unsigned int a, unsigned int b);
	void moveTo(float x2, float y2, Map& m);
	void moveInDir(float dirx, float diry, int n, Map& m);
	virtual bool swapWith(float x2, float y2, Map& m);

	virtual sf::Color makeColor();

	virtual void preUpdate(float delta, Map& map);
	virtual void postUpdate(float delta, Map& map);
	

	//bool isSurrounded(); maybe not virtual??
	//bool isSurroundedBy(); // other combos

	virtual void applyImpulse(Map& map);
	virtual void applyHeat(float delta, Map& map);
public:
	Element(float x, float y, Square* currentSquare, Random* rand = new Random());
	virtual ~Element();
	virtual void update(float delta, Map& map);
	void setPosition(float x, float y);
	void setCurrentSquare(Square* square);
	virtual double force();

	virtual bool isEmpty();
	virtual bool isOut();
	virtual bool isLiquid();
	virtual bool isSolid();
	virtual bool isMoveable();
	virtual bool isGas();
	virtual bool isFakeLiquid();

	virtual bool canMoveThrough(Element& e);

	virtual void receiveImpulse();
	virtual void receiveHeat(int receivedHeat, float delta);
	virtual void receiveCorrosion();
	virtual void push(float x, float y, float p, Map& m);
	virtual void receive_push(float p, unsigned long long pushFrame);
	virtual void receive_liquid_density(int i, float f, Map& map);

	//static
public:
	static float gravity;
};
