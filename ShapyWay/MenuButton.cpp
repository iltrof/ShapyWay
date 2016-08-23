#include "MenuButton.h"
#include "SharedRes.h"
#include "shaders.h"

MenuButton::MenuButton()
{
	font = NULL; x = y = sx = sy = 0;
}

MenuButton::MenuButton(std::string label, sf::Font* font, int x, int y, int w, int h)
	: label(label), font(font), x(x), y(y), sx(w), sy(h)
{
	rect.setSize(sf::Vector2f((float)sx, (float)sy));
	rect.setPosition((float)x, (float)y);
	rect.setFillColor(sf::Color(96, 96, 96));
	rect.setOutlineColor(sf::Color::Black);
	rect.setOutlineThickness(1);
}

bool MenuButton::input(sf::Event& e)
{
	if(e.type == e.MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left)
	{
		if(e.mouseButton.x >= x && e.mouseButton.x < x+sx && e.mouseButton.y >= y && e.mouseButton.y < y+sy)
			return true;
	}
	return false;
}

void MenuButton::update(sf::Vector2i& mousePos)
{
	if(mousePos.x >= x && mousePos.x < x+sx && mousePos.y >= y && mousePos.y < y+sy)
		rect.setFillColor(sf::Color(128, 128, 128));
	else
		rect.setFillColor(sf::Color(96, 96, 96));
}

void MenuButton::render(sf::RenderTarget* target)
{
	target->draw(rect);
	sf::Text lb(label, *font); lb.setFillColor(sf::Color::Black);
	lb.setPosition(x+sx/2-lb.getLocalBounds().width/2, y+sy/2-lb.getLocalBounds().height/2-lb.getCharacterSize()/4);
	target->draw(lb);
}


RoundMenuButton::RoundMenuButton()
{
	x = y = radius = 0; glowing = false;
}

RoundMenuButton::RoundMenuButton(std::string spritePath, int x, int y, int radius)
	: x(x), y(y), radius(radius)
{
	texture.loadFromFile(SharedRes::Get()->programDirectory+"res/graphics/" + spritePath);
	glowing = false;
}

bool RoundMenuButton::input(sf::Event& e)
{
	if(e.type == e.MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left)
	{
		int dist = (e.mouseButton.x-x)*(e.mouseButton.x-x) + (e.mouseButton.y-y)*(e.mouseButton.y-y);
		if(dist <= radius*radius)
			return true;
	}
	return false;
}

void RoundMenuButton::update(sf::Vector2i& mousePos)
{
	int dist = (mousePos.x-x)*(mousePos.x-x) + (mousePos.y-y)*(mousePos.y-y);
	if(dist <= radius*radius)
		glowing = true;
	else
		glowing = false;
}

void RoundMenuButton::render(sf::RenderTarget* target)
{
	sf::Sprite sprite(texture);
	sprite.setOrigin(texture.getSize().y/2.f, texture.getSize().y/2.f);
	sprite.setTextureRect(sf::IntRect(texture.getSize().y*glowing, 0, texture.getSize().y, texture.getSize().y));
	sprite.setPosition((float)x, (float)y);
	target->draw(sprite);
}