#include "MainMenu.h"
#include "SharedRes.h"
#include "notifications.h"
#include "Settings.h"
#include "SoundSystem.h"

#define NUM_BUTTONS 4

MainMenu::MainMenu(void)
{
	segoe = SharedRes::Get()->getFont("segoesc.ttf");
	logoTex = SharedRes::Get()->getTexture("logo/ShapyWay.png");
	logoGlowTex = SharedRes::Get()->getTexture("logo/ShapyWayGlow.png");

	buttons = new Button[NUM_BUTTONS];
	buttons[0].texture = SharedRes::Get()->getTexture("menu/buttonPlay.png");
	buttons[1].texture = SharedRes::Get()->getTexture("menu/buttonOptions.png");
	buttons[2].texture = SharedRes::Get()->getTexture("menu/buttonExit.png");
	buttons[3].texture = SharedRes::Get()->getTexture("menu/buttonSkin.png");
	for(int i = 0; i < NUM_BUTTONS; i++)
	{
		buttons[i].texture->setSmooth(true);
		buttons[i].sprite.setTexture(*buttons[i].texture, true);
		buttons[i].sprite.setOrigin(40.f, 40.f);
	}
	buttonTex = SharedRes::Get()->getTexture("menu/button.png");
	buttonTex->setSmooth(true);

	SoundSystem::Get()->playMusic("menuLoop.ogg");

	rotCount = 0;
}

MainMenu::~MainMenu(void)
{
	for(int i = 0; i < NUM_BUTTONS; i++)
		SharedRes::Get()->subtractTexture(buttons[i].texture);
	delete[] buttons;

	SharedRes::Get()->subtractFont(segoe);
	SharedRes::Get()->subtractTexture(logoTex);
	SharedRes::Get()->subtractTexture(logoGlowTex);
	SharedRes::Get()->subtractTexture(buttonTex);
}

void MainMenu::input(sf::Event& e)
{
	if(e.type == sf::Event::MouseButtonPressed)
	{
		int button = -1;
		for(int i = 0; i < NUM_BUTTONS; i++)
		{
			int mx = e.mouseButton.x - (int)buttons[i].x;
			int my = e.mouseButton.y - (int)buttons[i].y;
			if(mx*mx + my*my < 45*45)
				button = i;
		}

		switch(button)
		{
		case 0: Notifications::queue.push(notifGOTO_LEVEL_SELECT); //Go to level select
			break;
		case 1: Notifications::queue.push(notifGOTO_OPTIONS_MENU); //Go to options
			break;
		case 2: Notifications::queue.push(notifEXIT_GAME); //Exit the game
			break;
		case 3: Notifications::queue.push(notifGOTO_SKIN_EDITOR); //Go to skin editor
			break;
		default:
			break;
		}
	}
}

void MainMenu::update(sf::Vector2i mousePos)
{
	rotCount++;

	for(int i = 0; i<NUM_BUTTONS; i++)
	{
		float angle = ((float)rotCount)/600.f+(i*360/NUM_BUTTONS-180)*3.14159f/180.f;
		buttons[i].x = WINDOW_WIDTH/2 - std::sinf(angle)*140.f;
		buttons[i].y = WINDOW_HEIGHT/2+135 + std::cosf(angle)*140.f;
	}

	for(int i = 0; i<NUM_BUTTONS; i++)
	{
		int dx = mousePos.x - (int)buttons[i].x;
		int dy = mousePos.y - (int)buttons[i].y;
		buttons[i].hovered = (dx*dx + dy*dy < 45*45);
	}
}

void MainMenu::render(sf::RenderTarget* target)
{
	sf::Int64 eighth = SoundSystem::Get()->currentMusic->getDuration().asMicroseconds()/8;
	float value;
	if((int)(SoundSystem::Get()->currentMusic->getPlayingOffset().asMicroseconds()/eighth)%2)
		value = (float)(SoundSystem::Get()->currentMusic->getPlayingOffset().asMicroseconds()%eighth)/(float)eighth;
	else
		value = 1.f-(float)(SoundSystem::Get()->currentMusic->getPlayingOffset().asMicroseconds()%eighth)/(float)eighth;

	float opacity = ((float)rotCount)/240.f; opacity = std::min(opacity*255, 255.f);
	buttons[0].sprite.setColor(sf::Color(sf::Uint8(value*128), 255, sf::Uint8(value*128), sf::Uint8(opacity)));
	buttons[1].sprite.setColor(sf::Color(sf::Uint8(value*128), sf::Uint8(128+value*64), 255, sf::Uint8(opacity)));
	buttons[2].sprite.setColor(sf::Color(255, sf::Uint8(value*128), sf::Uint8(value*128), sf::Uint8(opacity)));
	buttons[3].sprite.setColor(sf::Color(255, 255, sf::Uint8(value*128), sf::Uint8(opacity)));

	for(int i = 0; i < NUM_BUTTONS; i++)
	{
		if(!buttons[i].hovered)
			buttons[i].sprite.setColor(sf::Color(255, 255, 255, sf::Uint8(opacity)));

		sf::Sprite border(*buttonTex);
		border.setOrigin(50.f, 50.f);
		border.setPosition(buttons[i].x, buttons[i].y);
		border.setColor(buttons[i].sprite.getColor());
		border.setTextureRect(sf::IntRect(buttons[i].hovered ? 100 : 0, 0, 100, 100));
		target->draw(border);

		buttons[i].sprite.setPosition(buttons[i].x, buttons[i].y);
		target->draw(buttons[i].sprite);

		if(buttons[i].hovered)
		{
			sf::Text buttonName("", *segoe, 20);
			switch(i)
			{
			case 0: buttonName.setString("Play"); break;
			case 1: buttonName.setString("Options"); break;
			case 2: buttonName.setString("Exit"); break;
			case 3: buttonName.setString("Character"); break;
			default: break;
			}

			buttonName.setFillColor(sf::Color(255, 255, 255, sf::Uint8(opacity)));
			buttonName.setPosition(WINDOW_WIDTH/2 - buttonName.getLocalBounds().width/2, WINDOW_HEIGHT/2 + 125);
			target->draw(buttonName);
		}
	}

	sf::Sprite logoGlow(*logoGlowTex);
	logoGlow.setOrigin(logoGlowTex->getSize().x/2.f, 21);
	logoGlow.setPosition(WINDOW_WIDTH/2, 40);
	logoGlow.setColor(sf::Color(255, 255, 255, sf::Uint8((1.f-value)*255*((float)opacity/255.f))));
	target->draw(logoGlow);

	sf::Sprite logo(*logoTex);
	logo.setOrigin(logoTex->getSize().x/2.f, 0);
	logo.setPosition(WINDOW_WIDTH/2, 40);
	logo.setColor(sf::Color(255, 255, 255, sf::Uint8(opacity)));
	target->draw(logo);
}