#include "Map.h"
#include "Square.h"
#include "Elements.h"
#include "algorithm"
#include "thread"

Map::Map()
{
	frameCounter = 0;
	width = sqdim * nsqx;
	height = sqdim * nsqy;
	defaultOutParticle = new Out();
	view_position = sf::Vector2f(0, 0);
	view_size = sf::Vector2f(480, 270);
	squares.resize(nsqx);
	for (int i = 0; i < nsqx; i++) {
		squares[i].resize(nsqy);
		for (int j = 0; j < nsqy; j++)
			loaded[i][j] = false;
	}
	int nx = std::min((int)view_size.x / sqdim + 3, nsqx);
	int ny = std::min((int)view_size.y / sqdim + 3, nsqy);

	mapBuffer.setPrimitiveType(sf::Quads);
	mapBuffer.create(4 * nx * ny * sqdim * sqdim);

	for (int i = 0; i < nx * ny; i++)
		freeIndices.push(i*sqdim*sqdim*4);
	loaded_start = sf::Vector2i(nsqx, nsqy);
	loaded_end = sf::Vector2i(0, 0);
	LoadEmpty();
	num_threads = std::thread::hardware_concurrency() - 1;
	threads.resize(num_threads);
	sync4 = new std::barrier(num_threads);
	sync_update = new std::barrier(num_threads + 1);
	for (int i = 0; i < num_threads; i++)
		threads[i] = new std::thread(&Map::thread_update_all, this, i, *new Random());
}

void Map::thread_update_all(int thread_id, Random r)
{
	while (true)
	{
		sync_update->arrive_and_wait();
		int sx = loaded_start.x;
		int sy = loaded_start.y;
		int nx = loaded_end.x - sx + 1;
		int ny = loaded_end.y - sy + 1;

		int batch = nx * ny / num_threads;
		int start = thread_id * batch;
		int end = start + batch;
		if (thread_id == num_threads - 1)
			end = nx * ny;
		thread_update_batch(nx, ny, sx, sy, start, end, 0, 0);
		sync4->arrive_and_wait();
		thread_update_batch(nx, ny, sx, sy, start, end, 0, 1);
		sync4->arrive_and_wait();
		thread_update_batch(nx, ny, sx, sy, start, end, 1, 0);
		sync4->arrive_and_wait();
		thread_update_batch(nx, ny, sx, sy, start, end, 1, 1);
		for (int iter = start; iter < end; iter++)
		{
			int i = sx + iter / ny;
			int j = sy + iter % ny;
			if (loaded[i][j] && squares[i][j]->updatedThisFrame) {
				squares[i][j]->updatedThisFrame = false;
				alert_surrounding(i, j);
				squares[i][j]->updateVertices(*this);
			}
		}
		sync_update->arrive_and_wait();
	}
}

void Map::alert_surrounding(int k, int l)
{
	for (int i = k - 1; i <= k + 1; i++)
	{
		for (int j = l - 1; j <= l + 1; j++)
		{
			if (i < 0 || j < 0 || i >= nsqx || j >= nsqy)
				continue;
			if (i == k && j == l)
				continue;
			if (loaded[i][j])
				squares[i][j]->updateNextFrame = true;
		}
	}
}

void Map::thread_update_batch(int nx, int ny, int sx, int sy, int start, int end, int x2, int y2)
{
	for (int iter = start; iter < end; iter++)
	{
		int i = sx + iter / ny;
		int j = sy + iter % ny;
		if (i%2 != x2 || j%2 != y2)
			continue;
		if (loaded[i][j] && squares[i][j]->updateNextFrame)
			squares[i][j]->update(delta, *this);
	}
}



void Map::LoadEmpty()
{
	sf::Vector2i ul;
	getSquareCoords(view_position.x, view_position.y, ul);
	if (ul.x > 0) ul.x--;
	if (ul.y > 0) ul.y--;
	int nx = std::min((int)((view_position.x + view_size.x - ul.x*sqdim) / sqdim) + 1, nsqx);
	int ny = std::min((int)((view_position.y + view_size.y - ul.y*sqdim) / sqdim) + 1, nsqy);

	for (int i = 0; i < nx; i++)
	{
		for (int j = 0; j < ny; j++)
		{
			LoadSquare(i,j,ul);
		}
	}
}

void Map::LoadSquare(int i, int j, sf::Vector2i& ul)
{
	squares[ul.x+i][ul.y+j] = new Square(ul.x * sqdim + i * sqdim, ul.y * sqdim + j * sqdim);
	loaded[ul.x+i][ul.y+j] = true;
	squares[ul.x+i][ul.y+j]->bufferStartIndex = freeIndices.top();
	freeIndices.pop();
	//squares[ul.x+i][ul.y+j]->initBuffer(*this);

	if (ul.x + i < loaded_start.x)
		loaded_start.x = ul.x + i;
	else if (ul.x + i > loaded_end.x)
		loaded_end.x = ul.x + i;

	if (ul.y + j < loaded_start.y)
		loaded_start.y = ul.y + j;
	else if (ul.y + j > loaded_end.y)
		loaded_end.y = ul.y + j;
}

void Map::UnloadSquare(int i, int j)
{
	//does not work with loaded_start/end yet
	freeIndices.push(squares[i][j]->bufferStartIndex);
	delete squares[i][j];
	loaded[i][j] = false;
}

void Map::replaceParticle(float x, float y, Element& p)
{
	sf::Vector2i v;
	if (!getSquareCoords(x, y, v)) return;
	if (!loaded[v.x][v.y])
		return;
	squares[v.x][v.y]->replaceParticle(p, x - v.x*sqdim, y-v.y*sqdim);
}

bool Map::getSquareCoords(float x, float y, sf::Vector2i &v)
{
	if (x >= width || y >= height || x < 0 || y < 0)
	{
		if (x >= width)
		{
			getSquareCoords(width - 1, y, v);
			return false;
		}
		if (x < 0)
		{
			getSquareCoords(0, y, v);
			return false;
		}
		if (y < 0)
		{
			getSquareCoords(x, 0, v);
			return false;
		}
		getSquareCoords(x, height - 1, v);
		return false;
	}
	v.x = x / sqdim;
	v.y = y / sqdim;
	return true;
}

Element& Map::getParticle(float x, float y)
{
	sf::Vector2i v;
	if (!getSquareCoords(x, y, v))
	{
		return *defaultOutParticle;
	}
	if (!loaded[v.x][v.y])
	{
		return *defaultOutParticle;
	}
	return (*squares[v.x][v.y])(x, y);
}

void Map::swap(float x1, float y1, float x2, float y2)
{
	sf::Vector2i v1;
	if (!getSquareCoords(x1, y1, v1)) return;
	sf::Vector2i v2;
	if (!getSquareCoords(x2, y2, v2)) return;
	if (!loaded[v1.x][v1.y] || !loaded[v2.x][v2.y])
		return;
	Element& p = (*squares[v1.x][v1.y])(x1, y1);
	Element& q = (*squares[v2.x][v2.y])(x2, y2);
	squares[v2.x][v2.y]->setParticle(p, x2 - v2.x * sqdim, y2 - v2.y * sqdim);
	squares[v1.x][v1.y]->setParticle(q, x1 - v1.x * sqdim, y1 - v1.y * sqdim);
}

void Map::updateParticle(float x, float y)
{
	sf::Vector2i v;
	if (!getSquareCoords(x, y, v)) return;
	if (!loaded[v.x][v.y])
		return;
	squares[v.x][v.y]->updatedParticle(x - v.x * sqdim, y - v.y * sqdim);
}

void Map::swapWithTrail(float x1, float y1, float x2, float y2, sf::Color& c)
{
	replaceParticle(x2, y2, *new Trail(x2, y2, nullptr, c));
	swap(x1, y1, x2, y2);
}

void Map::update(float delta)
{
	frameCounter++;
	this->delta = delta;
	sync_update->arrive_and_wait();
	sync_update->arrive_and_wait();
	/*for (int i = loaded_start.x; i <= loaded_end.x; i++)
	{
		for (int j = loaded_start.y; j <= loaded_end.y; j++)
		{
			if (loaded[i][j] && squares[i][j]->updatedThisFrame) {
				squares[i][j]->updatedThisFrame = false;
				squares[i][j]->updateVertices(*this);
			}
		}
	}*/
	/*update4(delta, 0, 0);
	update4(delta, 1, 0);
	update4(delta, 0, 1);
	update4(delta, 1, 1);*/

	/*update4(delta, 0, 2);
	update4(delta, 1, 2);
	update4(delta, 2, 2);
	update4(delta, 2, 1);
	update4(delta, 2, 0);*/
}

void Map::update4(float delta, int istart, int jstart)
{
	for (int i = istart; i < nsqx; i += 2)
	{
		for (int j = jstart; j < nsqy; j += 2)
		{
			if (loaded[i][j] && squares[i][j]->updateNextFrame) {
				squares[i][j]->update(delta, *this);
				//pool->push_task(&Square::update, squares[i][j], delta, std::ref(*this));
			}
		}
	}
	//pool->wait_for_tasks();
}

Element& Map::operator()(float x, float y)
{
	return getParticle(x, y);
}

void Map::draw(sf::RenderWindow* window)
{
	//window->draw(mapBuffer);
	
	for (int i = 0; i < nsqx; i++)
	{
		for (int j = 0; j < nsqy; j++)
		{
			if (loaded[i][j])
				window->draw(*squares[i][j]);
		}
	}
}