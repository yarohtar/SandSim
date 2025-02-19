#pragma once
#include <SFML/Graphics.hpp>
#include <map>

class Element;
class Square;

namespace map
{
	extern sf::VertexBuffer mapBuffer;
	extern unsigned long long frameCounter;

	void initialize();
	void loadEmpty();

	::Element & grid(float a, float b);
	void alert(float x, float y);

	void setViewPosition(float x, float y);
	void setViewSize(float x, float y);
	void setViewPosition(sf::Vector2f xy);
	void setViewSize(sf::Vector2f xy);

	void replaceParticle(float x, float y, ::Element& p);
	void requestSwap(float x1, float y1, float x2, float y2, Square* origin);
	void swap(float x1, float y1, float x2, float y2);
	void swapWithTrail(float x1, float y1, float x2, float y2, sf::Color& c);

	void update(float delta);
	void draw(sf::RenderWindow* window);

	class Diagnostics : public sf::Drawable, public sf::Transformable
	{
	private:
		sf::Font font;
		sf::Text display_text;
		bool loaded = false;
		std::map<std::string, std::vector<float>> measurements;
		std::map<std::string, sf::Clock> clocks;
		void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	public:
		void start_mmt(std::string s);
		void end_mmt(std::string s);
		void refresh(std::vector<std::string>);
		void add_mmt(std::string, float);
	};

	extern Diagnostics diagnostics;
};