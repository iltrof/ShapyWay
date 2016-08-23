#pragma once
#include "InterfaceElement.h"

struct ColorSelector : public InterfaceElement
{
	ColorSelector();
	ColorSelector(int x, int y, sf::Texture* paletteTexture);

	void input(sf::Event& e);
	void update(sf::RenderWindow* window);
	void render(sf::RenderTarget* target);

	int x;
	int y;
	sf::Color selectedColor;
	sf::Texture* paletteTexture;
};