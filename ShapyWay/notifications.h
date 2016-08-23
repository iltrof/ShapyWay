#include <queue>

//notifications can be sent from any point of the program using Notifications::queue.push(int)
//the listener is located in Game.cpp

//notifications list
enum notifications {notifNO_NOTIFICATION, notifEXIT_GAME, notifGOTO_LEVEL_SELECT, notifGOTO_MAIN_MENU, 
	notifGOTO_OPTIONS_MENU, notifGOTO_SKIN_EDITOR, notifPAUSE, notifRESTART, notifPOP_SCENE, notifON_PLAYER_DEATH,
	notifON_CHANGE_SPAWN, notifUPDATE_SETTINGS};

struct Notifications
{
	static std::queue<int> queue; //shared notifications queue
private:
	Notifications() {};
};