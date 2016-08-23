#pragma once
#include "Util.h"

enum ActionTypes {ACTTYPE_CHANGEBLOCKS, ACTTYPE_ADDSCREEN, ACTTYPE_REMOVESCREEN, ACTTYPE_ADDLINK, ACTTYPE_REMOVELINK, ACTTYPE_SWITCHLINK, ACTTYPE_SETSPAWN};

struct Action;

void setBlock(int cellx, int celly, int newID, int data);
void addScreen(int dir);
void removeScreen(int dir, Action* undo);

int worldWidth = 1; //The width of the world (in screens)
int worldHeight = 1; //The height of the world (in screens)
vec2i spawnPoint(GRID_WIDTH/2, GRID_HEIGHT/2);

int xoffset = 0; //The horizontal offset at which the world is drawn (in cells)
int yoffset = 0; //The vertical offset at which the world is drawn (in cells)

Block* blocks; //The array of all blocks
std::vector<vec2i> interscreenBlocks; //The array of indices for blocks which should be drawn even when they are not on the screen
std::vector<Link> links; //The array of all links

struct Action
{
	Action(int type) : type(type)
	{
		screenRemove.cutBlocks = NULL;
	}

	~Action()
	{
		delete[] screenRemove.cutBlocks;
	}

	int type;

	struct BlocksChange
	{
		std::vector<Block> blocksBefore;
		std::vector<Block> blocksAfter;
		std::vector<vec2i> positions;
	} blockChange;

	struct ScreenAdd
	{
		int direction;
	} screenAdd;

	struct ScreenRemove
	{
		int direction;
		Block* cutBlocks;
		std::vector<Link> deadLinks;
		std::vector<vec2i> deadISBs;
		vec2i spawnBefore;
	} screenRemove;

	struct LinkAct
	{
		vec2i start;
		vec2i end;
		bool green;
	} link;

	struct SpawnSet
	{
		vec2i oldPos;
		vec2i newPos;
	} spawn;
};

void undoAction(Action* action)
{
	switch(action->type)
	{
	case ACTTYPE_CHANGEBLOCKS:
	{
		for(unsigned int i = 0; i<action->blockChange.positions.size(); i++)
		{
			setBlock(action->blockChange.positions[i].x, action->blockChange.positions[i].y,
					 action->blockChange.blocksBefore[i].ID, action->blockChange.blocksBefore[i].data);
		}
	} break;
	case ACTTYPE_ADDSCREEN:
	{
		removeScreen(action->screenAdd.direction, NULL);
	} break;
	case ACTTYPE_REMOVESCREEN:
	{
		addScreen(action->screenRemove.direction);
		int cw = 1, ch = 1, xoff = 0, yoff = 0; 
		if(action->screenRemove.direction == 1) xoff = (worldWidth-1)*GRID_WIDTH;
		else if(action->screenRemove.direction == 2) yoff = (worldHeight-1)*GRID_HEIGHT;
		if(action->screenRemove.direction == 0 || action->screenRemove.direction == 2) cw = worldWidth; else ch = worldHeight;

		for(int i = 0; i<cw*GRID_WIDTH; i++)
		{
			for(int j = 0; j<ch*GRID_HEIGHT; j++)
			{
				blocks[(j+yoff)*worldWidth*GRID_WIDTH + i+xoff] = action->screenRemove.cutBlocks[j*cw*GRID_WIDTH + i];
			}
		}

		for(auto isb : action->screenRemove.deadISBs)
			interscreenBlocks.push_back(isb);
		for(auto isb : action->screenRemove.deadLinks)
			links.push_back(isb);
		spawnPoint = action->screenRemove.spawnBefore;
	} break;
	case ACTTYPE_ADDLINK:
	{
		for(unsigned int i = 0; i<links.size(); i++)
		{
			if(links[i].from.x == action->link.start.x && links[i].from.y == action->link.start.y
			   && links[i].to.x == action->link.end.x && links[i].to.y == action->link.end.y)
			{
				links.erase(links.begin()+i);
				break;
			}
		}
	} break;
	case ACTTYPE_REMOVELINK:
	{
		links.push_back(Link(action->link.start, action->link.end, action->link.green));
	} break;
	case ACTTYPE_SWITCHLINK:
	{
		for(unsigned int i = 0; i<links.size(); i++)
		{
			if(links[i].from.x == action->link.start.x && links[i].from.y == action->link.start.y
			   && links[i].to.x == action->link.end.x && links[i].to.y == action->link.end.y)
			{
				links[i].onPowerOn = !links[i].onPowerOn;
				break;
			}
		}
	} break;
	case ACTTYPE_SETSPAWN:
	{
		spawnPoint = action->spawn.oldPos;
	} break;
	}
}

void redoAction(Action* action)
{
	switch(action->type)
	{
	case ACTTYPE_CHANGEBLOCKS:
	{
		for(unsigned int i = 0; i<action->blockChange.positions.size(); i++)
		{
			setBlock(action->blockChange.positions[i].x, action->blockChange.positions[i].y,
					 action->blockChange.blocksAfter[i].ID, action->blockChange.blocksAfter[i].data);
		}
	} break;
	case ACTTYPE_ADDSCREEN:
	{
		addScreen(action->screenAdd.direction);
	} break;
	case ACTTYPE_REMOVESCREEN:
	{
		removeScreen(action->screenRemove.direction, NULL);
	} break;
	case ACTTYPE_ADDLINK:
	{
		links.push_back(Link(action->link.start, action->link.end, action->link.green));
	} break;
	case ACTTYPE_REMOVELINK:
	{
		for(unsigned int i = 0; i<links.size(); i++)
		{
			if(links[i].from.x == action->link.start.x && links[i].from.y == action->link.start.y
			   && links[i].to.x == action->link.end.x && links[i].to.y == action->link.end.y)
			{
				links.erase(links.begin()+i);
				break;
			}
		}
	} break;
	case ACTTYPE_SWITCHLINK:
	{
		for(unsigned int i = 0; i<links.size(); i++)
		{
			if(links[i].from.x == action->link.start.x && links[i].from.y == action->link.start.y
			   && links[i].to.x == action->link.end.x && links[i].to.y == action->link.end.y)
			{
				links[i].onPowerOn = !links[i].onPowerOn;
				break;
			}
		}
	} break;
	case ACTTYPE_SETSPAWN:
	{
		spawnPoint = action->spawn.newPos;
	} break;
	}
}

void setBlock(int cellx, int celly, int newID, int data)
{
	int oldID = blocks[celly*worldWidth*GRID_WIDTH+cellx].ID;
	blocks[celly*worldWidth*GRID_WIDTH+cellx].ID = newID;
	blocks[celly*worldWidth*GRID_WIDTH+cellx].data = data;

	if(Util::isInterscreen(oldID) && !Util::isInterscreen(newID))
	{
		for(unsigned int i = 0; i<interscreenBlocks.size(); i++)
		{
			if(interscreenBlocks[i].x == cellx && interscreenBlocks[i].y == celly)
			{
				interscreenBlocks.erase(interscreenBlocks.begin()+i);
				break;
			}
		}
	}
	else if(!Util::isInterscreen(oldID) && Util::isInterscreen(newID))
	{
		interscreenBlocks.push_back(vec2i(cellx, celly));
	}

	if((Util::isPowering(oldID) && !Util::isPowering(newID)) || (Util::isSwitchable(oldID) && !Util::isSwitchable(newID)))
	{
		for(unsigned int i = 0; i<links.size(); i++)
		{
			if((links[i].from.x == cellx && links[i].from.y == celly) || (links[i].to.x == cellx && links[i].to.y == celly))
			{
				links.erase(links.begin()+i);
				break;
			}
		}
	}
}

void addScreen(int dir)
{
	int newWidth = worldWidth, newHeight = worldHeight;
	if(dir == 0 || dir == 2) newHeight++;
	else newWidth++;

	Block* newBlocks = new Block[newWidth*newHeight*GRID_WIDTH*GRID_HEIGHT];
	int xoff = 0, yoff = 0;
	if(dir == 0) yoff = GRID_HEIGHT;
	else if(dir == 3) xoff = GRID_WIDTH;

	for(int i = 0; i<worldWidth*GRID_WIDTH; i++)
	{
		for(int j = 0; j<worldHeight*GRID_HEIGHT; j++)
		{
			newBlocks[(j+yoff)*newWidth*GRID_WIDTH + i+xoff] = blocks[j*worldWidth*GRID_WIDTH + i];
		}
	}

	for(unsigned int i = 0; i<interscreenBlocks.size(); i++)
	{
		interscreenBlocks[i].x += xoff;
		interscreenBlocks[i].y += yoff;
	}

	for(unsigned int i = 0; i<links.size(); i++)
	{
		links[i].from.x += xoff; links[i].to.x += xoff;
		links[i].from.y += yoff; links[i].to.y += yoff;
	}

	delete[] blocks; blocks = newBlocks;
	worldWidth = newWidth; worldHeight = newHeight;
	xoffset += xoff; yoffset += yoff;
	spawnPoint.x += xoff; spawnPoint.y += yoff;
}

void removeScreen(int dir, Action* undo)
{
	int newWidth = worldWidth, newHeight = worldHeight;
	if(dir == 0 || dir == 2) newHeight--;
	else newWidth--;
	if(newWidth == 0 || newHeight == 0) return;

	Block* newBlocks = new Block[newWidth*newHeight*GRID_WIDTH*GRID_HEIGHT];
	int xoff = 0, yoff = 0;
	if(dir == 0) yoff = GRID_HEIGHT;
	else if(dir == 3) xoff = GRID_WIDTH;

	Block* cutBlocks;
	if(dir == 0 || dir == 2) cutBlocks = new Block[worldWidth*GRID_WIDTH*GRID_HEIGHT];
	else cutBlocks = new Block[worldHeight*GRID_WIDTH*GRID_HEIGHT];

	for(int i = 0; i<worldWidth*GRID_WIDTH; i++)
	{
		for(int j = 0; j<worldHeight*GRID_HEIGHT; j++)
		{
			if(i>=xoff && i<newWidth*GRID_WIDTH+xoff && j>=yoff && j<newHeight*GRID_HEIGHT+yoff)
				newBlocks[(j-yoff)*newWidth*GRID_WIDTH + i-xoff] = blocks[j*worldWidth*GRID_WIDTH + i];
			else
			{
				if(dir == 0)
					cutBlocks[j*worldWidth*GRID_WIDTH+i] = blocks[j*worldWidth*GRID_WIDTH+i];
				else if(dir == 1)
					cutBlocks[j*GRID_WIDTH+i-newWidth*GRID_WIDTH] = blocks[j*worldWidth*GRID_WIDTH+i];
				else if(dir == 2)
					cutBlocks[(j-newHeight*GRID_HEIGHT)*worldWidth*GRID_WIDTH+i] = blocks[j*worldWidth*GRID_WIDTH+i];
				else
					cutBlocks[j*GRID_WIDTH+i] = blocks[j*worldWidth*GRID_WIDTH+i];
			}
		}
	}

	if(undo != NULL)
		undo->screenRemove.cutBlocks = cutBlocks;
	else 
		delete[] cutBlocks;

	for(unsigned int i = 0; i<interscreenBlocks.size(); i++)
	{
		auto isb = interscreenBlocks[i];
		interscreenBlocks[i].x -= xoff;
		interscreenBlocks[i].y -= yoff;
		if(interscreenBlocks[i].x < 0 || interscreenBlocks[i].x >= newWidth*GRID_WIDTH
		   ||interscreenBlocks[i].y < 0 || interscreenBlocks[i].y >= newHeight*GRID_HEIGHT)
		{
			if(undo != NULL)
				undo->screenRemove.deadISBs.push_back(isb);
			interscreenBlocks.erase(interscreenBlocks.begin()+i);
			i--;
		}
	}

	for(unsigned int i = 0; i<links.size(); i++)
	{
		Link oldLink = links[i];
		links[i].from.x -= xoff; links[i].to.x -= xoff;
		links[i].from.y -= yoff; links[i].to.y -= yoff;
		if(links[i].from.x < 0 || links[i].from.x >= newWidth*GRID_WIDTH||links[i].from.y < 0 || links[i].from.y >= newHeight*GRID_HEIGHT
		   ||links[i].to.x < 0 || links[i].to.x >= newWidth*GRID_WIDTH||links[i].to.y < 0 || links[i].to.y >= newHeight*GRID_HEIGHT)
		{
			if(undo != NULL)
				undo->screenRemove.deadLinks.push_back(oldLink);
			links.erase(links.begin()+i);
			i--;
		}
	}

	if(undo != NULL) undo->screenRemove.spawnBefore = spawnPoint;

	delete[] blocks; blocks = newBlocks; blocks = newBlocks;
	worldWidth = newWidth; worldHeight = newHeight;
	xoffset = Util::clamp(xoffset - xoff, 0, GRID_WIDTH*(worldWidth-1));
	yoffset = Util::clamp(yoffset - yoff, 0, GRID_HEIGHT*(worldHeight-1));
	spawnPoint.x = Util::clamp(spawnPoint.x - xoff, 0, GRID_WIDTH*worldWidth-1);
	spawnPoint.y = Util::clamp(spawnPoint.y - yoff, 0, GRID_HEIGHT*worldHeight-1);
}