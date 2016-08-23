#include "ColorSelector.h"

ColorSelector::ColorSelector()
{

}

ColorSelector::ColorSelector(int x, int y, sf::Texture* paletteTexture)
{
	this->x = x;
	this->y = y;
	this->paletteTexture = paletteTexture;
}

void ColorSelector::update(sf::RenderWindow* window)
{
	int mx = sf::Mouse::getPosition(*window).x, my = sf::Mouse::getPosition(*window).y;
	if(sf::Mouse::isButtonPressed(sf::Mouse::Left) && mx >= x && mx < x+192 && my >= y && my < y+192)
	{
		int colorCellX = (mx-x)/12, colorCellY = (my-y)/12;
		selectedColor = sf::Color::Black;
		if(colorCellX > 7) selectedColor.b += 85;
		if(colorCellY > 7) selectedColor.b += 170;
		selectedColor.g = (colorCellX % 8)*36;
		selectedColor.r = (colorCellY % 8)*36;
	}
}

void ColorSelector::input(sf::Event& e)
{
	if(e.type == sf::Event::MouseButtonPressed)
	{
		if(e.mouseButton.x >= x && e.mouseButton.x < x+192 && e.mouseButton.y >= y && e.mouseButton.y < y+192)
		{
			int colorCellX = (e.mouseButton.x-x)/12, colorCellY = (e.mouseButton.y-y)/12;
			selectedColor = sf::Color::Black;
			if(colorCellX > 7) selectedColor.b += 85;
			if(colorCellY > 7) selectedColor.b += 170;
			selectedColor.g = (colorCellX % 8)*36;
			selectedColor.r = (colorCellY % 8)*36;
		}
	}
}

void ColorSelector::render(sf::RenderTarget* target)
{
	sf::Sprite palette(*paletteTexture);
	palette.setPosition(x, y);
	target->draw(palette);

	sf::RectangleShape selColor(sf::Vector2f(190, 14));
	selColor.setOutlineThickness(1);
	selColor.setFillColor(selectedColor);
	selColor.setPosition(x, y+200);
	target->draw(selColor);
}