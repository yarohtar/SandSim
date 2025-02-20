#pragma once
#include <SFML/Graphics.hpp>
#include<map>


namespace map
{
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
}
