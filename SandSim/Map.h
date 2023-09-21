#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <stack>
#include <thread>
#include <barrier>
#include "Random.h"

#define nsqx 32
#define nsqy 32

class Square;
class Element;
class Out;

class Map
{
private:
	std::vector<std::vector<Square*>> squares;
	std::stack<int> freeIndices;
	//Square** squares = new Square*[nsqx];
	bool loaded[nsqx][nsqy];
	sf::Vector2i loaded_start;
	sf::Vector2i loaded_end;
	float width;
	float height;
	sf::Vector2f view_position;
	sf::Vector2f view_size;
	Element* defaultOutParticle;
	int num_threads;
	std::vector<std::thread*> threads;
	std::barrier<>* sync4;
	std::barrier<>* sync_update;
	float delta;
public:
	sf::VertexBuffer mapBuffer;
	unsigned long long frameCounter;


private:
	bool getSquareCoords(float x, float y, sf::Vector2i &v);
	void LoadSquare(int i, int j, sf::Vector2i& ul);
	void UnloadSquare(int i, int j);
	void update4(float delta, int istart, int jstart);
	void thread_update_all(int thread_id, Random r);
	void thread_update_batch(int nx, int ny, int sx, int sy, int start, int end, int x2, int y2);
	void alert_surrounding(int i, int j);
public:
	Map();
	void LoadEmpty();

	Element &operator()(float x, float y);

	void setViewPosition(float x, float y);
	void setViewSize(float x, float y);
	void setViewPosition(sf::Vector2f xy);
	void setViewSize(sf::Vector2f xy);

	void replaceParticle(float x, float y, Element& p);
	void swap(float x1, float y1, float x2, float y2);
	void updateParticle(float x, float y);
	void swapWithTrail(float x1, float y1, float x2, float y2, sf::Color& c);
	void destroyParticle(float x, float y);
	Element& getParticle(float x, float y);

	void update(float delta);
	void draw(sf::RenderWindow* window);
};
