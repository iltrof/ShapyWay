#include "SharedRes.h"
#include <direct.h>
#include <fstream>

#include "shaders.h"

#define GetCurrentDir _getcwd

SharedRes* SharedRes::instance = nullptr;

bool SharedRes::init()
{
	char cCurrentPath[FILENAME_MAX];

	if(!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
		return 0;
		
	cCurrentPath[sizeof(cCurrentPath) - 1] = '\0';
	programDirectory = cCurrentPath;
	programDirectory+="\\";

	blurShader.loadFromMemory(blurFragShader, sf::Shader::Fragment);
	return 1;
}

sf::Texture* SharedRes::getTexture(std::string name)
{
	if(textures.find(name) == textures.end())
	{
		sf::Texture* newTex = new sf::Texture();
		newTex->loadFromFile(programDirectory+"res/graphics/"+name);
		textures[name] = std::pair<sf::Texture*, int>(newTex, 1);
		return newTex;
	}
	else
	{
		textures[name].second++;
		return textures[name].first;
	}
}

void SharedRes::subtractTexture(sf::Texture* theTex)
{
	std::string texId;
	for(auto& texture : textures)
	{
		if(texture.second.first == theTex)
			texId = texture.first;
	}
	textures[texId].second--;
	if(textures[texId].second==0)
	{
		delete textures[texId].first;
		textures.erase(texId);
	}
}

sf::Font* SharedRes::getFont(std::string name)
{
	if(fonts.find(name) == fonts.end())
	{
		sf::Font* newFont = new sf::Font();
		newFont->loadFromFile(programDirectory+"res/fonts/"+name);
		fonts[name] = std::pair<sf::Font*, int>(newFont, 1);
		return newFont;
	}
	else
	{
		fonts[name].second++;
		return fonts[name].first;
	}
}

void SharedRes::subtractFont(sf::Font* theFont)
{
	std::string fontId;
	for(auto& font : fonts)
	{
		if(font.second.first == theFont)
			fontId = font.first;
	}
	fonts[fontId].second--;
	if(fonts[fontId].second==0)
	{
		delete fonts[fontId].first;
		fonts.erase(fontId);
	}
}