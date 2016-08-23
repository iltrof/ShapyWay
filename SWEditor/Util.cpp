#include "Util.h"

namespace Util
{
	int getBlockID(Block* blocks, int width, int height, int cellx, int celly)
	{
		if(cellx<0) return -1;
		if(cellx>=width*40) return -1;
		if(celly<0) return -1;
		if(celly>=height*30) return -1;

		return blocks[celly*width*40+cellx].ID;
	}

	int getBlockData(Block* blocks, int width, int height, int cellx, int celly)
	{
		if(cellx<0) return -1;
		if(cellx>=width*40) return -1;
		if(celly<0) return -1;
		if(celly>=height*30) return -1;

		return blocks[celly*width*40+cellx].data;
	}

	int clamp(int val, int min, int max)
	{
		if(val<min) return min;
		if(val>max) return max;
		return val;
	}

	sf::Color from8bit(int byte)
	{
		return sf::Color((byte >> 5)*36, ((byte&31)>>2)*36, (byte&3)*85);
	}

	bool isSolid(int ID)
	{
		return (ID == -1 || ID == ID_GRASS);
	}

	bool isInterscreen(int ID)
	{
		return (ID == ID_LASER || ID == ID_LIGHT);
	}

	bool isPowering(int ID)
	{
		return (ID == ID_LEVER || ID == ID_PRESS);
	}

	bool isSwitchable(int ID)
	{
		return (ID == ID_LASER || ID == ID_LIGHT);
	}
}