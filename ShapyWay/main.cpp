#include "Game.h"
#include "SharedRes.h"

int main(int argc, char** argv)
{
	if(!SharedRes::init()) //Initialization of shared textures and fonts handler
		return 0;

	Game game;
	game.start();
	return 0;
}