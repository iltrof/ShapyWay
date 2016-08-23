#include "Level.h"
#include "vec2.h"
#include "notifications.h"
#include "Util.h"
#include "Actors.h"
#include "SoundSystem.h"
#include "../SWEditor/VersionControl.h"

#include <iostream>
#include <algorithm>

void Level::loadMap(std::string mapName)
{
	delete[] blocks; for(int i = 0; i<actors.size(); i++) delete actors[i]; actors.clear();
	std::ifstream file; file.open(SharedRes::programDirectory+mapName);
	std::string mapVersion;
	file >> mapVersion;
	file >> width >> height;
	blocks = new Block[width*height*GRID_WIDTH*GRID_HEIGHT];

	for(int i = 0; i<width*height*GRID_WIDTH*GRID_HEIGHT;)
	{
		int cnt; file >> cnt;
		if(cnt == -1) break;
		std::string id; file >> id;
		int curID, curData;
		if(id.find(':')==id.npos)
		{
			curID = atoi(id.c_str());
			curData = 0;
		}
		else
		{
			curID = atoi(id.substr(0, id.find(':')).c_str());
			curData = atoi(id.substr(id.find(':')+1).c_str());
		}
		for(int j = 0; j<cnt; i++, j++)
		{
			updateBlock(mapVersion, curID, curData, blocks[i].ID, blocks[i].data);
			if(curID == TILEID_LASER || curID == TILEID_LIGHT)
				transscreenBlocks.push_back(sf::Vector2i(i%(width*GRID_WIDTH), i/(width*GRID_WIDTH)));
		}
	}

	file >> spawnPointX >> spawnPointY;

	int linksSize; file >> linksSize;
	for(int i = 0; i<linksSize; i++)
	{
		Link newLink;
		file >> newLink.srcx >> newLink.srcy >> newLink.destx >> newLink.desty >> newLink.inv;
		newLink.inv = !newLink.inv;

		links.push_back(newLink);
	}

	player = new Player(spawnPointX*24+12, spawnPointY*24+24);
	actors.push_back(player);

	file.close();
}

Level::Level(std::string mapName)
{
	SoundSystem::stopMusic();

	textures[TILEID_AIR] = NULL;
	textures[TILEID_GRASS] = SharedRes::getTexture("tiles/grass.png");
	textures[TILEID_LASER] = SharedRes::getTexture("tiles/laser.png");
	textures[TILEID_LEVER] = SharedRes::getTexture("tiles/lever.png");
	textures[TILEID_WATER] = SharedRes::getTexture("tiles/water.png");
	textures[TILEID_LAVA] = SharedRes::getTexture("tiles/lava.png");
	textures[TILEID_ACID] = SharedRes::getTexture("tiles/acid.png");
	textures[TILEID_BUTTON] = SharedRes::getTexture("tiles/press.png");
	textures[TILEID_PORTAL] = SharedRes::getTexture("tiles/portal.png");
	textures[TILEID_LIGHT] = SharedRes::getTexture("tiles/light.png");
	textures[TILEID_CHECKPOINT] = SharedRes::getTexture("tiles/checkpoint.png");

	blocks = NULL; player = NULL;
	this->mapName = mapName;
	loadMap(mapName);

	depthLayers = new sf::RenderTexture[NUM_DEPTH_LAYERS];
	for(int i = 0; i<NUM_DEPTH_LAYERS; i++)
		depthLayers[i].create(GRID_WIDTH*24, GRID_HEIGHT*24);

	Physics::initPhysics(blocks, width, height);
}

Level::~Level()
{
	delete[] blocks;
	for(auto tex : textures)
		SharedRes::subtractTexture(tex.second);
	for(int i = 0; i<actors.size(); i++)
		delete actors[i];

	delete[] depthLayers;
}

#define isPressed(a) sf::Keyboard::isKeyPressed(sf::Keyboard::a)

void Level::update(sf::Vector2i mousePos)
{
	float playerXspeed;
	playerXspeed = 4*((isPressed(D)||isPressed(Right))-(isPressed(A)||isPressed(Left)));
	if(player->xspeed >= -4 && player->xspeed <= 4) player->xspeed = playerXspeed;

	if(isPressed(W)||isPressed(Space)||isPressed(Up))
	{
		if(player->state & ACTSTATE_CAN_JUMP)
		{
			if(!(player->state & ACTSTATE_SWIMMING)) player->yspeed = -10;
			else player->yspeed = -5;
			player->state ^= ACTSTATE_CAN_JUMP;
		}
		else if(player->state & ACTSTATE_SWIMMING)
		{
			if(player->yspeed > -2 && player->yspeed < 1)
				player->yspeed -= 1.5;
		}
	}
	else if(isPressed(S)||isPressed(Down))
	{
		if(player->state & ACTSTATE_SWIMMING)
			if(player->yspeed < 1)
				player->yspeed = 1;
	}

	for(int i = 0; i<actors.size(); i++)
		Physics::updateActor(actors[i], blocks, width, height, links);
}

void Level::input(sf::Event& e)
{
	if(e.type == sf::Event::KeyPressed)
	{
		switch(e.key.code)
		{
		case sf::Keyboard::Escape:
			Notifications::queue.push(notifGOTO_MAIN_MENU);
			break;
		case sf::Keyboard::E:
			SharedUtil::interactWithBlock(blocks, player->x/24, (player->y-8)/24, width, height, links);
			break;
		case sf::Keyboard::P:
			Notifications::queue.push(notifPAUSE);
			break;
		}
	}
}

void Level::render(sf::RenderTarget* target)
{
	std::vector<sf::Vertex> vv;
	vv.push_back(sf::Vertex(sf::Vector2f(0, 0), sf::Color(10,32,48)));
	vv.push_back(sf::Vertex(sf::Vector2f(target->getSize().x, 0), sf::Color(10,32,48)));
	vv.push_back(sf::Vertex(sf::Vector2f(target->getSize().x, target->getSize().y), sf::Color(10, 10, 32)));
	vv.push_back(sf::Vertex(sf::Vector2f(0, target->getSize().y), sf::Color(10, 10, 32)));
	target->draw(&vv[0],4,sf::Quads);
	
	for(int i = 0; i<actors.size(); i++)
	{
		actors[i]->render(target);
	}

	int screenX = player->x / WINDOW_WIDTH, screenY = (player->y-8) / WINDOW_HEIGHT;

	for(int i = 0; i<NUM_DEPTH_LAYERS; i++)
		depthLayers[i].clear(sf::Color::Transparent);

	for(int j = -1; j<=GRID_HEIGHT; j++)
	{
		for(int i = -1; i<=GRID_WIDTH; i++)
		{
			int blID = SharedUtil::getBlockID(blocks, i+screenX*GRID_WIDTH, j+screenY*GRID_HEIGHT, width, height);
			if(blID > 0)
			{
				sf::RenderTarget* target;
				if(blID == TILEID_BUTTON) target = &depthLayers[1];
				else if(blID == TILEID_LIGHT) target = &depthLayers[2];
				else target = &depthLayers[0];
				BlockRenderer::render(blocks, i+screenX*GRID_WIDTH, j+screenY*GRID_HEIGHT, width, height, i*24, j*24, textures[blocks[(j+screenY*GRID_HEIGHT)*width*GRID_WIDTH+i+screenX*GRID_WIDTH].ID], target);
			}
		}
	}
	for(sf::Vector2i tsb : transscreenBlocks)
	{
		if(tsb.x < screenX*GRID_WIDTH-1 || tsb.x >(screenX+1)*GRID_WIDTH || tsb.y < screenY*GRID_HEIGHT-1 || tsb.y >(screenY+1)*GRID_HEIGHT)
			BlockRenderer::render(blocks, tsb.x, tsb.y, width, height, (tsb.x-screenX*GRID_WIDTH)*24, (tsb.y-screenY*GRID_HEIGHT)*24, textures[blocks[tsb.y*width*GRID_WIDTH + tsb.x].ID], &depthLayers[0]);
			//blocks[tsb.y*width*GRID_WIDTH + tsb.x].render(tsb.x, isb.y, &depthLayers[0]);
	}

	sf::Sprite depthLayer;
	for(int i = 0; i<NUM_DEPTH_LAYERS; i++)
	{
		depthLayers[i].display();
		depthLayer.setTexture(depthLayers[i].getTexture(), 1);
		target->draw(depthLayer);
	}
}

void Level::handleNotification()
{
	if(Notifications::queue.front() == notifON_PLAYER_DEATH)
	{
		player->x = spawnPointX*24+12;
		player->y = spawnPointY*24+24;
		player->xspeed = 0;
		player->yspeed = 0;
		player->state = 0;
	}
	else if(Notifications::queue.front() == notifON_CHANGE_SPAWN)
	{
		if(blocks[spawnPointY*width*GRID_WIDTH + spawnPointX].ID == TILEID_CHECKPOINT)
			blocks[spawnPointY*width*GRID_WIDTH + spawnPointX].data &= 65279;

		Notifications::queue.pop();
		spawnPointX = Notifications::queue.front();
		Notifications::queue.pop();
		spawnPointY = Notifications::queue.front();
	}
	else if(Notifications::queue.front() == notifRESTART)
	{
		loadMap(mapName);
	}
}