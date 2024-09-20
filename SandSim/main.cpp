#include <SFML/Graphics.hpp>
#include "Map.h"
#include "Elements.h"
#include <iostream>

void draw_circle(float brush_size, sf::Vector2f posgl, char mode, Map &m)
{
	for (float i = posgl.x - brush_size; i <= posgl.x + brush_size; i++)
	{
		for (float j = posgl.y - brush_size; j <= posgl.y + brush_size; j++)
		{
			if ((i - posgl.x) * (i - posgl.x) + (j - posgl.y) * (j - posgl.y) <= brush_size * brush_size)
			{
				if (m(i, j).isEmpty()) {
					if (mode == 'g')
						m.replaceParticle(i, j, *new Ground(i, j, nullptr));
					else if (mode == 's')
						m.replaceParticle(i, j, *new Sand(i, j, nullptr));
					else
						m.replaceParticle(i, j, *new Water(i, j, nullptr));

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

	sf::View view(sf::Vector2f(240, 135), sf::Vector2f(480,270));
	window.setView(view);

	sf::Clock clock;

	Map m;

	char mode = 's';

	window.setFramerateLimit(60);
	
	float brush_size = 1;

	int counter = 0;

	//sf::VertexArray arr;
	//arr.resize(30000);
	//arr.setPrimitiveType(sf::Points);

	while (window.isOpen())
	{

		float delta = clock.restart().asSeconds();
		counter++;
		if (counter > 10) {
			std::cout << 1 / delta << std::endl;
			counter = 0;
		}

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
				case sf::Keyboard::RBracket:
					if (brush_size < 25)
						brush_size++;
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

			draw_circle(brush_size, posgl, mode, m);

		}
		view.move(viewSpeed.x * delta, viewSpeed.y * delta);
		window.setView(view);
		m.update(delta);

		window.clear();

		/*Random r;
		for (int i = 0; i < 30000; i++) {
			arr[i].position.y = 10;
			arr[i].position.x = 100*r.beta(5,5);
			arr[i].color = sf::Color(255, 255, 255, 4);
		}
		arr[0].position.y = 10;
		arr[0].position.x = 100;
		arr[0].color = sf::Color::White;*/

		m.draw(&window);
		//window.draw(arr);
		window.display();

	}
}