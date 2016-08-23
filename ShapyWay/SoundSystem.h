#pragma once
#include <SFML\Audio.hpp>
#include "SharedRes.h"

static struct SoundSystem
{
	static sf::Music* currentMusic;

	static void init();
	static void playMusic(std::string name);
	static void stopMusic();
	static void setMusicVolume(int volume);
private:
	SoundSystem();
};