#pragma once
#include <SFML/Graphics.hpp>

class Cell;
class Square;

namespace map
{
	extern sf::VertexBuffer mapBuffer;
	extern unsigned long long frameCounter;

	void initialize();
	void loadEmpty();

	Cell& grid(float a, float b);
	void alert(float x, float y);

	void setViewPosition(float x, float y);
	void setViewSize(float x, float y);
	void setViewPosition(sf::Vector2f xy);
	void setViewSize(sf::Vector2f xy);

	void replaceParticle(float x, float y, Cell& p);
	void requestSwap(float x1, float y1, float x2, float y2, Square* origin);
	void swap(float x1, float y1, float x2, float y2);
	void swapWithTrail(float x1, float y1, float x2, float y2, sf::Color& c);

	void update(float delta);
	void draw(sf::RenderWindow* window);

};