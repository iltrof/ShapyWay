#pragma once
#include "InterfaceElement.h"

struct FourWaySelector : public InterfaceElement
{
	FourWaySelector();
	FourWaySelector(int x, int y, int val, std::string label, sf::Font* font);

	void input(sf::Event& e);
	void render(sf::RenderTarget* target);

	int x;
	int y;
	int value;
	sf::Font* font;
	std::string label;
};