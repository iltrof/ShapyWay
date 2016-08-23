#pragma once
#include <SFML/Graphics.hpp>
#include "Scene.h"

class MainMenu : public Scene
{
public:
	MainMenu();
	~MainMenu();

	void update(sf::Vector2i mousePos);
	void render(sf::RenderTarget* target);
	void input(sf::Event& e); //input handler
private:
	struct Button //the struct for main menu's buttons
	{
		float x, y;
		bool hovered;
		sf::Texture* texture;
		sf::Sprite sprite;
	} *buttons;

	sf::Font* segoe;
	sf::Texture* logoTex;
	sf::Texture* logoGlowTex;
	sf::Texture* buttonTex;

	int rotCount; //counter for buttons' rotations and fading
};