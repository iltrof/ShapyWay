#pragma once
#include "Block.h"

#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 720

#define GRID_WIDTH 40
#define GRID_HEIGHT 30

typedef sf::Vector2i vec2i;

namespace Util
{
	int getBlockID(Block* blocks, int width, int height, int cellx, int celly);
	int getBlockData(Block* blocks, int width, int height, int cellx, int celly);
	int clamp(int val, int min, int max);
	sf::Color from8bit(int byte);
	bool isSolid(int ID);
	bool isInterscreen(int ID);
	bool isPowering(int ID);
	bool isSwitchable(int ID);
}

struct Link
{
	Link() : from(0, 0), to(0, 0), onPowerOn(true) {}
	Link(vec2i from, vec2i to, bool onPowerOn) : from(from), to(to), onPowerOn(onPowerOn) {}

	vec2i from;
	vec2i to;
	bool onPowerOn;
};