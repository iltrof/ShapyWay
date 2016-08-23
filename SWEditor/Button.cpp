#include "Button.h"

Button::Button(int x, int y, int sx, int sy, std::string label, sf::Font* font)
	: x(x), y(y), sx(sx), sy(sy), label(label), font(font) {}

bool Button::input(sf::Event& e)
{
	if(e.type == e.MouseButtonPressed)
	{
		if(e.mouseButton.x >= x && e.mouseButton.x < x+sx && e.mouseButton.y >= y && e.mouseButton.y < y+sy)
			return 1;
	}
	return 0;
}

void Button::render(sf::RenderWindow* window)
{
	sf::RectangleShape button({ (float)sx, (float)sy });
	button.setPosition({ x-1.f, y-1.f });
	int mx = sf::Mouse::getPosition(*window).x, my = sf::Mouse::getPosition(*window).y;
	if(mx >= x && mx < x+sx && my >= y && my < y+sy)
		button.setFillColor(sf::Color(64, 64, 64));
	else
		button.setFillColor(sf::Color(16, 16, 16));
	button.setOutlineThickness(1);
	window->draw(button);

	sf::Text labelText(label, *font, 16);
	labelText.setPosition({ x+sx/2 - labelText.getLocalBounds().width/2, y+2.f });
	window->draw(labelText);
}