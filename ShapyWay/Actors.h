#pragma once
#include <SFML/Graphics.hpp>
#include "Elements.h"

class Player : public Actor
{
public:
	Player(int x, int y);
	~Player(void);

	void render(sf::RenderTarget* target);

	sf::Texture* texture;
};