#include <algorithm>
#include <stack>
#include <thread>
#include <mutex>
#include <barrier>
#include <random>
#include "Elements.h"
#include "Cell.h"
#include "Random.h"
#include "Square.h"
#include "MoveableSolidWorker.h"
#include "Diagnostics.h"

namespace
{
	const int nsqx = 32;
	const int nsqy = 32;
	std::vector<std::vector<Square*>> squares;
	std::stack<int> freeIndices;
	//square** squares = new square*[nsqx];
	bool loaded[nsqx][nsqy];
	sf::Vector2i loaded_start;
	sf::Vector2i loaded_end;
	float width;
	float height;
	sf::Vector2f view_position;
	sf::Vector2f view_size;
	Cell* default_out_cell;
	int num_threads;
	std::vector<std::thread*> threads;
	std::barrier<>* sync4;
	std::barrier<>* sync_update;
	float delta;
	bool loaded_changed;
	Random random;


	std::vector<std::pair<sf::Vector2i, sf::Vector2i>> swap_requests;
	std::mutex swap_mutex;

	bool getSquareCoords(float x, float y, sf::Vector2i& v)
	{
		if (x >= width || y >= height || x < 0 || y < 0)
		{
			return false;
		}
		v.x = x / sqdim;
		v.y = y / sqdim;
		return true;
	}

	void LoadSquare(int i, int j, sf::Vector2i& ul)
	{
		loaded_changed = true;
		squares[ul.x + i][ul.y + j] = new Square(ul.x * sqdim + i * sqdim, ul.y * sqdim + j * sqdim);
		loaded[ul.x + i][ul.y + j] = true;
		squares[ul.x + i][ul.y + j]->bufferStartIndex = freeIndices.top();
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
	void UnloadSquare(int i, int j)
	{
		//does not work with loaded_start/end yet
		loaded_changed = true;
		freeIndices.push(squares[i][j]->bufferStartIndex);
		delete squares[i][j];
		loaded[i][j] = false;
	}

	void alert_surrounding(int k, int l)
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

	void update_batch(std::vector<Square*> batch)
	{
		for (auto s : batch)
		{
			MoveableSolidWorker(s).UpdateSquare(delta);
			continue;
		}
	}


	void refresh_batches(std::vector<Square*> batches[], int thread_id)
	{
		int sx = loaded_start.x;
		int sy = loaded_start.y;
		int nx = loaded_end.x - sx + 1;
		int ny = loaded_end.y - sy + 1;

		int batch_size = nx * ny / num_threads;
		int start = thread_id * batch_size;
		int end = start + batch_size;
		if (thread_id == num_threads - 1)
			end = nx * ny;

		for (int batch_id = 0; batch_id < 4; batch_id++)
		{
			batches[batch_id].clear();
		}
		for (int iter = start; iter < end; iter++)
		{
			int i = sx + iter / ny;
			int j = sy + iter % ny;
			if(loaded[i][j])
				batches[i & 2 + j & 1].push_back(squares[i][j]);
		}
	}

	void thread_update(int thread_id, Random r)
	{
		std::vector<Square*> batches[4];
		for (int i = 0; i < 4; i++)
		{
			std::vector<Square*> temp;
			batches[i] = temp;
		}

		while (true)
		{
			sync_update->arrive_and_wait();
			if (loaded_changed)
				refresh_batches(batches, thread_id);

			for (int i = 0; i < 4; i++)
			{
				update_batch(batches[i]);
				sync4->arrive_and_wait();
			}

			sync_update->arrive_and_wait();

			for (int i = 0; i < 4; i++)
			{
				for (auto s : batches[i])
				{
					s->handle_swaps();
				}
			}

			sync_update->arrive_and_wait();

			for (int i = 0; i < 4; i++)
			{
				for (auto s : batches[i])
				{
						s->updateVertices();
						s->updateLazyRectangle();
				}
			}
			sync_update->arrive_and_wait();
		}
	}
};


sf::VertexBuffer map::mapBuffer;
unsigned long long map::frameCounter;
map::Diagnostics map::diagnostics;


void map::initialize()
{
	frameCounter = 0;
	width = sqdim * nsqx;
	height = sqdim * nsqy;
	default_out_cell = new Cell(Element::Out);
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
		freeIndices.push(i * sqdim * sqdim * 4);
	loaded_start = sf::Vector2i(nsqx, nsqy);
	loaded_end = sf::Vector2i(0, 0);
	loadEmpty();
	num_threads = std::thread::hardware_concurrency() - 1;
	threads.resize(num_threads);
	sync4 = new std::barrier(num_threads);
	sync_update = new std::barrier(num_threads + 1);
	for (int i = 0; i < num_threads; i++)
		threads[i] = new std::thread(thread_update, i, *new Random());
}


void map::loadEmpty()
{
	sf::Vector2i ul;
	getSquareCoords(view_position.x, view_position.y, ul);
	if (ul.x > 0) ul.x--;
	if (ul.y > 0) ul.y--;
	int nx = std::min((int)((view_position.x + view_size.x - ul.x * sqdim) / sqdim) + 1, nsqx);
	int ny = std::min((int)((view_position.y + view_size.y - ul.y * sqdim) / sqdim) + 1, nsqy);

	for (int i = 0; i < nx; i++)
	{
		for (int j = 0; j < ny; j++)
		{
			LoadSquare(i, j, ul);
		}
	}
}

void map::replaceParticle(float x, float y, Cell& p)
{
	sf::Vector2i v;
	if (!getSquareCoords(x, y, v)) return;
	if (!loaded[v.x][v.y])
		return;
	squares[v.x][v.y]->replaceParticle(p, x - v.x * sqdim, y - v.y * sqdim);
}

Cell& map::grid(float x, float y)
{
	x = round(x);
	y = round(y);
	sf::Vector2i v;
	if (!getSquareCoords(x, y, v))
	{
		return *default_out_cell;
	}
	if (!loaded[v.x][v.y])
	{
		return *default_out_cell;
	}
	return (*squares[v.x][v.y])(x, y);
}



void map::alert(float x, float y)
{
	sf::Vector2i v;
	for (float i = -1; i <= 1; i++)
	{
		for (float j = -1; j <= 1; j++)
		{
			if (getSquareCoords(x + i, y + j, v))
				if(loaded[v.x][v.y])
					squares[v.x][v.y]->alert(x + i, y + j);
		}
	}
}

void map::swap(float x1, float y1, float x2, float y2)
{
	sf::Vector2i v1;
	if (!getSquareCoords(x1, y1, v1)) return;
	sf::Vector2i v2;
	if (!getSquareCoords(x2, y2, v2)) return;
	if (!loaded[v1.x][v1.y] || !loaded[v2.x][v2.y])
		return;
	Cell& p = (*squares[v1.x][v1.y])(x1, y1);
	Cell& q = (*squares[v2.x][v2.y])(x2, y2);
	squares[v2.x][v2.y]->setParticle(p, x2, y2);
	squares[v1.x][v1.y]->setParticle(q, x1, y1);
}

void map::requestSwap(float x1, float y1, float x2, float y2, Square* origin)
{
	sf::Vector2i v;
	if (!getSquareCoords(x2, y2, v)) return;
	if (!loaded[v.x][v.y]) return;

	squares[v.x][v.y]->requestSwap(x1, y1, x2, y2, origin);
}


void map::swapWithTrail(float x1, float y1, float x2, float y2, sf::Color& c)
{
	swap(x1, y1, x2, y2);
}

void handle_swaps()
{
	return;
	for (int i = loaded_start.x; i < loaded_end.x; i++)
	{
		for (int j = loaded_start.y; j < loaded_end.y; j++)
		{
			squares[i][j]->handle_swaps();
		}
	}
	return;
	std::sort(swap_requests.begin(), swap_requests.end(),
		[](std::pair<sf::Vector2i, sf::Vector2i>& a, std::pair<sf::Vector2i, sf::Vector2i>& b) 
		{ if (a.second.x != b.second.x) return a.second.x < b.second.x;
			return a.second.y < b.second.y;});
	swap_requests.push_back(std::pair<sf::Vector2i, sf::Vector2i>({ 0,0 }, { 0,0 }));
	int prev = 0;
	for (int i = 0; i < swap_requests.size() - 1; i++)
	{
		auto& dest = swap_requests[i].second;
		auto& origin = swap_requests[i].first;
		if (swap_requests[i + 1].second.x != dest.x || swap_requests[i + 1].second.y != dest.y)
		{
			float s = random.rng();
			int ind = s * (i - prev+1) + prev;
			if (ind > i)
				ind = i;
			map::swap(origin.x, origin.y, dest.x, dest.y);
			prev = i + 1;
		}
	}
	swap_requests.clear();
}

void map::update(float delta)
{
	frameCounter++;
	::delta = delta;
	diagnostics.start_mmt("update");
	sync_update->arrive_and_wait();
	sync_update->arrive_and_wait();
	diagnostics.end_mmt("update");
	diagnostics.start_mmt("swaps");
	sync_update->arrive_and_wait();
	diagnostics.end_mmt("swaps");
	sync_update->arrive_and_wait();
	loaded_changed = false;
}


void map::draw(sf::RenderWindow* window)
{
	//window->draw(mapBuffer);

	for (int i = 0; i < nsqx; i++)
	{
		for (int j = 0; j < nsqy; j++)
		{
			if (loaded[i][j])
			{
				window->draw(*squares[i][j]);
			}
		}
	}
}
void map::Diagnostics::start_mmt(std::string s)
{
	if (clocks.count(s))
	{
		clocks[s].restart();
	}
	else
	{
		sf::Clock c;
		c.restart();
		clocks.insert({ s,c });
	}
}
void map::Diagnostics::end_mmt(std::string s)
{
	float t = clocks[s].restart().asSeconds();
	if (measurements.count(s))
	{
		measurements[s][0] = t;
	}
	else
	{
		std::vector<float> v;
		measurements[s].push_back(t);
	}
}

void map::Diagnostics::add_mmt(std::string s, float t)
{
	if (measurements.count(s))
	{
		measurements[s][0] = t;
	}
	else
	{
		std::vector<float> v;
		measurements[s].push_back(t);
	}
}

void map::Diagnostics::refresh(std::vector<std::string> to_display)
{
	if (!loaded)
	{
		font.loadFromFile("../Arial.ttf");
		display_text.setFont(font);
		display_text.setCharacterSize(12);
		loaded = true;
	}
	std::string text;
	for(int i =0; i<to_display.size(); i++)
	{
		if (measurements.count(to_display[i]) == 0)
		{
			text += "no \"" + to_display[i] + "\" found\n";
			continue;
		}
		text += to_display[i];
		text += ": ";
		text += std::to_string(measurements[to_display[i]].back());
		text += "\n";
	}
	display_text.setString(text);
}

void map::Diagnostics::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.texture = NULL;
	target.draw(display_text, states);
}
