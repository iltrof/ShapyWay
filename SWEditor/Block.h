#pragma once
#include <SFML\Graphics.hpp>

enum BLOCK_IDS { ID_NONE = -1, ID_AIR, ID_GRASS, ID_LASER, ID_LEVER, ID_WATER, ID_LAVA, ID_ACID,
	ID_PRESS, ID_PORTAL, ID_LIGHT, ID_CHECKPOINT, NUM_IDS };

struct Block
{
	Block();
	Block(int ID, int data);

	void render(Block* blocks, int width, int height, int cellx, int celly, int xoffset, int yoffset, sf::Texture* textures, sf::RenderTarget* target);

	int ID;
	int data;
};