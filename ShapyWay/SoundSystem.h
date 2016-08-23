#pragma once
#include <SFML\Audio.hpp>
#include "SharedRes.h"

class SoundSystem
{
public:
	~SoundSystem() { instance = nullptr; }

	static SoundSystem* Get() 
	{
		if (instance == nullptr)
			instance = new SoundSystem;
		return instance;
	}

	sf::Music* currentMusic;

	void init();
	void playMusic(std::string name);
	void stopMusic();
	void setMusicVolume(int volume);
private:
	SoundSystem() {}
	static SoundSystem* instance;
};