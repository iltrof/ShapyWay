#pragma once
#include "Block.h"
#include "Util.h"

#define NUM_DEPTH_LAYERS 3

namespace WorldRenderer
{
	sf::RenderTexture* depthLayers;
	int lastOverviewWidth;
	int lastOverviewHeight;
	sf::RenderTexture* overviewTexture;

	void init()
	{
		depthLayers = new sf::RenderTexture[NUM_DEPTH_LAYERS];
		for(int i = 0; i<NUM_DEPTH_LAYERS; i++)
			depthLayers[i].create(GRID_WIDTH*24, GRID_HEIGHT*24);
		lastOverviewWidth = 0;
		lastOverviewHeight = 0;
		overviewTexture = new sf::RenderTexture;
	}

	void render(Block* blocks, int width, int height, int xoffset, int yoffset, sf::Texture* textures, sf::RenderWindow* window, std::vector<vec2i>& interscreenBlocks)
	{
		for(int i = 0; i<NUM_DEPTH_LAYERS; i++)
			depthLayers[i].clear(sf::Color::Transparent);

		for(int i = -1; i<=GRID_WIDTH; i++)
		{
			for(int j = -1; j<=GRID_HEIGHT; j++)
			{
				int blockID = Util::getBlockID(blocks, width, height, xoffset+i, yoffset+j);
				if(blockID>0)
				{
					sf::RenderTarget* target;
					if(blockID == ID_PRESS) target = &depthLayers[1];
					else if(blockID == ID_LIGHT) target = &depthLayers[2];
					else target = &depthLayers[0];

					blocks[(j+yoffset)*width*GRID_WIDTH + i+xoffset].render(blocks, width, height, xoffset+i, yoffset+j, xoffset, yoffset, textures, target);
				}
			}
		}
		for(vec2i isb : interscreenBlocks)
		{
			if(isb.x < xoffset-1 || isb.x > xoffset+GRID_WIDTH || isb.y < yoffset-1 || isb.y > yoffset+GRID_HEIGHT)
				blocks[isb.y*width*GRID_WIDTH + isb.x].render(blocks, width, height, isb.x, isb.y, xoffset, yoffset, textures, &depthLayers[0]);
		}

		for(int i = 0; i<NUM_DEPTH_LAYERS; i++)
			depthLayers[i].display();

		sf::Sprite depthLayer;
		for(int i = 0; i<NUM_DEPTH_LAYERS; i++)
		{
			depthLayer.setTexture(depthLayers[i].getTexture(), 1);
			window->draw(depthLayer);
		}
	}

	void renderOverview(Block* blocks, int width, int height, int spawnx, int spawny, sf::RenderWindow* window)
	{
		if(lastOverviewWidth != width || lastOverviewHeight != height)
			overviewTexture->create(width*GRID_WIDTH, height*GRID_HEIGHT);
		lastOverviewWidth = width; lastOverviewHeight = height;

		std::vector<sf::Vertex> overview;
		
		for(int i = 0; i<width*GRID_WIDTH; i++)
		{
			for(int j = height*GRID_HEIGHT-1; j>=0; j--)
			{
				sf::Vertex pt({ (float)i, (float)j });
				switch(blocks[j*width*GRID_WIDTH + i].ID)
				{
				case ID_GRASS: 
					pt.color = sf::Color(125, 107, 73); break;
				case ID_LASER: 
					if(blocks[j*width*GRID_WIDTH + i].data & 4)
						pt.color = sf::Color::Red;
					else
						pt.color = sf::Color::Transparent;
					break;
				case ID_LEVER: case ID_PORTAL: case ID_CHECKPOINT:
					pt.color = Util::from8bit(blocks[j*width*GRID_WIDTH + i].data & 255); break;
				case ID_WATER: 
					pt.color = sf::Color(107, 137, 200); break;
				case ID_PRESS: 
					pt.position.y++; pt.color = Util::from8bit(blocks[j*width*GRID_WIDTH + i].data & 255); break;
				case ID_LIGHT:
					if(blocks[j*width*GRID_WIDTH + i].data & 16384)
						pt.color = Util::from8bit(blocks[j*width*GRID_WIDTH + i].data & 255);
					else
						pt.color = sf::Color::Transparent;
					break;
				default:
					pt.color = sf::Color::Transparent;
				}

				if(pt.color.a > 0)
					overview.push_back(pt);
			}
		}
		overview.push_back(sf::Vertex({ (float)spawnx, (float)spawny }, sf::Color::White));

		overviewTexture->draw(&overview[0], overview.size(), sf::Points);

		sf::Sprite ov(overviewTexture->getTexture());
		float scale = std::min(24.f/width, 24.f/height);
		ov.setScale(scale, -scale);
		ov.setPosition(0, WINDOW_HEIGHT);
		window->draw(ov);
	}
}