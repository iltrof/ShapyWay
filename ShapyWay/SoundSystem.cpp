#include "SoundSystem.h"
#include "Settings.h"

sf::Music* SoundSystem::currentMusic;

void SoundSystem::init()
{
	currentMusic = NULL;
}

void SoundSystem::playMusic(std::string name)
{
	currentMusic = new sf::Music;
	currentMusic->openFromFile(SharedRes::programDirectory+"res/audio/"+name);
	currentMusic->play();
	currentMusic->setLoop(true);
	setMusicVolume(Settings::musicVolume);
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
		currentMusic->setVolume(volume);
}