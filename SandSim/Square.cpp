#include "Square.h"
#include "Elements.h"

Square::Square(float x, float y)
{
	this->x = round(x);
	this->y = round(y);
	particles.setPrimitiveType(sf::Quads);
	particles.resize(4 * sqdim * sqdim);
	mapLocal.resize(sqdim);
	for (int i = 0; i < sqdim; i++)
	{
		mapLocal[i].resize(sqdim);
		for (int j = 0; j < sqdim; j++) {
			mapLocal[i][j] = new Empty();
		}
	}
	for (int i = 0; i < sqdim; i++)
	{
		for (int j = 0; j < sqdim; j++)
		{
			updated[i][j] = false;
		}
	}
	updateNextFrame = true;
	r = new Random();
	rect = new sf::RectangleShape();

	lazyStartX = 0;
	lazyStartY = 0;
	lazyEndX = sqdim;
	lazyEndY = sqdim;
	futureLazyStartX = sqdim;
	futureLazyStartY = sqdim;
	futureLazyEndX = -1;
	futureLazyEndY = -1;
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

void Square::updateVertices()
{
	if (!updatedThisFrame)
		return;
	for (int i = 0; i < sqdim; i++)
	{
		for (int j = 0; j < sqdim; j++)
		{
			if (updated[i][j]) {
				updateVertex(i, j);
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

bool Square::inBounds(float x, float y)
{
	return x >= this->x && y >= this->y && x <= this->x + sqdim - 1 && y <= this->y + sqdim - 1;
	x = round(x);
	y = round(y);
	if (x < this->x || y<this->y || x>this->x+sqdim-1 || y>this->y+sqdim-1)
		return false;
	return true;
}

Element& Square::getParticle(float x, float y)
{
	x = round(x);
	y = round(y);
	x -= this->x;
	y -= this->y;
	if (x < 0)
		x = 0;
	if (y < 0)
		y = 0;
	return *mapLocal[(int)x][(int)y];
}

void Square::updatedParticle(int i, int j)
{
	updated[i][j] = true;
	updateNextFrame = true;
	updatedThisFrame = true;
}

void Square::setParticle(Element& p, float x, float y)
{
	int i = round(x - this->x);
	int j = round(y - this->y);
	mapLocal[i][j] = &p;
	updated[i][j] = true;
	updateNextFrame = true;
	updatedThisFrame = true;
	map::alert(x, y);
}

void Square::replaceParticle(Element& p, int i, int j)
{
	delete mapLocal[i][j];
	setParticle(p, x+i, y+j);
}

Element& Square::operator()(float x, float y)
{
	return getParticle(x, y);
}

void Square::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	states.texture = NULL;
	target.draw(particles, states);
//	return;
	if (lazyEndX != -1) {
		rect->setPosition({ x + lazyStartX, y + lazyStartY });
		rect->setSize({ (float)lazyEndX - lazyStartX, (float)lazyEndY - lazyStartY });
		rect->setFillColor(sf::Color::Transparent);
		rect->setOutlineColor(sf::Color::Green);
		rect->setOutlineThickness(.5);
		target.draw(*rect, states);
	}
}

float Square::rng()
{
	return r->rng();
}

void Square::requestSwap(float x1, float y1, float x2, float y2, Square* sq)
{
	x1 = round(x1);
	y1 = round(y1);
	x2 = round(x2);
	y2 = round(y2);
	if (!inBounds(x2, y2))
	{
		map::requestSwap(x1, y1, x2, y2, sq);
		return;
	}
	std::unique_lock lock(swap_mutex);
	swap_requests.push_back(std::tuple<sf::Vector2i, sf::Vector2i,Square*>({ (int)x1,(int)y1 }, { (int)x2,(int)y2 }, sq));
}

void Square::handle_swaps()
{
	sf::Clock clock;
	std::sort(swap_requests.begin(), swap_requests.end(),
		[](std::tuple<sf::Vector2i, sf::Vector2i, Square*>& a, std::tuple<sf::Vector2i, sf::Vector2i, Square*>& b) 
		{ if (std::get<1>(a).x != std::get<1>(b).x) return std::get<1>(a).x < std::get<1>(b).x;
			return std::get<1>(a).y < std::get<1>(b).y;});
	swap_requests.push_back(std::tuple<sf::Vector2i, sf::Vector2i, Square*>({ 0,0 }, { 0,0 },nullptr));
	int prev = 0;
	for (int i = 0; i < swap_requests.size() - 1; i++)
	{
		if (std::get<1>(swap_requests[i + 1]) != std::get<1>(swap_requests[i]))
		{
			float s = rng();
			int ind = s * (i - prev+1) + prev;
			if (ind > i)
				ind = i;
			auto [origin, dest, sq] = swap_requests[ind];
			Element& p = getParticle(dest.x, dest.y);
			Element& q = sq->getParticle(origin.x, origin.y);
			sq->setParticle(p, origin.x, origin.y);
			setParticle(q, dest.x, dest.y);
			prev = i + 1;
		}
	}
	swap_requests.clear();
	swapTime = clock.restart().asSeconds();
}

void Square::lb_stream(float delta)
{
	for (int i = 0; i < sqdim; i++)
	{
		for (int j = 0; j < sqdim; j++)
		{
			if (!mapLocal[i][j]->isLiquid())
				continue;

		}
	}
}

void Square::alert(float x, float y)
{
	x -= this->x;
	y -= this->y;
	int xi = lround(x);
	int yi = lround(y);
	
	if (xi < futureLazyStartX)
		futureLazyStartX = std::max(0, xi);
	if (xi+1 > futureLazyEndX)
		futureLazyEndX = std::min(xi+1, sqdim);
	if (yi < futureLazyStartY)
		futureLazyStartY = std::max(0, yi);
	if (yi+1 > futureLazyEndY)
		futureLazyEndY = std::min(yi + 1, sqdim);
}
void Square::updateLazyRectangle()
{
	lazyStartX = futureLazyStartX;
	lazyStartY = futureLazyStartY;
	lazyEndX = futureLazyEndX;
	lazyEndY = futureLazyEndY;

	futureLazyStartX = sqdim;
	futureLazyStartY = sqdim;
	futureLazyEndX = -1;
	futureLazyEndY = -1;
}

void Square::lb_collision(float delta)
{

}

void Square::lb_interface()
{

}
