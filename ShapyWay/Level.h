#pragma once
#include <SFML/Graphics.hpp>
#include <fstream>
#include "SharedRes.h"
#include "Elements.h"
#include "Scene.h"

#define NUM_DEPTH_LAYERS 3

class Level : public Scene
{
public:
	Level(std::string mapName);
	~Level();

	virtual void update(sf::Vector2i mousePos);
	virtual void render(sf::RenderTarget* target);
	virtual void input(sf::Event& e); //input handler

	void handleNotification();
private:
	void loadMap(std::string mapName);
	std::string mapName;

	Block* blocks; //array of blocks
	std::vector<Link> links;
	Actor* player;
	std::map<int, sf::Texture*> textures; //textures mapped to their respective blocks' IDs
	std::vector<Actor*> actors;
	int width, height; //width and height of the whole blocks grid

	sf::RenderTexture* depthLayers;
	std::vector<sf::Vector2i> transscreenBlocks;

	int spawnPointX;
	int spawnPointY;
};

