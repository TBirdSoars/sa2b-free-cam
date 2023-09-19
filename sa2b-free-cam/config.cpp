#include "pch.h"
#include "SA2ModLoader.h"
#include "IniFile.hpp"

namespace config
{
	bool menu_enabled = true;
	float min_dist    = 16.0f;
	float max_dist    = 50.0f;
	float height      = 10.5f;
	int analog_spd    = 0x40;
	int analog_max    = 0x100;
	bool invert_axis_x = false;
	bool invert_axis_y = false;

	void read(const char* path)
	{
		const IniFile* config = new IniFile(std::string(path) + "\\config.ini");
		const IniGroup* general = config->getGroup("");

		if (general)
		{
			menu_enabled = general->getBool("Menu", menu_enabled);
			min_dist     = max(0.0f, general->getFloat("MinDist", min_dist));
			max_dist     = max(min_dist, general->getFloat("MaxDist", max_dist));
			height       = general->getFloat("Height", height);
			analog_spd   = max(0, min(2000, general->getInt("AnalogThreshold", analog_spd)));
			analog_max   = max(analog_spd, min(2000, general->getInt("AnalogMaxSpeed", analog_max)));
			invert_axis_x = general->getBool("InvertAxisX", invert_axis_x);
			invert_axis_y = general->getBool("InvertAxisY", invert_axis_y);
		}

		delete config;
	}
}