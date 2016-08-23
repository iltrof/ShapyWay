#include "Game.h"
#include "MainMenu.h"
#include "OptionsMenu.h"
#include "Level.h"
#include "notifications.h"
#include "SoundSystem.h"
#include "Settings.h"
#include "PauseMenu.h"
#include "SkinEditor.h"

#include <iostream>
#include <SFML\Graphics.hpp>

std::queue<int> Notifications::queue;

Game::~Game()
{
	delete window;
}

void Game::start(int fps)
{
	window = new sf::RenderWindow(sf::VideoMode(960, 720), "ShapyWay");
	if(fps!=-1) window->setFramerateLimit(fps);
	isRunning = true; hasFocus = true; autoPause = false;

	Settings::musicVolume = 100;
	Settings::soundVolume = 100;
	SoundSystem::init();

	scenes.push_back(new MainMenu());

	blurred = new sf::RenderTexture;
	blurred->create(960, 720);

	while(isRunning)
		loop();
}

void Game::stop()
{
	isRunning = false;
	window->close();
}

void Game::loop()
{
	sf::Event e; 
	while(window->pollEvent(e))
		input(e);
	update();
	render();
}

void Game::input(sf::Event& e)
{
	switch(e.type)
	{
	case sf::Event::Closed:
		stop();
		break;
	case sf::Event::GainedFocus:
		hasFocus = true;
		break;
	case sf::Event::LostFocus:
		hasFocus = false;
		if(autoPause) Notifications::queue.push(notifPAUSE);
		break;
	default:
		if(hasFocus)
			scenes.back()->input(e);
		break;
	}
}

void Game::update()
{
	while(!Notifications::queue.empty())
	{
		switch(Notifications::queue.front())
		{
		case notifEXIT_GAME:
			stop();
			return;
		case notifGOTO_LEVEL_SELECT:
			delete scenes.back();
			scenes.pop_back();
			scenes.push_back(new Level("maps/world.txt"));
			autoPause = true;
			break;
		case notifGOTO_MAIN_MENU:
			delete scenes.back();
			scenes.pop_back();
			scenes.push_back(new MainMenu());
			autoPause = false;
			break;
		case notifGOTO_OPTIONS_MENU:
			scenes.push_back(new OptionsMenu());
			break;
		case notifGOTO_SKIN_EDITOR:
			delete scenes.back();
			scenes.pop_back();
			scenes.push_back(new SkinEditor(SharedRes::programDirectory + "res/graphics/player/playerWalk.png"));
			autoPause = false;
			break;
		case notifPOP_SCENE:
			delete scenes.back();
			scenes.pop_back();
			break;
		case notifON_PLAYER_DEATH: case notifON_CHANGE_SPAWN:
			((Level*)scenes.back())->handleNotification();
			break;
		case notifUPDATE_SETTINGS:
			SoundSystem::setMusicVolume(Settings::musicVolume);
			break;
		case notifPAUSE:
			scenes.push_back(new PauseMenu());
			break;
		case notifRESTART:
			((Level*)scenes.back())->handleNotification();
			break;
		default:
			break;
		}

		Notifications::queue.pop();
	}

	scenes.back()->update(sf::Mouse::getPosition(*window));
}

void Game::render()
{
	window->clear();
	if(scenes.size()>1)
	{
		blurred->clear();
		for(int i = 0; i<scenes.size()-1; i++)
			scenes[i]->render(blurred);
		blurred->display();

		sf::Sprite blur(blurred->getTexture());
		SharedRes::blurShader.setParameter("texture", sf::Shader::CurrentTexture);
		SharedRes::blurShader.setParameter("radius", 2);
		SharedRes::blurShader.setParameter("dimensions", sf::Vector2f(window->getSize()));
		window->draw(blur, &SharedRes::blurShader);
	}

	scenes.back()->render(window);
	window->display();
}