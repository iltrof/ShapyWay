#pragma once

//util file containing physics and block rendering handlers

#include "Elements.h"
#include "SharedRes.h"
#include "vec2.h"
#include <bitset>
#include <iostream>

#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)
#define ABS(a) (a < 0 ? -a : a)

namespace SharedUtil
{
	Block* getBlock(Block* blocks, int cellx, int celly, int width, int height)
	{
		if(cellx<0) return NULL;
		if(cellx>=width*GRID_WIDTH) return NULL;
		if(celly<0) return NULL;
		if(celly>=height*GRID_HEIGHT) return NULL;

		return &blocks[celly*width*GRID_WIDTH+cellx];
	}

	int getBlockID(Block* blocks, int cellx, int celly, int width, int height)
	{
		Block* bl = getBlock(blocks, cellx, celly, width, height);
		if(bl!=NULL) return bl->ID;
		return -1;
	}

	bool segSegIntersection(const vec2& p, const vec2& r, const vec2& q, const vec2& s, vec2* resultVec, double* resultT) //for 2 line segments from p to p+r and q to q+s
	{
		float t = (q - p) % (s / (r % s)); //p+tr is where the intersection lies
		float u = (q - p) % (r / (r % s)); //it is also q+us
		if(r % s != 0 && t>=0 && t<=1 && u>=0 && u<=1)
		{
			if(resultT) *resultT = t;
			if(resultVec) *resultVec = p+(r*t);
			return true;
		}
		else return false;
	}

	bool compareAngles(vec2 p1, vec2 p2)
	{
		vec2 ref(0, 1);

		if(ref % p2 == 0 && ref * p2 >= 0) return false;
		if(ref % p1 == 0 && ref * p1 >= 0) return true;
		if((ref%p2) * (ref%p1) >= 0) return p1 % p2 > 0;
		return ref % p1 > 0;
	}

	void handlePowerSwitch(Block* blocks, int cellx, int celly, int width, int height, bool powered, std::vector<Link>& links)
	{
		for(unsigned int i = 0; i<links.size(); i++)
		{
			if(cellx == links[i].srcx && celly == links[i].srcy)
			{
				Block& destBlock = blocks[links[i].desty*width*GRID_WIDTH + links[i].destx];
				bool ORlinks = false;
				switch(destBlock.ID)
				{
				case TILEID_LASER: if(destBlock.data & 8) ORlinks = true; break;
				case TILEID_LIGHT: if(destBlock.data & 32768) ORlinks = true; break;
				}

				bool destPowerOn = ORlinks ? false : true;

				for(unsigned int j = 0; j<links.size(); j++)
				{
					if(links[i].destx == links[j].destx && links[i].desty == links[j].desty)
					{
						Block& srcBlock = blocks[links[j].srcy*width*GRID_WIDTH + links[j].srcx];
						switch(srcBlock.ID)
						{
						case TILEID_LEVER: case TILEID_BUTTON:
							if(!ORlinks)
							{
								if(((srcBlock.data & 256) > 0) == links[j].inv)
									destPowerOn = false;
							}
							else
							{
								if(((srcBlock.data & 256) > 0) != links[j].inv)
									destPowerOn = true;
							}
							break;
						}
					}
				}

				switch(destBlock.ID)
				{
				case TILEID_LASER:
					if(destPowerOn) destBlock.data |= 4;
					else destBlock.data &= 65531;
					break;
				case TILEID_LIGHT:
					if(destPowerOn) destBlock.data |= 16384;
					else destBlock.data &= 49151;
					break;
				}
			}
		}
	}

	void interactWithBlock(Block* blocks, int cellx, int celly, int width, int height, std::vector<Link>& links)
	{
		Block& curBlock = blocks[celly*width*GRID_WIDTH+cellx];
		switch(curBlock.ID)
		{
		case TILEID_LEVER:
			curBlock.data ^= 256;
			handlePowerSwitch(blocks, cellx, celly, width, height, (curBlock.data & 256) > 0, links);
			break;
		default: break;
		}
	}
}

namespace Physics
{
	float gravity = 0.5f;
	int jumpCompensation = 2; //how many ticks after stepping off of a ledge the player is still able to jump

	std::vector<vec2> laserPositions;

	void initPhysics(Block* blocks, int width, int height)
	{
		for(int i = 0; i<width*GRID_WIDTH; i++)
		{
			for(int j = 0; j<height*GRID_HEIGHT; j++)
			{
				if(blocks[j*width*GRID_WIDTH+i].ID == TILEID_LASER)
					laserPositions.push_back(vec2((float)i, (float)j));
			}
		}
	}

	inline bool rectRectIntersect(sf::FloatRect rect1, sf::FloatRect rect2) //rectangle-rectangle intersection
	{
		return rect1.left < rect2.left+rect2.width && rect1.left+rect1.width > rect2.left && rect1.top < rect2.top+rect2.height && rect1.top+rect1.height > rect2.top;
	}

	void updateActor(Actor* actor, Block* blocks, int width, int height, std::vector<Link>& links) //updates an actor
	{
		float oldX = actor->x, oldY = actor->y;
		actor->x += actor->xspeed;

		if(actor->xspeed < 0) actor->state |= ACTSTATE_LOOKING_LEFT;
		else if(actor->xspeed > 0 && actor->state & ACTSTATE_LOOKING_LEFT) actor->state ^= ACTSTATE_LOOKING_LEFT;

		int startx = (int)MIN(actor->x/24, oldX/24) - 3, starty = (int)MIN(actor->y/24, oldY/24) - 3;
		int endx = (int)MAX(actor->x/24, oldX/24) + 3, endy = (int)MAX(actor->y/24, oldY/24) + 3;

		sf::FloatRect actorRect(actor->x+actor->collisionRect.left, actor->y+actor->collisionRect.top, actor->collisionRect.width, actor->collisionRect.height);
		for(int i = startx; i<=endx; i++)
		{
			for(int j = starty; j<=endy; j++)
			{
				sf::FloatRect blockRect;
				if(SharedUtil::getBlockID(blocks, i, j, width, height) == TILEID_GRASS)
					blockRect = sf::FloatRect(i*24.f, j*24.f, 24, 24);
				if(blockRect.width == 0 || blockRect.height == 0) continue;

				if(rectRectIntersect(actorRect, blockRect))
				{
					if(oldX < i*24.f+12.f) actor->x = i*24.f-8.f;
					else if(oldX > i*24.f+12.f) actor->x = i*24.f+24.f+8.f;
				}
			}
		}

		actor->yspeed += gravity; actor->y += actor->yspeed;
		
		sf::FloatRect motionRect;
		if(oldY < actor->y)
			motionRect = sf::FloatRect(actor->x+actor->collisionRect.left, oldY+actor->collisionRect.top, actor->collisionRect.width, actor->y-oldY+actor->collisionRect.height);
		else
			motionRect = sf::FloatRect(actor->x+actor->collisionRect.left, actor->y+actor->collisionRect.top, actor->collisionRect.width, oldY-actor->y+actor->collisionRect.height);

		if(actor->state & ACTSTATE_CAN_JUMP)
		{
			if(jumpCompensation == 0)
				actor->state ^= ACTSTATE_CAN_JUMP;
			else
				jumpCompensation--;
		}
		if(actor->state & ACTSTATE_SWIMMING)
			actor->state ^= ACTSTATE_SWIMMING;

		for(int i = startx; i<=endx; i++)
		{
			for(int j = starty; j<=endy; j++)
			{
				sf::FloatRect blockRect;
				int blockID = SharedUtil::getBlockID(blocks, i, j, width, height);
				if(blockID == TILEID_GRASS || blockID == TILEID_WATER || blockID == TILEID_LAVA || blockID == TILEID_ACID)
					blockRect = sf::FloatRect(i*24.f, j*24.f, 24.f, 24.f);
				else if(blockID == TILEID_CHECKPOINT)
					blockRect = sf::FloatRect(i*24.f+5.f, j*24.f, 14.f, 24.f);
				if(blockRect.width == 0 || blockRect.height == 0) continue;

				bool buttonPower = false;

				if(rectRectIntersect(motionRect, blockRect))
				{
					if(blockID == TILEID_GRASS)
					{
						if(oldY < j*24+12)
						{
							if(SharedUtil::getBlockID(blocks, i, j-1, width, height) == TILEID_BUTTON)
							{
								buttonPower = true;
							}

							actor->y = j*24.f;
							actor->state |= ACTSTATE_CAN_JUMP;
							jumpCompensation = 2;
						}
						else if(oldY > j*24.f+12.f) actor->y = j*24.f+24.f+16.f;
						actor->yspeed = 0;
					}
					else if(blockID == TILEID_CHECKPOINT)
					{
						if(!(blocks[j*width*GRID_WIDTH+i].data & 256))
						{
							blocks[j*width*GRID_WIDTH+i].data |= 256;
							Notifications::queue.push(notifON_CHANGE_SPAWN);
							Notifications::queue.push(i);
							Notifications::queue.push(j);
						}
					}
					else if(blockID == TILEID_WATER && rectRectIntersect(actorRect, blockRect))
					{
						actor->state |= ACTSTATE_SWIMMING;
					}
					else if(blockID == TILEID_LAVA && rectRectIntersect(actorRect, blockRect))
					{
						actor->state |= ACTSTATE_SWIMMING;
						Notifications::queue.push(notifON_PLAYER_DEATH);
					}
					else if(blockID == TILEID_ACID && rectRectIntersect(actorRect, blockRect))
					{
						actor->state |= ACTSTATE_SWIMMING;
						Notifications::queue.push(notifON_PLAYER_DEATH);
					}
				}

				if(SharedUtil::getBlockID(blocks, i, j-1, width, height) == TILEID_BUTTON)
				{
					if(((blocks[(j-1)*width*GRID_WIDTH+i].data & 256) > 0) != buttonPower)
					{
						blocks[(j-1)*width*GRID_WIDTH+i].data ^= 256;
						SharedUtil::handlePowerSwitch(blocks, i, j-1, width, height, buttonPower, links);
					}
				}
			}
		}

		if(actor->state & ACTSTATE_SWIMMING)
		{
			if(actor->yspeed > 0)
			{
				if(actor->yspeed <= 1) actor->yspeed = 0;
				actor->yspeed -= gravity/2.f;
				actor->yspeed /= 1.6f;
			}
			else if(actor->yspeed < -4)
			{
				actor->yspeed /= 1.2f;
				if(actor->yspeed > -0.5) actor->yspeed = 0;
			}
		}

		for(vec2 pos : laserPositions)
		{
			Block* laser = SharedUtil::getBlock(blocks, (int)pos.x, (int)pos.y, width, height);
			if(laser->data & 4)
			{
				if(pos.x*24+8 < actorRect.left+actorRect.width && pos.x*24+16 > actorRect.left)
				{
					if((laser->data & 3) == 0 && actorRect.top+actorRect.height <= pos.y*24+24)
					{
						int obstructionY = -1;
						for(int i = (int)pos.y; i>=0; i--)
						{
							if(SharedUtil::getBlockID(blocks, (int)pos.x, i, width, height) == TILEID_GRASS)
							{
								obstructionY = i*24+24;
								break;
							}
						}
						if(obstructionY == -1 || obstructionY < actorRect.top+actorRect.height)
							Notifications::queue.push(notifON_PLAYER_DEATH);
					}
					else if((laser->data & 3) == 2 && actorRect.top >= pos.y*24)
					{
						int obstructionY = -1;
						for(int i = (int)pos.y; i<height*GRID_HEIGHT; i++)
						{
							if(SharedUtil::getBlockID(blocks, (int)pos.x, i, width, height) == TILEID_GRASS)
							{
								obstructionY = i*24;
								break;
							}
						}
						if(obstructionY == -1 || obstructionY > actorRect.top)
							Notifications::queue.push(notifON_PLAYER_DEATH);
					}
				}
				if(pos.y*24+8 < actorRect.top+actorRect.height && pos.y*24+16 > actorRect.top)
				{
					if((laser->data & 3) == 1 && actorRect.left >= pos.x*24)
					{
						int obstructionX = -1;
						for(int i = (int)pos.x; i<width*GRID_WIDTH; i++)
						{
							if(SharedUtil::getBlockID(blocks, i, (int)pos.y, width, height) == TILEID_GRASS)
							{
								obstructionX = i*24;
								break;
							}
						}
						if(obstructionX == -1 || obstructionX > actorRect.left)
							Notifications::queue.push(notifON_PLAYER_DEATH);
					}
					else if((laser->data & 3) == 3 && actorRect.left+actorRect.width <= pos.x*24+24)
					{
						int obstructionX = -1;
						for(int i = (int)pos.x; i>=0; i--)
						{
							if(SharedUtil::getBlockID(blocks, i, (int)pos.y, width, height) == TILEID_GRASS)
							{
								obstructionX = i*24+24;
								break;
							}
						}
						if(obstructionX == -1 || obstructionX < actorRect.left+actorRect.width)
							Notifications::queue.push(notifON_PLAYER_DEATH);
					}
				}
			}
		}
	}
}

namespace BlockRenderer
{
	bool isSolid(int id)
	{
		return id == TILEID_GRASS;
	}

	sf::Color from8bit(int byte) //turns 8-bit rrrgggbb color into proper rgb
	{
		return sf::Color((byte >> 5)*36, ((byte&31)>>2)*36, (byte&3)*85);
	}

	std::bitset<8> getConnect(Block* blocks, int cellx, int celly, int width, int height, int compID = -1)
	{
		if(compID == -1) compID = SharedUtil::getBlockID(blocks, cellx, celly, width, height);
		std::bitset<8> connect;
		connect[0] = (SharedUtil::getBlockID(blocks, cellx, celly-1, width, height) == compID);
		connect[1] = (SharedUtil::getBlockID(blocks, cellx+1, celly-1, width, height) == compID);
		connect[2] = (SharedUtil::getBlockID(blocks, cellx+1, celly, width, height) == compID);
		connect[3] = (SharedUtil::getBlockID(blocks, cellx+1, celly+1, width, height) == compID);
		connect[4] = (SharedUtil::getBlockID(blocks, cellx, celly+1, width, height) == compID);
		connect[5] = (SharedUtil::getBlockID(blocks, cellx-1, celly+1, width, height) == compID);
		connect[6] = (SharedUtil::getBlockID(blocks, cellx-1, celly, width, height) == compID);
		connect[7] = (SharedUtil::getBlockID(blocks, cellx-1, celly-1, width, height) == compID);
		return connect;
	}

	std::bitset<8> getSolidConnect(Block* blocks, int cellx, int celly, int width, int height)
	{
		std::bitset<8> connect;
		connect[0] = (isSolid(SharedUtil::getBlockID(blocks, cellx, celly-1, width, height)));
		connect[1] = (isSolid(SharedUtil::getBlockID(blocks, cellx+1, celly-1, width, height)));
		connect[2] = (isSolid(SharedUtil::getBlockID(blocks, cellx+1, celly, width, height)));
		connect[3] = (isSolid(SharedUtil::getBlockID(blocks, cellx+1, celly+1, width, height)));
		connect[4] = (isSolid(SharedUtil::getBlockID(blocks, cellx, celly+1, width, height)));
		connect[5] = (isSolid(SharedUtil::getBlockID(blocks, cellx-1, celly+1, width, height)));
		connect[6] = (isSolid(SharedUtil::getBlockID(blocks, cellx-1, celly, width, height)));
		connect[7] = (isSolid(SharedUtil::getBlockID(blocks, cellx-1, celly-1, width, height)));
		return connect;
	}

	sf::IntRect getTexRect(std::bitset<8> connect)
	{
		if(!connect[0]) { connect[1] = 0; connect[7] = 0; }
		if(!connect[2]) { connect[3] = 0; connect[1] = 0; }
		if(!connect[4]) { connect[5] = 0; connect[3] = 0; }
		if(!connect[6]) { connect[7] = 0; connect[5] = 0; }

		int resx, resy;
		switch(connect.to_ulong())
		{
		case 0: resx = 0; resy = 0; break;
		case 4: resx = 1; resy = 0; break;
		case 68: resx = 2; resy = 0; break;
		case 64: resx = 3; resy = 0; break;
		case 20: resx = 4; resy = 0; break;
		case 80: resx = 5; resy = 0; break;
		case 21: resx = 6; resy = 0; break;
		case 84: resx = 7; resy = 0; break;
		case 87: resx = 8; resy = 0; break;
		case 93: resx = 9; resy = 0; break;
		case 245: resx = 10; resy = 0; break;
		case 215: resx = 11; resy = 0; break;
		case 16: resx = 0; resy = 1; break;
		case 28: resx = 1; resy = 1; break;
		case 124: resx = 2; resy = 1; break;
		case 112: resx = 3; resy = 1; break;
		case 5: resx = 4; resy = 1; break;
		case 65: resx = 5; resy = 1; break;
		case 69: resx = 6; resy = 1; break;
		case 81: resx = 7; resy = 1; break;
		case 213: resx = 8; resy = 1; break;
		case 117: resx = 9; resy = 1; break;
		case 125: resx = 10; resy = 1; break;
		case 95: resx = 11; resy = 1; break;
		case 17: resx = 0; resy = 2; break;
		case 31: resx = 1; resy = 2; break;
		case 255: resx = 2; resy = 2; break;
		case 241: resx = 3; resy = 2; break;
		case 29: resx = 4; resy = 2; break;
		case 116: resx = 5; resy = 2; break;
		case 23: resx = 6; resy = 2; break;
		case 92: resx = 7; resy = 2; break;
		case 247: resx = 8; resy = 2; break;
		case 223: resx = 9; resy = 2; break;
		case 119: resx = 10; resy = 2; break;
		case 221: resx = 11; resy = 2; break;
		case 1: resx = 0; resy = 3; break;
		case 7: resx = 1; resy = 3; break;
		case 199: resx = 2; resy = 3; break;
		case 193: resx = 3; resy = 3; break;
		case 71: resx = 4; resy = 3; break;
		case 209: resx = 5; resy = 3; break;
		case 197: resx = 6; resy = 3; break;
		case 113: resx = 7; resy = 3; break;
		case 253: resx = 8; resy = 3; break;
		case 127: resx = 9; resy = 3; break;
		case 85: resx = 10; resy = 3; break;
		default: resx = 0; resy = 0; break;
		}

		return sf::IntRect(resx*32, resy*32, 32, 32);
	}

	void renderLight(Block* blocks, int width, int height, int lightx, int lighty, int x, int y, sf::Color color, float radius, sf::RenderTarget* target)
	{
		std::vector<std::pair<vec2, vec2>> lines;
		std::vector<vec2> points;

		int sn = 15; //number of sides of the light circle
		for(int j=0; j<sn; j++)
		{
			double rads=j*(360/sn); rads=rads/180*3.14159265359;
			vec2 p1 = vec2(radius*sinf((float)rads)+lightx, radius*cosf((float)rads)+lighty);
			double rads2=(j+1)*(360/sn); rads2=rads2/180*3.14159265359;
			vec2 p2 = vec2(radius*sinf((float)rads2)+lightx, radius*cosf((float)rads2)+lighty);
		
			lines.push_back(std::pair<vec2, vec2>(p1, p2));
			points.push_back(p1);
		}
		
		{
			std::vector<vec2> rays;
			for(int i = int((lightx - radius)/24.f); i < int((lightx + radius)/24.f); i++)
			{
				for(int j = int((lighty - radius)/24.f); j < int((lighty + radius)/24.f); j++)
				{
					if(isSolid(SharedUtil::getBlockID(blocks, i, j, width, height)))
					{
						if(!isSolid(SharedUtil::getBlockID(blocks, i, j-1, width, height)) && lighty <= j*24)
							lines.push_back(std::pair<vec2, vec2>(vec2(i*24.f, j*24.f), vec2(i*24.f+24.f, j*24.f)));
						if(!isSolid(SharedUtil::getBlockID(blocks, i+1, j, width, height)) && lightx >= i*24+24)
							lines.push_back(std::pair<vec2, vec2>(vec2(i*24.f+24.f, j*24.f), vec2(i*24.f+24.f, j*24.f+24.f)));
						if(!isSolid(SharedUtil::getBlockID(blocks, i, j+1, width, height)) && lighty >= j*24+24)
							lines.push_back(std::pair<vec2, vec2>(vec2(i*24.f+24.f, j*24.f+24.f), vec2(i*24.f, j*24.f+24.f)));
						if(!isSolid(SharedUtil::getBlockID(blocks, i-1, j, width, height)) && lightx <= i*24)
							lines.push_back(std::pair<vec2, vec2>(vec2(i*24.f, j*24.f+24.f), vec2(i*24.f, j*24.f)));

						auto con = getSolidConnect(blocks, i, j, width, height);
						if(!con[0]&&!con[2]) { points.push_back(vec2(i*24.f+24.f, j*24.f)); rays.push_back(vec2(i*24.f+24.f, j*24.f-0.5f)); rays.push_back(vec2(i*24.f+24.f+0.5f, j*24.f)); }
						if(!con[2]&&!con[4]) { points.push_back(vec2(i*24.f+24.f, j*24.f+24.f)); rays.push_back(vec2(i*24.f+24.f, j*24.f+24.f+0.5f)); rays.push_back(vec2(i*24.f+24.f+0.5f, j*24.f+24.f)); }
						if(!con[4]&&!con[6]) { points.push_back(vec2(i*24.f, j*24.f+24.f)); rays.push_back(vec2(i*24.f, j*24.f+24.f+0.5f)); rays.push_back(vec2(i*24.f-0.5f, j*24.f+24.f)); }
						if(!con[6]&&!con[0]) { points.push_back(vec2(i*24.f, j*24.f)); rays.push_back(vec2(i*24.f, j*24.f-0.5f)); rays.push_back(vec2(i*24.f-0.5f, j*24.f)); }

						if(con[0]&&con[2]&&!con[1]) points.push_back(vec2(i*24.f+24.f, j*24.f));
						if(con[2]&&con[4]&&!con[3]) points.push_back(vec2(i*24.f+24.f, j*24.f+24.f));
						if(con[4]&&con[6]&&!con[5]) points.push_back(vec2(i*24.f, j*24.f+24.f));
						if(con[6]&&con[0]&&!con[7]) points.push_back(vec2(i*24.f, j*24.f));
					}
				}
			}

			//for(Actor* actor : actors)
			//{
			//	float left = actor->collisionRect.left, top = actor->collisionRect.top, right = left+actor->collisionRect.width, bottom = top+actor->collisionRect.height;
			//	left += actor->x; right += actor->x; top += actor->y; bottom += actor->y;

			//	points.push_back(vec2(right, top)); rays.push_back(vec2(right, top-0.5)); rays.push_back(vec2(right+0.5, top));
			//	points.push_back(vec2(right, bottom)); rays.push_back(vec2(right, bottom+0.5)); rays.push_back(vec2(right+0.5, bottom));
			//	points.push_back(vec2(left, bottom)); rays.push_back(vec2(left, bottom+0.5)); rays.push_back(vec2(left-0.5, bottom));
			//	points.push_back(vec2(left, top)); rays.push_back(vec2(left, top-0.5)); rays.push_back(vec2(left-0.5, top));

			//	lines.push_back(std::pair<vec2, vec2>(vec2(right, top), vec2(right, bottom)));
			//	lines.push_back(std::pair<vec2, vec2>(vec2(right, bottom), vec2(left, bottom)));
			//	lines.push_back(std::pair<vec2, vec2>(vec2(left, bottom), vec2(left, top)));
			//	lines.push_back(std::pair<vec2, vec2>(vec2(left, top), vec2(right, top)));
			//}

			for(vec2& ray : rays)
			{
				if((vec2((float)lightx, (float)lighty)-ray).magsqr() < radius*radius+0.1)
					points.push_back(vec2((float)lightx, (float)lighty) + (ray-vec2((float)lightx, (float)lighty)).normalized() * radius);
			}
		}

		for(unsigned int i = 0; i<points.size(); i++)
		{
			if((vec2((float)lightx, (float)lighty)-points[i]).magsqr() > radius*radius+0.1)
			{
				points.erase(points.begin()+i); i--;
			}
		}

		for(unsigned int i = sn; i < lines.size(); i++)
		{
			for(int j = 0; j<sn; j++)
			{
				vec2 tempRes(0, 0);
				if(SharedUtil::segSegIntersection(lines[i].first, lines[i].second-lines[i].first, lines[j].first, lines[j].second-lines[j].first, &tempRes, NULL))
					points.push_back(tempRes);
			}
		}
		lines.erase(lines.begin(), lines.begin()+sn);

		for(vec2& point : points)
		{
			double minT = 1.f, tempT;
			for(auto& line : lines)
			{
				if(SharedUtil::segSegIntersection(vec2((float)lightx, (float)lighty), point-vec2((float)lightx, (float)lighty), line.first, line.second-line.first, NULL, &tempT))
				{
					if(tempT < minT) minT = tempT;
				}
			}
			point = (point-vec2((float)lightx, (float)lighty))*(float)minT;
		}

		std::sort(points.begin(), points.end(), SharedUtil::compareAngles);
		
		std::vector<sf::Vertex> result;
		result.push_back(sf::Vertex(sf::Vector2f((float)x, (float)y), sf::Color(color.r, color.g, color.b, 128)));
		for(unsigned int j = 0; j<=points.size(); j++)
		{
			vec2 pt = points[j%points.size()];
			int alpha = int((1-pt.mag()/(radius))*128);
			result.push_back(sf::Vertex(sf::Vector2f(x+pt.x, y+pt.y), sf::Color(color.r, color.g, color.b, alpha)));
		}
		
		target->draw(&result[0], result.size(), sf::TrianglesFan);
	}

	void render(Block* blocks, int cellx, int celly, int width, int height, int x, int y, sf::Texture* texture, sf::RenderTarget* target)
	{
		Block& curBlock = blocks[celly*width*GRID_WIDTH+cellx];
		switch(curBlock.ID)
		{
		case TILEID_AIR:
			break;
		case TILEID_GRASS: {
			sf::Sprite spr(*texture); spr.setTextureRect(getTexRect(getConnect(blocks, cellx, celly, width, height)));
			spr.setOrigin(4, 4); spr.setPosition((float)x, (float)y);
			target->draw(spr);
		} break;
		case TILEID_LASER: { //........ .....POO; O: orientation; P: power;
			if(((curBlock.data & 3) == 0 || (curBlock.data & 3) == 2)&&(x < -24 || x > WINDOW_WIDTH+24))
				break;
			if(((curBlock.data & 3) == 1 || (curBlock.data & 3) == 3)&&(y < -24 || y > WINDOW_HEIGHT+24))
				break;

			sf::Sprite spr(*texture);

			spr.setOrigin(16, 16);
			spr.setTextureRect(sf::IntRect(64, 0, 32, 32));

			int xinc = 0, yinc = 0;
			switch(curBlock.data & 3) //orientation
			{
			case 0: spr.setRotation(-90); yinc = -1; break; //UP
			case 1: spr.setRotation(0); xinc = 1; break; //RIGHT
			case 2: spr.setRotation(90); yinc = 1; break; //DOWN
			case 3: spr.setRotation(180); xinc = -1; break; //LEFT
			}

			if(curBlock.data & 4) //power
			{
				for(int curcx = cellx, curcy = celly; !isSolid(SharedUtil::getBlockID(blocks, curcx, curcy, width, height)); curcx += xinc, curcy += yinc)
				{
					spr.setPosition(x+12.f+24.f*(curcx-cellx), y+12.f+24.f*(curcy-celly));
					target->draw(spr);
				}
			}

			if(curBlock.data & 4) spr.setTextureRect(sf::IntRect(32, 0, 32, 32));
			else spr.setTextureRect(sf::IntRect(0, 0, 32, 32));
			spr.setPosition(x+12.f, y+12.f);

			target->draw(spr);
		} break;
		case TILEID_LEVER: { //.......P CCCCCCCC; C: color; P: power;
			sf::Sprite spr(*texture); spr.setOrigin(16, 16); spr.setPosition(x+12.f, y+12.f);
			spr.setTextureRect(sf::IntRect(0, 0, 32, 32));

			spr.setScale(curBlock.data & 256 ? -1.f : 1.f, 1.f);
			target->draw(spr);
			spr.setTextureRect(sf::IntRect(curBlock.data & 256 ? 64 : 32, 0, 32, 32));
			spr.setColor(from8bit(curBlock.data & 255));
			spr.setScale(1, 1);
			target->draw(spr);

			//if(curBlock.data & 256)
			//	renderLight(blocks, width, height, cellx*24+18, celly*24+12, x+18, y+12, from8bit(curBlock.data & 255), 36, window);
		} break;
		case TILEID_WATER: case TILEID_LAVA: case TILEID_ACID: {
			bool top = false;
			if(SharedUtil::getBlockID(blocks, cellx, celly-1, width, height)==curBlock.ID)
				top = false;
			else if(!isSolid(SharedUtil::getBlockID(blocks, cellx, celly-1, width, height)))
				top = true;
			else
			{
				for(int curcx = cellx; SharedUtil::getBlockID(blocks, curcx, celly, width, height)==curBlock.ID; curcx--)
				{
					if(!isSolid(SharedUtil::getBlockID(blocks, curcx, celly-1, width, height)) && SharedUtil::getBlockID(blocks, curcx, celly-1, width, height)!=curBlock.ID)
						top = true;
				}
				for(int curcx = cellx; SharedUtil::getBlockID(blocks, curcx, celly, width, height)==curBlock.ID; curcx++)
				{
					if(!isSolid(SharedUtil::getBlockID(blocks, curcx, celly-1, width, height)) && SharedUtil::getBlockID(blocks, curcx, celly-1, width, height)!=curBlock.ID)
						top = true;
				}
			}

			sf::Sprite spr(*texture); spr.setOrigin(4, 4); spr.setPosition((float)x, (float)y);
			spr.setTextureRect(sf::IntRect(top ? 0 : 32, 0, 32, 32));
			target->draw(spr);
		} break;
		case TILEID_BUTTON: { //.......P CCCCCCCC; C: color; P: power;
			sf::Sprite spr(*texture); spr.setOrigin(16, 16); spr.setPosition(x+12.f, y+12.f);
			spr.setTextureRect(sf::IntRect(0, 0, 32, 32));
			target->draw(spr);
			spr.setTextureRect(sf::IntRect(curBlock.data & 256 ? 64 : 32, 0, 32, 32));
			spr.setColor(from8bit(curBlock.data & 255));
			target->draw(spr);
		} break;
		case TILEID_PORTAL: { //........ CCCCCCCC; C: color;
			sf::Sprite spr(*texture); spr.setPosition(x-4.f, y-4.f);
			target->draw(spr);
		} break;
		case TILEID_LIGHT: { //.POORRRR CCCCCCCC; C: color; R: light radius; O: orientation; P: power;
			sf::Sprite spr(*texture); spr.setOrigin(16, 16); spr.setPosition(x+12.f, y+12.f);
			int xoff = 12, yoff = 12, radius = ((curBlock.data & 15360)>>10)*24;
			if(x < -24-radius || x > WINDOW_WIDTH+24+radius || y < -24-radius || y > WINDOW_HEIGHT+24+radius)
				break;

			switch((curBlock.data & 768)>>8) //orientation
			{
			case 0: spr.setRotation(-90); yoff = 17; break; //UP
			case 1: spr.setRotation(0); xoff = 7; break; //RIGHT
			case 2: spr.setRotation(90); yoff = 7; break; //DOWN
			case 3: spr.setRotation(180); xoff = 17; break; //LEFT
			}
			target->draw(spr);
			if(curBlock.data & 16384) //power
				renderLight(blocks, width, height, cellx*24+xoff, celly*24+yoff, x+xoff, y+yoff, from8bit(curBlock.data & 255), (float)radius, target);
		} break;
		case TILEID_CHECKPOINT: { //........ .......R; R: raised;
			sf::Sprite spr(*texture); spr.setPosition(x-4.f, y-4.f);
			spr.setTextureRect(sf::IntRect(0, 0, 32, 32));
			target->draw(spr);
			spr.setTextureRect(sf::IntRect(32, 0, 32, 32));
			spr.setColor(from8bit(curBlock.data & 255));
			if(!(curBlock.data & 256))
				spr.setPosition(x-4.f, y+6.f);
			target->draw(spr);
		}
		default:
			break;
		}
	}
}