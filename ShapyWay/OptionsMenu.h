#pragma once
#include <SFML/Graphics.hpp>
#include "Scene.h"

class OptionsMenu : public Scene
{
public:
	OptionsMenu();
	~OptionsMenu();

	virtual void update(sf::Vector2i mousePos);
	virtual void render(sf::RenderTarget* target);
	virtual void input(sf::Event& e); //input handler
private:
	struct Slider
	{
		Slider() {}
		Slider(float x, float y, float bcap, float tcap, float* value)
			: x(x), y(y), bcap(bcap), tcap(tcap), value(value), dragged(0) {}

		void render(sf::RenderTarget* target);

		float x, y;
		float bcap, tcap;
		float* value;

		bool dragged;
	} soundSlider, musicSlider;

	sf::Font* font;
};