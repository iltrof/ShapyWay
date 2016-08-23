#pragma once
#include "Scene.h"
#include "MenuButton.h"

class PauseMenu : public Scene
{
public:
	PauseMenu();
	~PauseMenu();

	virtual void input(sf::Event& e);
	virtual void update(sf::Vector2i mousePos);
	virtual void render(sf::RenderTarget* target);
private:
	sf::Font* font;
	bool pHeld;
	RoundMenuButton returnButton;
	RoundMenuButton restartButton;
	RoundMenuButton settingsButton;
	RoundMenuButton quitButton;
};