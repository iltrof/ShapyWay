#pragma once
#include "Scene.h"

class SkinEditor : public Scene
{
public:
	SkinEditor(std::string fileName);
	~SkinEditor();

	virtual void input(sf::Event& e);
	virtual void update(sf::Vector2i mousePos);
	virtual void render(sf::RenderTarget* target);
private:
	sf::Color* pixels;
	sf::RenderTexture miniature;
	std::vector<sf::Vertex> grid;

	struct ColorPicker
	{
		ColorPicker();
		ColorPicker(int x, int y, int width, int hueHeight);
		void render(sf::RenderTarget* target);

		int x, y;
		int width;
		int hueHeight;
		sf::Shader SVColorShader;
		sf::Shader HColorShader;

		std::vector<sf::Vertex> svColorRect;
		std::vector<sf::Vertex> hColorRect;
	} *colorPicker;
};