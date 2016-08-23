#include <bitset>

#include "Block.h"
#include "Util.h"

Block::Block()
{
	ID = 0;
	data = 0;
}

Block::Block(int ID, int data)
{
	this->ID = ID;
	this->data = data;
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

void Block::render(Block* blocks, int width, int height, int cellx, int celly, int xoffset, int yoffset, sf::Texture* textures, sf::RenderTarget* target)
{
	sf::Sprite spr;
	spr.setTexture(textures[ID]);

	float xonscreen = (cellx-xoffset)*24+12.f, yonscreen = (celly-yoffset)*24+12.f;
	spr.setPosition(xonscreen, yonscreen);
	spr.setOrigin(16, 16);
	spr.setRotation(0);

	switch(ID)
	{
	case ID_GRASS:
	{
		std::bitset<8> connect;

		connect[0] = Util::getBlockID(blocks, width, height, cellx, celly-1) == ID;
		connect[1] = Util::getBlockID(blocks, width, height, cellx+1, celly-1) == ID;
		connect[2] = Util::getBlockID(blocks, width, height, cellx+1, celly) == ID;
		connect[3] = Util::getBlockID(blocks, width, height, cellx+1, celly+1) == ID;
		connect[4] = Util::getBlockID(blocks, width, height, cellx, celly+1) == ID;
		connect[5] = Util::getBlockID(blocks, width, height, cellx-1, celly+1) == ID;
		connect[6] = Util::getBlockID(blocks, width, height, cellx-1, celly) == ID;
		connect[7] = Util::getBlockID(blocks, width, height, cellx-1, celly-1) == ID;

		spr.setTextureRect(getTexRect(connect));
		target->draw(spr);
	} break;
	case ID_LASER:
	{
		if(((data & 3) == 0 || (data & 3) == 2)&&(xonscreen < -12 || xonscreen > WINDOW_WIDTH+12))
			break;
		if(((data & 3) == 1 || (data & 3) == 3)&&(yonscreen < -12 || yonscreen > WINDOW_HEIGHT+12))
			break;

		spr.setTextureRect(sf::IntRect(64, 0, 32, 32));

		int xinc = 0, yinc = 0;
		switch(data & 3) //orientation
		{
		case 0: spr.setRotation(-90); yinc = -1; break; //UP
		case 1: spr.setRotation(0); xinc = 1; break; //RIGHT
		case 2: spr.setRotation(90); yinc = 1; break; //DOWN
		case 3: spr.setRotation(180); xinc = -1; break; //LEFT
		}

		if(data & 4) //power
		{
			for(int curcx = cellx, curcy = celly; !Util::isSolid(Util::getBlockID(blocks, width, height, curcx, curcy)); curcx += xinc, curcy += yinc)
			{
				spr.setPosition(xonscreen+24*(curcx-cellx), yonscreen+24*(curcy-celly));
				target->draw(spr);
			}
		}

		if(data & 4) spr.setTextureRect(sf::IntRect(32, 0, 32, 32));
		else spr.setTextureRect(sf::IntRect(0, 0, 32, 32));

		spr.setPosition(xonscreen, yonscreen);
		target->draw(spr);
	} break;
	case ID_LEVER:
	{
		spr.setTextureRect(sf::IntRect(0, 0, 32, 32));

		spr.setScale(data & 256 ? -1.f : 1.f, 1.f);
		target->draw(spr);
		spr.setTextureRect(sf::IntRect(data & 256 ? 64 : 32, 0, 32, 32));
		spr.setColor(Util::from8bit(data & 255));
		spr.setScale(1, 1);
		target->draw(spr);
	} break;
	case ID_WATER: case ID_LAVA: case ID_ACID:
	{
		bool top = false;
		if(Util::getBlockID(blocks, width, height, cellx, celly-1) == ID)
			top = false;
		else if(!Util::isSolid(Util::getBlockID(blocks, width, height, cellx, celly-1)))
			top = true;
		else
		{
			for(int curcx = cellx; Util::getBlockID(blocks, width, height, curcx, celly) == ID; curcx--)
			{
				if(!Util::isSolid(Util::getBlockID(blocks, width, height, curcx, celly-1)) && Util::getBlockID(blocks, width, height, curcx, celly-1) != ID)
					top = true;
			}
			for(int curcx = cellx; Util::getBlockID(blocks, width, height, curcx, celly) == ID; curcx++)
			{
				if(!Util::isSolid(Util::getBlockID(blocks, width, height, curcx, celly-1)) && Util::getBlockID(blocks, width, height, curcx, celly-1) != ID)
					top = true;
			}
		}

		spr.setTextureRect(sf::IntRect(top ? 0 : 32, 0, 32, 32));
		target->draw(spr);
	} break;
	case ID_PRESS:
	{
		spr.setTextureRect(sf::IntRect(0, 0, 32, 32));
		target->draw(spr);
		spr.setTextureRect(sf::IntRect(data & 256 ? 64 : 32, 0, 32, 32));
		spr.setColor(Util::from8bit(data & 255));
		target->draw(spr);
	} break;
	case ID_PORTAL:
		spr.setColor(Util::from8bit(data & 255));
		target->draw(spr);
		break;
	case ID_LIGHT:
	{
		int xoff = 0, yoff = 0; float radius = ((data & 15360)>>10)*24.f;
		if(xonscreen < -12-radius || xonscreen > WINDOW_WIDTH+12+radius || yonscreen < -12-radius || yonscreen > WINDOW_HEIGHT+12+radius)
			break;

		switch((data & 768)>>8) //orientation
		{
		case 0: spr.setRotation(-90); yoff = 5; break; //UP
		case 1: spr.setRotation(0); xoff = -5; break; //RIGHT
		case 2: spr.setRotation(90); yoff = -5; break; //DOWN
		case 3: spr.setRotation(180); xoff = 5; break; //LEFT
		}
		spr.setColor(Util::from8bit(data & 255));
		target->draw(spr);
		if(data & 16384) //power
		{
			sf::CircleShape light(radius);
			sf::Color lCol = Util::from8bit(data & 255); lCol.a = 128;
			light.setOutlineColor(lCol); light.setOutlineThickness(1);
			light.setFillColor(sf::Color(lCol.r, lCol.g, lCol.b, 64));
			light.setPosition(xonscreen+xoff-radius, yonscreen+yoff-radius);
			target->draw(light);
		}
	} break;
	case ID_CHECKPOINT:
	{
		spr.setTextureRect(sf::IntRect(0, 0, 32, 32));
		target->draw(spr);
		spr.setTextureRect(sf::IntRect(32, 0, 32, 32));
		spr.setColor(Util::from8bit(data & 255));
		if(!(data & 256))
			spr.setPosition(xonscreen, yonscreen+10);
		target->draw(spr);
	} break;
	}
}