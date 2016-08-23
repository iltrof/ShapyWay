#pragma once
#include <map>
#include <string>
#include <SFML\Graphics.hpp>
#include <stack>

#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 720

#define GRID_WIDTH 40
#define GRID_HEIGHT 30

class SharedRes //handles textures and fonts shared by different classes to avoid unnecessary texture/font loading
{
public:
	~SharedRes() { instance = nullptr; }

	static SharedRes* Get() 
	{
		if (instance == nullptr)
			instance = new SharedRes;
		return instance;
	}

	bool init();
	sf::Texture* getTexture(std::string name); //requests a texture by name, the texture is loaded if it hasn't been before
	void subtractTexture(sf::Texture* theTex); //decrements the number of classes that use a texture, if it becomes 0, the texture is deleted
	sf::Font* getFont(std::string name);
	void subtractFont(sf::Font* theFont);

	sf::Shader blurShader;
	std::string programDirectory;
private:
	SharedRes() {}
	static SharedRes* instance;

	std::map<std::string, std::pair<sf::Texture*, int>> textures; //for each texture/font name stores the pointer to the resource
	std::map<std::string, std::pair<sf::Font*, int>> fonts;		 //and the number of classes that currently use it
};