#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include "Random.h"

class Map;
class Element;

const int sqdim = 16;

class Square : public sf::Drawable, public sf::Transformable
{
private:
	std::vector<std::vector<Element*>> mapLocal;
	sf::VertexArray particles;
	void updateVertex(int i, int j);
	void updateVertex(int i, int j, Map& map);
	std::vector<int> primes;
	Random* r;
	//sf::Vertex* quad; //array size 4
public:
	float x;
	float y;
	bool updateNextFrame;
	bool updatedThisFrame;
	int bufferStartIndex;
	bool updated[sqdim][sqdim];

private:
	Element& getParticle(float x, float y);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	float rng();
public:
	Square(float x, float y);
	void initEmpty();
	//void initBuffer(Map& map);
	void setParticle(Element& p, int i, int j);
	void updatedParticle(int i, int j);
	void replaceParticle(Element& p, int i, int j);
	Element& operator()(float x, float y);
	void update(float delta, Map& map);
	void updateVertices(Map& map);
};
