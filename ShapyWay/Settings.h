#pragma once

class Settings
{
public:
	~Settings() { instance = nullptr; }

	static Settings* Get() 
	{
		if (instance == nullptr)
			instance = new Settings;
		return instance;
	}

	float soundVolume; //Sound volume (0-100)
	float musicVolume; //Music volume (0-100)
private:
	Settings() {}
	static Settings* instance;
};