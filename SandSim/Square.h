#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include "Random.h"

#include<thread>
#include <mutex>

class Element;

const int sqdim = 16;

class Square : public sf::Drawable, public sf::Transformable
{
private:
	std::vector<std::vector<Element*>> mapLocal;
	sf::VertexArray particles;
	void updateVertex(int i, int j);
	std::vector<int> primes;
	Random* r;
	sf::RectangleShape* rect;
	std::vector<std::tuple<sf::Vector2i, sf::Vector2i, Square*>> swap_requests;
	std::mutex swap_mutex;

	int futureLazyStartX;
	int futureLazyEndX;
	int futureLazyStartY;
	int futureLazyEndY;
	//sf::Vertex* quad; //array size 4
public:
	int lazyStartX;
	int lazyEndX;
	int lazyStartY;
	int lazyEndY;

	float x;
	float y;
	bool updateNextFrame;
	bool updatedThisFrame;
	int bufferStartIndex;
	bool updated[sqdim][sqdim];

	float swapTime = 0;

private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	float rng();
public:
	Square(float x, float y);
	void initEmpty();
	bool inBounds(float x, float y);
	Element& getParticle(float x, float y);
	void setParticle(Element& p, float x, float y);
	void updatedParticle(int i, int j);
	void replaceParticle(Element& p, int i, int j);
	Element& operator()(float x, float y);
	void updateVertices();
	void handle_swaps();
	void requestSwap(float x1, float y1, float x2, float y2, Square* sq);
	void alert(float x, float y);
	void updateLazyRectangle();
	void lb_stream(float delta);
	void lb_collision(float delta);
	void lb_interface();
};
