#include "Square.h"
#include "Map.h"
#include "Elements.h"

Square::Square(float x, float y)
{
	this->x = x;
	this->y = y;
	particles.setPrimitiveType(sf::Quads);
	particles.resize(4 * sqdim * sqdim);
	mapLocal.resize(sqdim);
	for (int i = 0; i < sqdim; i++)
	{
		mapLocal[i].resize(sqdim);
		for (int j = 0; j < sqdim; j++) {
			mapLocal[i][j] = new Empty(x + i, y + j, this);
		}
	}
	for (int i = 0; i < sqdim; i++)
	{
		for (int j = 0; j < sqdim; j++)
		{
			updated[i][j] = false;
		}
	}
	primes = { 3,5,7,11,13,17,19,23,29,31,37,41,43,47 };
	/*quad = new sf::Vertex[4];
	quad[0] = *new sf::Vertex();
	quad[1] = *new sf::Vertex();
	quad[2] = *new sf::Vertex();
	quad[3] = *new sf::Vertex();*/
	updateNextFrame = true;
	r = new Random();
}

void Square::initEmpty()
{
	for (int i = 0; i < sqdim; i++)
	{
		for (int j = 0; j < sqdim; j++)
		{
			sf::Vertex* quad = &particles[((i * sqdim) + j) * 4];
			quad[0].position = quad[1].position = quad[2].position = quad[3].position = sf::Vector2f(x + i, y + j);
			quad[1].position += sf::Vector2f(0, 1);
			quad[2].position += sf::Vector2f(1, 1);
			quad[3].position += sf::Vector2f(1, 0);
			quad[0].color = quad[1].color = quad[2].color = quad[3].color = mapLocal[i][j]->color;
			updated[i][j] = false;
			updateNextFrame = true;
		}
	}
}

void Square::updateVertices(Map& map)
{
	for (int i = 0; i < sqdim; i++)
	{
		for (int j = 0; j < sqdim; j++)
		{
			if (updated[i][j]) {
				updateVertex(i, j);
				//updateVertex(i, j, map);
			}
		}
	}
}

void Square::updateVertex(int i, int j)
{
	sf::Vertex* quad = &particles[((i * sqdim) + j) * 4];
	quad[0].position = quad[1].position = quad[2].position = quad[3].position = sf::Vector2f(x+i, y+j);
	quad[1].position += sf::Vector2f(0, 1);
	quad[2].position += sf::Vector2f(1, 1);
	quad[3].position += sf::Vector2f(1, 0);
	quad[0].color = quad[1].color = quad[2].color = quad[3].color = mapLocal[i][j]->color;
	updated[i][j] = false;
	updateNextFrame = true;
}

/*void Square::updateVertex(int i, int j, Map& map)
{
	quad[0].position = quad[1].position = quad[2].position = quad[3].position = sf::Vector2f(x + i, y + j);
	quad[1].position += sf::Vector2f(0, 1);
	quad[2].position += sf::Vector2f(1, 1);
	quad[3].position += sf::Vector2f(1, 0);
	quad[0].color = quad[1].color = quad[2].color = quad[3].color = mapLocal[i][j]->color;
	map.mapBuffer.update(quad, 4, bufferStartIndex + ((i * sqdim) + j) * 4);
	updated[i][j] = false;
	updateNextFrame = true;
}*/

/*void Square::initBuffer(Map& map) //can be faster -> update whole buffer at once
{
	for (int i = 0; i < sqdim; i++)
	{
		for (int j = 0; j < sqdim; j++)
		{
			// = &(map->mapBuffer[bufferStartIndex + ((i * sqdim) + j) * 4]);

			quad[0].position = quad[1].position = quad[2].position = quad[3].position = sf::Vector2f(x + i, y + j);
			quad[1].position += sf::Vector2f(0, 1);
			quad[2].position += sf::Vector2f(1, 1);
			quad[3].position += sf::Vector2f(1, 0);
			quad[0].color = quad[1].color = quad[2].color = quad[3].color = mapLocal[i][j]->color;
			map.mapBuffer.update(quad, 4, bufferStartIndex + ((i * sqdim) + j) * 4);
			updated[i][j] = false;
			updateNextFrame = true;
		}
	}
}*/


Element& Square::getParticle(float x, float y)
{
	x -= this->x;
	y -= this->y;
	return *mapLocal[(int)x][(int)y];
}

void Square::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	states.texture = NULL;
	target.draw(particles, states);
}

float Square::rng()
{
	return r->rng();
}

void Square::updatedParticle(int i, int j)
{
	updated[i][j] = true;
	updateNextFrame = true;
	updatedThisFrame = true;
}

void Square::setParticle(Element& p, int i, int j)
{
	p.setPosition(x+i, y+j);
	p.setCurrentSquare(this);
	mapLocal[i][j] = &p;
	updated[i][j] = true;
	updateNextFrame = true;
	updatedThisFrame = true;
}

void Square::replaceParticle(Element& p, int i, int j)
{
	delete mapLocal[i][j];
	setParticle(p, i, j);
}

Element& Square::operator()(float x, float y)
{
	return getParticle(x, y);
}

void Square::update(float delta, Map& map)
{
	int pi = (primes.size()-1) * rng();
	int p = primes[pi];
	while (sqdim % p == 0)
	{
		pi++;
		pi %= (primes.size() - 1);
		p = primes[pi];
	}
	updateNextFrame = false;
	for (int i = 0; i < sqdim; i++)
	{
		int i1 = i * p;
		i1 %= sqdim;
		for (int j = 0; j < sqdim; j++)
		{
			int j1 = j * p;
			j1 %= sqdim;
			if(!updated[i1][j1])
				mapLocal[i1][j1]->update(delta, map);
		}
	}
	//updateVertices(map);
}