#pragma once
#include <SFML\Graphics.hpp>

struct TextField
{
	TextField();
	TextField(int x, int y, int width, int height, sf::Font* font, bool numOnly = false, bool multiline = false);
	void input(sf::Event& e);
	void render(sf::RenderTarget* target);

	void setBounds(int min, int max);
	
	int minVal;
	int maxVal;
	int curPos;
	bool active;
	bool numOnly;
	bool multiline;
	sf::Font* font;
	std::string text;
	sf::Clock cursorClock;
	sf::RectangleShape field;
	std::string excludedChars;
};