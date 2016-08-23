#pragma once
#include <SFML\Graphics.hpp>

struct Button
{
	Button(int x, int y, int sx, int sy, std::string label, sf::Font* font);
	bool input(sf::Event& e);
	void render(sf::RenderWindow* window);

	int x;
	int y;
	int sx;
	int sy;
	sf::Font* font;
	std::string label;
};