#include "FourWaySelector.h"

FourWaySelector::FourWaySelector()
{

}

FourWaySelector::FourWaySelector(int x, int y, int val, std::string label, sf::Font* font)
{
	this->x = x;
	this->y = y;
	this->font = font;
	this->value = val;
	this->label = label;
}

void FourWaySelector::input(sf::Event& e)
{
	if(e.type == sf::Event::MouseButtonPressed)
	{
		int mx = e.mouseButton.x, my = e.mouseButton.y;
		if(mx>=x-12 && mx<x+12 && my>=y && my<y+24) //up
			value = 0;
		else if(mx>=x+14 && mx<x+38 && my>=y+13 && my<y+37) //right
			value = 1;
		else if(mx>=x-12 && mx<x+12 && my>=y+26 && my<y+50) //down
			value = 2;
		else if(mx>=x-38 && mx<x-14 && my>=y+13 && my<y+37) //left
			value = 3;
	}
}

void FourWaySelector::render(sf::RenderTarget* target)
{
	sf::RectangleShape button(sf::Vector2f(22, 22));
	button.setOutlineThickness(1);

	button.setFillColor(sf::Color(16+(value==0)*64, 16+(value==0)*64, 16+(value==0)*64));
	button.setPosition(x-12, y); //up
	target->draw(button);

	button.setFillColor(sf::Color(16+(value==1)*64, 16+(value==1)*64, 16+(value==1)*64));
	button.setPosition(x+14, y+13); //right
	target->draw(button);

	button.setFillColor(sf::Color(16+(value==2)*64, 16+(value==2)*64, 16+(value==2)*64));
	button.setPosition(x-12, y+26); //down
	target->draw(button);

	button.setFillColor(sf::Color(16+(value==3)*64, 16+(value==3)*64, 16+(value==3)*64));
	button.setPosition(x-38, y+13); //left
	target->draw(button);

	sf::Text txt(label, *font, 16);
	txt.setPosition(x-64-txt.getLocalBounds().width, y+13);
	target->draw(txt);
}