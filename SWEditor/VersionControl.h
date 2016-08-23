#pragma once
#include <string>

#define CURRENT_VERSION "0.1"

void updateBlock(std::string oldVersion, int oldID, int oldData, int& newID, int& newData)
{
	if(oldVersion == CURRENT_VERSION)
	{
		newID = oldID;
		newData = oldData;
		return;
	}
	else if(oldVersion == "0.0")
	{
		if(oldID <= 4)
			newID = oldID;
		else
			newID = oldID+2;

		newData = oldData;
		return;
	}
}

/*
0.0: air, grass, laser, lever, water, press, portal, light, checkpoint
0.1: air, grass, laser, lever, water, lava, acid, press, portal, light, checkpoint
*/