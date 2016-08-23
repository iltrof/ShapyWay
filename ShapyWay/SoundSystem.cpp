#include "SoundSystem.h"
#include "Settings.h"

SoundSystem* SoundSystem::instance = nullptr;

void SoundSystem::init()
{
	currentMusic = NULL;
}

void SoundSystem::playMusic(std::string name)
{
	currentMusic = new sf::Music;
	currentMusic->openFromFile(SharedRes::Get()->programDirectory+"res/audio/"+name);
	currentMusic->play();
	currentMusic->setLoop(true);
	setMusicVolume((int)Settings::Get()->musicVolume);
}

void SoundSystem::stopMusic()
{
	currentMusic->stop();
	delete currentMusic;
	currentMusic = NULL;
}

void SoundSystem::setMusicVolume(int volume)
{
	if(currentMusic != NULL)
		currentMusic->setVolume((float)volume);
}