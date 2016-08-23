#include "Actors.h"
#include "SharedRes.h"

Player::Player(int X, int Y)
{
	x = (float)X; y = (float)Y; xspeed = 0; yspeed = 0; state = 0;
	texture = SharedRes::Get()->getTexture("player/playerWalk.png");
	texture->setSmooth(true);
	collisionRect = sf::FloatRect(-8, -16, 16, 16);
}

Player::~Player(void)
{
	SharedRes::Get()->subtractTexture(texture);
}

void Player::render(sf::RenderTarget* target)
{
	sf::Sprite spr(*texture); 
	float relPosx = x - ((int)x/(GRID_WIDTH*24))*GRID_WIDTH*24, relPosy = y - ((int)(y-8)/(GRID_HEIGHT*24))*GRID_HEIGHT*24;
	spr.setOrigin(12, 20); spr.setPosition(relPosx, relPosy);
	if(state & ACTSTATE_LOOKING_LEFT) spr.setScale(-1, 1);

	target->draw(spr);
}