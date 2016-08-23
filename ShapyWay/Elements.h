#pragma once

#include <SFML\Graphics.hpp>

//List of tiles' IDs
#define TILEID_AIR 0 
#define TILEID_GRASS 1
#define TILEID_LASER 2
#define TILEID_LEVER 3
#define TILEID_WATER 4
#define TILEID_LAVA 5
#define TILEID_ACID 6
#define TILEID_BUTTON 7
#define TILEID_PORTAL 8
#define TILEID_LIGHT 9
#define TILEID_CHECKPOINT 10

//The class for immovable blocks
struct Block 
{
	Block() : ID(0), data(0) {}
	Block(int ID, int data) : ID(ID), data(data) {}
	int ID;
	int data; //At least 16 bits of data containing information about the block
};

//The class for movable objects
struct Object
{
	virtual ~Object() {}

	virtual void render(sf::RenderTarget* target) {}
	virtual void update() {}

	float x, xspeed;
	float y, yspeed;
	sf::Texture* texture;

	sf::FloatRect collisionRect;
};

//Flags for actors' "state" variables
#define ACTSTATE_LOOKING_LEFT 1
#define ACTSTATE_CAN_JUMP 2
#define ACTSTATE_SWIMMING 4

//The class for actors
struct Actor : public Object
{
	virtual ~Actor() {}

	int health;
	int state; //At least 16 flags for each actor
};

//The class for links (connections between switches and switchable things)
struct Link
{
	Link() {}
	Link(int sx, int sy, int dx, int dy, bool inv = false)
		: srcx(sx), srcy(sy), destx(dx), desty(dy), inv(inv) {}

	int srcx, srcy; //switch
	int destx, desty; //switchable thing
	bool inv; //inverse connection
};