#pragma once
#include "Scene.h"
#include <vector>
#include <SFML\System.hpp>

class Game
{
public:
	Game() {}
	~Game();

	void start(const int fps = 60); //Launches the game window and handler
	void stop(); //Destroys the window, stops the game
private:
	void loop(); //Runs the game loop

	void update();
	void render();
	void input(sf::Event& e); //Input handler

	bool isRunning;
	bool hasFocus;
	bool autoPause;

	std::vector<Scene*> scenes; //A stack of scenes (e.g. menu, level, options screens etc.)
	sf::RenderWindow* window;
	sf::RenderTexture* blurred;
};