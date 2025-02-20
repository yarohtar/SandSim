#pragma once
#include <SFML/Graphics.hpp>

class Element;

class Cell
{
protected:
	bool is_empty;
	bool is_solid;
	bool is_liquid;
	bool is_moveable;
	bool is_out;
	bool is_gas;
	int element_id;

	float rho0;
public:
	float speedx;
	float speedy;
	sf::Color color;

	//moveable solids
	float time_since_movedx;
	float time_since_movedy;
	float friction;
	float alpha;
	float beta;

	//lbm 
	float df[9];
	float feq[9];
	float fcoll[9];
	float eps;
	float mass;
	float rho;
public:
	Cell();
	Cell(const Element& e);
	void specialize(const Element& e);

	void deriveQuantities();
	void get_feq();
	void get_eps();
	void collision(float delta, float omega);

	void convertToEmpty();
	void convertToInterface();
	void convertToLiquid();

	bool isEmpty() const;
	bool isOut() const;
	bool isLiquid() const;
	bool isSolid() const;
	bool isMoveable() const;
	bool isGas() const;
};

