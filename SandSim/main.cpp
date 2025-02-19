#include <SFML/Graphics.hpp>
#include <iostream>
#include <thread>
#include "Elements.h"

void draw_circle(float brush_size, sf::Vector2f posgl, char mode)
{
	for (float i = posgl.x - brush_size; i <= posgl.x + brush_size; i++)
	{
		for (float j = posgl.y - brush_size; j <= posgl.y + brush_size; j++)
		{
			if ((i - posgl.x) * (i - posgl.x) + (j - posgl.y) * (j - posgl.y) <= brush_size * brush_size)
			{
				if (map::grid(i, j).isEmpty()) {
					if (mode == 'g')
						map::replaceParticle(i, j, *new Ground());
					else if (mode == 's')
						map::replaceParticle(i, j, *new Sand());
					else
						map::replaceParticle(i, j, *new Water());

				}
			}
		}
	}
}

int main()
{
	std::cout << std::thread::hardware_concurrency();
	sf::RenderWindow window(sf::VideoMode(1920, 1080), "SandSim", sf::Style::Fullscreen);

	sf::Vector2f viewSpeed(0, 0);

	sf::View gameView(sf::Vector2f(240, 135), sf::Vector2f(480,270));
	sf::View UIView({960,540}, {1920, 1080});

	sf::Clock clock;

	map::initialize();

	char mode = 's';

	//window.setFramerateLimit(60);
	
	float brush_size = 1;

	int counter = 0;


	while (window.isOpen())
	{
		window.setView(gameView);
		map::diagnostics.start_mmt("frame time");
		float delta = clock.restart().asSeconds();
		counter++;

		sf::Event evnt; 
		while (window.pollEvent(evnt))
		{
			switch (evnt.type)
			{
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::KeyPressed:
				switch (evnt.key.code)
				{
				case sf::Keyboard::Escape:
					window.close();
					break;
				case sf::Keyboard::G:
					mode = 'g';
					break;
				case sf::Keyboard::E:
					mode = 's';
					break;
				case sf::Keyboard::F:
					mode = 'w';
					break;
				case sf::Keyboard::LBracket:
					if (brush_size > 1)
						brush_size--;
					break;
				case sf::Keyboard::RBracket:
					if (brush_size < 25)
						brush_size++;
					break;
				}
			}
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			viewSpeed.x = -50;
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
			viewSpeed.x = 50;
		else
			viewSpeed.x = 0;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
			viewSpeed.y = -50;
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			viewSpeed.y = 50;
		else
			viewSpeed.y = 0;

		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			sf::Vector2i pos = sf::Mouse::getPosition(window);
			sf::Vector2f posgl = window.mapPixelToCoords(pos);

			draw_circle(brush_size, posgl, mode);

		}
		gameView.move(viewSpeed.x * delta, viewSpeed.y * delta);
		map::update(delta);

		window.setView(gameView);
		window.clear();


		map::draw(&window);
		map::diagnostics.end_mmt("frame time");

		map::diagnostics.add_mmt("frame rate", 1 / delta);
		if (counter > 10)
		{
			map::diagnostics.refresh({"frame rate", "frame time", "update", "swaps"});
			counter = 0;
		}
		window.setView(UIView);
		window.draw(map::diagnostics);
		window.display();
	}
}