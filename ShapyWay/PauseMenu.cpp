#include "PauseMenu.h"
#include "SharedRes.h"
#include "notifications.h"

PauseMenu::PauseMenu()
{
	font = SharedRes::getFont("segoesc.ttf");
	pHeld = sf::Keyboard::isKeyPressed(sf::Keyboard::P);
	returnButton = RoundMenuButton("menu/buttonPlaySmall.png", WINDOW_WIDTH/2-150, WINDOW_HEIGHT/2, 32);
	restartButton = RoundMenuButton("menu/buttonRestartSmall.png", WINDOW_WIDTH/2-50, WINDOW_HEIGHT/2, 32);
	settingsButton = RoundMenuButton("menu/buttonOptionsSmall.png", WINDOW_WIDTH/2+50, WINDOW_HEIGHT/2, 32);
	quitButton = RoundMenuButton("menu/buttonExitSmall.png", WINDOW_WIDTH/2+150, WINDOW_HEIGHT/2, 32);
}

PauseMenu::~PauseMenu()
{
	SharedRes::subtractFont(font);
}

void PauseMenu::input(sf::Event& e)
{
	if(e.type == e.KeyPressed)
	{
		if(e.key.code == sf::Keyboard::Escape || (e.key.code == sf::Keyboard::P && !pHeld))
			Notifications::queue.push(notifPOP_SCENE);
	}
	else if(e.type == e.KeyReleased && e.key.code == sf::Keyboard::P)
	{
		pHeld = false;
	}

	if(returnButton.input(e))
		Notifications::queue.push(notifPOP_SCENE);
	else if(restartButton.input(e))
	{
		Notifications::queue.push(notifPOP_SCENE);
		Notifications::queue.push(notifRESTART);
	}
	else if(settingsButton.input(e))
		Notifications::queue.push(notifGOTO_OPTIONS_MENU);
	else if(quitButton.input(e))
		Notifications::queue.push(notifEXIT_GAME);
}

void PauseMenu::update(sf::Vector2i mousePos)
{
	returnButton.update(mousePos);
	restartButton.update(mousePos);
	settingsButton.update(mousePos);
	quitButton.update(mousePos);
}

void PauseMenu::render(sf::RenderTarget* target)
{
	sf::Text paused("PAUSED", *font);
	paused.setPosition(WINDOW_WIDTH/2-paused.getLocalBounds().width/2, WINDOW_HEIGHT/2-96-paused.getLocalBounds().height/2);
	target->draw(paused);

	returnButton.render(target);
	restartButton.render(target);
	settingsButton.render(target);
	quitButton.render(target);

	if(returnButton.glowing)
	{
		paused.setString("BACK");
		paused.setPosition(WINDOW_WIDTH/2-paused.getLocalBounds().width/2, WINDOW_HEIGHT/2+76-paused.getLocalBounds().height/2);
		target->draw(paused);
	}
	else if(restartButton.glowing)
	{
		paused.setString("RESTART");
		paused.setPosition(WINDOW_WIDTH/2-paused.getLocalBounds().width/2, WINDOW_HEIGHT/2+76-paused.getLocalBounds().height/2);
		target->draw(paused);
	}
	else if(settingsButton.glowing)
	{
		paused.setString("OPTIONS");
		paused.setPosition(WINDOW_WIDTH/2-paused.getLocalBounds().width/2, WINDOW_HEIGHT/2+76-paused.getLocalBounds().height/2);
		target->draw(paused);
	}
	else if(quitButton.glowing)
	{
		paused.setString("EXIT");
		paused.setPosition(WINDOW_WIDTH/2-paused.getLocalBounds().width/2, WINDOW_HEIGHT/2+76-paused.getLocalBounds().height/2);
		target->draw(paused);
	}
}