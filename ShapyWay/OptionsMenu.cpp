#include "OptionsMenu.h"
#include "notifications.h"
#include "Settings.h"
#include "SharedRes.h"

OptionsMenu::OptionsMenu()
{
	musicSlider = Slider(500, 164, 0, 100, &Settings::musicVolume);
	soundSlider = Slider(500, 196, 0, 100, &Settings::soundVolume);

	font = SharedRes::getFont("segoesc.ttf");
}

OptionsMenu::~OptionsMenu()
{
	SharedRes::subtractFont(font);
}

void OptionsMenu::update(sf::Vector2i mousePos)
{
	if((musicSlider.dragged || soundSlider.dragged) && !sf::Mouse::isButtonPressed(sf::Mouse::Left))
		musicSlider.dragged = soundSlider.dragged = false;

	auto doDrag = [&mousePos](Slider& slider)
	{
		if(slider.dragged)
		{
			*slider.value = slider.bcap + ((mousePos.x - slider.x - 5.f) / 110.f)*(slider.tcap - slider.bcap);
			if(*slider.value < slider.bcap)
				*slider.value = slider.bcap;
			else if(*slider.value > slider.tcap)
				*slider.value = slider.tcap;
			Notifications::queue.push(notifUPDATE_SETTINGS);
		}
	};
	doDrag(musicSlider); doDrag(soundSlider);
}

void OptionsMenu::input(sf::Event& e)
{
	if(e.type == e.KeyPressed && e.key.code == sf::Keyboard::Escape)
		Notifications::queue.push(notifPOP_SCENE);
	else if(e.type == e.MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left)
	{
		if(e.mouseButton.x >= soundSlider.x && e.mouseButton.x < soundSlider.x + 120
		   && e.mouseButton.y >= soundSlider.y - 12 && e.mouseButton.y < soundSlider.y + 12)
		   soundSlider.dragged = true;
		if(e.mouseButton.x >= musicSlider.x && e.mouseButton.x < musicSlider.x + 120
		   && e.mouseButton.y >= musicSlider.y - 12 && e.mouseButton.y < musicSlider.y + 12)
		   musicSlider.dragged = true;
	}
}

void OptionsMenu::render(sf::RenderTarget* target)
{
	sf::RectangleShape overlay(sf::Vector2f(320, 480));
	overlay.setPosition(320, 120);
	
	overlay.setFillColor(sf::Color(0, 0, 0, 192));
	overlay.setOutlineColor(sf::Color(24, 24, 24));
	overlay.setOutlineThickness(8);

	target->draw(overlay);

	musicSlider.render(target);
	soundSlider.render(target);

	sf::Text musicVolume("Music Volume", *font, 16);
	musicVolume.setPosition(344, musicSlider.y-12);
	sf::Text soundVolume("Sound Volume", *font, 16);
	soundVolume.setPosition(344, soundSlider.y-12);

	target->draw(musicVolume);
	target->draw(soundVolume);
}

void OptionsMenu::Slider::render(sf::RenderTarget* target)
{
	sf::RectangleShape gauge(sf::Vector2f(120, 4));
	gauge.setOrigin(0, 2); gauge.setFillColor(sf::Color(64, 64, 64));
	gauge.setPosition(x, y);
	target->draw(gauge);

	sf::RectangleShape knob(sf::Vector2f(10, 24));
	knob.setOrigin(5, 12);
	knob.setFillColor(sf::Color(96, 96, 96));
	knob.setOutlineColor(sf::Color(64, 64, 64));
	knob.setOutlineThickness(1);
	knob.setPosition(x+5+110.f*((*value-bcap)/(tcap-bcap)), y);
	target->draw(knob);
}