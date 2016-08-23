#pragma once
#include <map>
#include <string>
#include <SFML\Graphics.hpp>
#include <stack>

#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 720

#define GRID_WIDTH 40
#define GRID_HEIGHT 30

static struct SharedRes //handles textures and fonts shared by different classes to avoid unnecessary texture/font loading
{
	static std::map<std::string, std::pair<sf::Texture*, int>> textures; //for each texture/font name stores the pointer to the resource
	static std::map<std::string, std::pair<sf::Font*, int>> fonts;		 //and the number of classes that currently use it
	static std::string programDirectory;
	static bool init();

	static sf::Texture* getTexture(std::string name); //requests a texture by name, the texture is loaded if it hasn't been before
	static void subtractTexture(sf::Texture* theTex); //decrements the number of classes that use a texture, if it becomes 0, the texture is deleted
	static sf::Font* getFont(std::string name);
	static void subtractFont(sf::Font* theFont);

	static sf::Shader blurShader;
};

