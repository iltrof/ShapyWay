#pragma once
#include <SFML\Graphics.hpp>

class Scene //generic class for any game screen, e.g. main menu, options, level
{
public:
	virtual ~Scene() {};

	virtual void update(sf::Vector2i mousePos) {};
	virtual void render(sf::RenderTarget* target) {};
	virtual void input(sf::Event& e) {};
};

