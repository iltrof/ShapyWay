#include "SkinEditor.h"
#include "notifications.h"
#include "SoundSystem.h"
#include "shaders.h"

#define GRID_LEFT 100
#define GRID_TOP 160
#define CELL_SIZE 20

SkinEditor::SkinEditor(std::string fileName)
{
	sf::Image skin; skin.loadFromFile(fileName);
	pixels = new sf::Color[480];
	for(int i = 0; i<24; i++)
	{
		for(int j = 0; j<20; j++)
		{
			pixels[j*24+i] = skin.getPixel(i, j);
		}
	}
	miniature.create(24, 20);

	for(int i = 0; i<=24; i++)
	{
		grid.push_back(sf::Vertex(sf::Vector2f(GRID_LEFT+CELL_SIZE*(float)i, GRID_TOP), sf::Color(192, 192, 192, 64)));
		grid.push_back(sf::Vertex(sf::Vector2f(GRID_LEFT+CELL_SIZE*(float)i, GRID_TOP+CELL_SIZE*20.f), sf::Color(192, 192, 192, 64)));
	}
	for(int j = 0; j<=20; j++)
	{
		grid.push_back(sf::Vertex(sf::Vector2f(GRID_LEFT, GRID_TOP+CELL_SIZE*(float)j), sf::Color(192, 192, 192, 64)));
		grid.push_back(sf::Vertex(sf::Vector2f(GRID_LEFT+CELL_SIZE*24.f, GRID_TOP+CELL_SIZE*(float)j), sf::Color(192, 192, 192, 64)));
	}

	colorPicker = new ColorPicker(680, 24, 256, 28);

	colorPicker->SVColorShader.loadFromMemory(SVColorFragShader, sf::Shader::Fragment);
	colorPicker->SVColorShader.setUniform("hueIndex", 0);
	colorPicker->SVColorShader.setUniform("hueFactor", 0);
	colorPicker->HColorShader.loadFromMemory(HColorFragShader, sf::Shader::Fragment);
	SoundSystem::Get()->stopMusic();
}

SkinEditor::~SkinEditor()
{
	delete[] pixels;
	delete colorPicker;
}

void SkinEditor::input(sf::Event& e)
{
	if(e.type == e.KeyPressed && e.key.code == sf::Keyboard::Escape)
		Notifications::queue.push(notifGOTO_MAIN_MENU);
}

void SkinEditor::update(sf::Vector2i mousePos)
{

}

void SkinEditor::render(sf::RenderTarget* target)
{
	miniature.clear();
	std::vector<sf::Vertex> points;
	for(int i = 0; i<24; i++)
	{
		for(int j = 0; j<20; j++)
		{
			points.push_back(sf::Vertex(sf::Vector2f((float)i, (float)j), pixels[j*24+i]));
		}
	}
	miniature.draw(&points[0], points.size(), sf::Points);
	miniature.display();

	sf::Sprite mini(miniature.getTexture());
	target->draw(mini);

	mini.setScale(CELL_SIZE, CELL_SIZE);
	mini.setPosition(GRID_LEFT, GRID_TOP);
	target->draw(mini);

	target->draw(&grid[0], grid.size(), sf::Lines);

	colorPicker->render(target);
}

SkinEditor::ColorPicker::ColorPicker() {}

SkinEditor::ColorPicker::ColorPicker(int x, int y, int width, int hueHeight)
	: x(x), y(y), width(width), hueHeight(hueHeight)
{
	svColorRect.push_back(sf::Vertex(sf::Vector2f((float)x, (float)y), sf::Vector2f(0, 1)));
	svColorRect.push_back(sf::Vertex(sf::Vector2f((float)x+width, (float)y), sf::Vector2f(1, 1)));
	svColorRect.push_back(sf::Vertex(sf::Vector2f((float)x+width, (float)y+width), sf::Vector2f(1, 0)));
	svColorRect.push_back(sf::Vertex(sf::Vector2f((float)x, (float)y+width), sf::Vector2f(0, 0)));

	hColorRect.push_back(sf::Vertex(sf::Vector2f((float)x, (float)y+width+16), sf::Vector2f(0, 1)));
	hColorRect.push_back(sf::Vertex(sf::Vector2f((float)x+width, (float)y+width+16), sf::Vector2f(1, 1)));
	hColorRect.push_back(sf::Vertex(sf::Vector2f((float)x+width, (float)y+width+16+hueHeight), sf::Vector2f(1, 0)));
	hColorRect.push_back(sf::Vertex(sf::Vector2f((float)x, (float)y+width+16+hueHeight), sf::Vector2f(0, 0)));
}

void SkinEditor::ColorPicker::render(sf::RenderTarget* target)
{
	target->draw(&svColorRect[0], svColorRect.size(), sf::Quads, &SVColorShader);
	target->draw(&hColorRect[0], hColorRect.size(), sf::Quads, &HColorShader);
}