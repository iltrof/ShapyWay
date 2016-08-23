#pragma once
#include "FourWaySelector.h"
#include "ColorSelector.h"
#include "TextField.h"
#include "Util.h"

struct BlockDataHandler : public InterfaceElement
{
	struct ExtraField
	{
		ExtraField() {}
		ExtraField(std::string d, int min, int max, int bitOffset, int bitLength) 
			: desc(d), min(min), max(max), bitOffset(bitOffset), bitLength(bitLength) {}

		std::string desc;
		int min;
		int max;
		int bitOffset;
		int bitLength;
		TextField textField;
	};

	std::vector<ExtraField> fields;

	BlockDataHandler();
	BlockDataHandler(bool hasColor, bool hasOrient, sf::Font* font, ColorSelector* colorSelector);

	void input(sf::Event& e);
	void update(sf::RenderWindow* window);
	void render(sf::RenderTarget* target);

	int getData();
	void setData(int data);

	void addField(std::string desc, int min, int max, int bitOffset, int bitLength, std::string defVal);

	bool hasColor;
	sf::Font* font;
	bool hasOrientation;
	ColorSelector* colorSelector;
	FourWaySelector* orientSelector;
};