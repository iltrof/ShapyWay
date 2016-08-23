#pragma once
#include <SFML\Graphics.hpp>

struct InterfaceElement
{
	virtual void input(sf::Event& e) {}
	virtual void update(sf::RenderWindow* window) {}
	virtual void render(sf::RenderTarget* target) {}
};