#include "BlockDataHandler.h"

BlockDataHandler::BlockDataHandler()
{

}

BlockDataHandler::BlockDataHandler(bool hasColor, bool hasOrient, sf::Font* font, ColorSelector* colorSelector)
	: hasColor(hasColor), hasOrientation(hasOrient), font(font), colorSelector(colorSelector)
{
	if(hasOrient)
	{
		if(hasColor) orientSelector = new FourWaySelector(WINDOW_WIDTH+186, 480, 0, "Orientation", font);
		else orientSelector = new FourWaySelector(WINDOW_WIDTH+186, 256, 0, "Orientation", font);
	}
}

void BlockDataHandler::input(sf::Event& e)
{
	if(e.type == e.MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left)
	{
		if(hasOrientation)
			orientSelector->input(e);
	}
	for(int i = 0; i<fields.size(); i++)
		fields[i].textField.input(e);
}

void BlockDataHandler::update(sf::RenderWindow* window)
{
	if(hasColor)
		colorSelector->update(window);
}

void BlockDataHandler::render(sf::RenderTarget* target)
{
	if(hasColor)
		colorSelector->render(target);
	if(hasOrientation)
		orientSelector->render(target);

	for(int i = 0; i<fields.size(); i++)
	{
		sf::Text description(fields[i].desc, *font, 16);
		description.setPosition(WINDOW_WIDTH+32, fields[i].textField.field.getPosition().y-20);
		target->draw(description);
		fields[i].textField.render(target);
	}
}

int BlockDataHandler::getData()
{
	int data = 0;
	if(hasColor)
	{
		data += colorSelector->selectedColor.b / 85;
		data += (colorSelector->selectedColor.g / 36) << 2;
		data += (colorSelector->selectedColor.r / 36) << 5;
	}
	if(hasOrientation)
	{
		if(!hasColor) data += orientSelector->value;
		else data += (orientSelector->value) << 8;
	}

	for(int i = 0; i<fields.size(); i++)
	{
		int num = Util::clamp(std::stoi(fields[i].textField.text), fields[i].min, fields[i].max);
		data += num << fields[i].bitOffset;
	}
	return data;
}

void BlockDataHandler::setData(int data)
{
	if(hasColor)
		colorSelector->selectedColor = Util::from8bit(data & 255);
	if(hasOrientation)
	{
		if(!hasColor) orientSelector->value = data & 3;
		else orientSelector->value = (data & 768) >> 8;
	}

	for(int i = 0; i<fields.size(); i++)
	{
		int mask = 0; mask = pow(2, fields[i].bitLength)-1;
		mask = mask << fields[i].bitOffset;
		int value = (data & mask) >> fields[i].bitOffset;

		fields[i].textField.text = std::to_string(value);
		fields[i].textField.curPos = fields[i].textField.text.length();
	}
}

void BlockDataHandler::addField(std::string desc, int min, int max, int bitOffset, int bitLength, std::string defVal)
{
	int startY = 256;
	if(hasColor && hasOrientation) startY = 540;
	else if(hasColor) startY = 480;
	else if(hasOrientation) startY = 316;

	ExtraField newField(desc, min, max, bitOffset, bitLength);
	newField.textField = TextField(WINDOW_WIDTH+32, startY+16+fields.size()*48, 192, 24, font, 1);
	newField.textField.setBounds(min, max);
	newField.textField.text = defVal;
	newField.textField.curPos = defVal.length();
	fields.push_back(newField);
}